#include "Ontology.hpp"
#include "Pattern.hpp"


/// Default ontology constructor                                              
Ontology::Ontology() {}

/// Ontology descriptor constructor                                           
///   @param d - descriptor                                                   
Ontology::Ontology(Describe&&) {
   TODO();
}

/// Ideas have their own hierarchy and circular references, and need to be    
/// teared down before we're able to reset them                               
void Ontology::Teardown() {
   mIdeas.Teardown();
}

/// Create/destroy ideas through a verb                                       
///   @param verb - the verb                                                  
void Ontology::Create(Verb& verb) {
   mIdeas.Create(this, verb);
}

/// Build an idea representing a hierarchy                                    
///   @param data - the hierarchy to represent                                
///   @param findMetapatterns - search for all metapatterns before building   
///      this way any previously defined complex ideas will be reused, making 
///      the database denser and smaller. But it costs more time...           
///   @return the idea representing the data                                  
auto Ontology::Build(const Many& data, bool findMetapatterns) -> Idea* {
   Ideas coalesced;
   if (data.IsOr())
      coalesced.MakeOr();

   if (data.IsDeep()) {
      // Represent each group as an idea                                
      data.ForEach([&](const Many& group) {
         auto idea = Build(group);
         if (idea)
            coalesced << idea;
      });
   }
   else if (data.Is<Idea>()) {
      // Ideas are forwarded directly                                   
      data.ForEach([&](const Idea& idea) {
         coalesced << &const_cast<Idea&>(idea);
      });
   }
   else if (data.Is<Code>()) {
      // Code needs to be normalized further, by compiling it. Can't    
      // just register it as it is - spaces, cases and optional brackets
      // will produce different hashes and overcomplicate the database. 
      data.ForEach([&](const Code& code) {
         auto idea = Build(code.Parse());
         if (idea)
            coalesced << idea;
      });
   }
   else if (data.CastsTo<Text>()) {
      // Text data needs to be sanitized further                        
      data.ForEach([&](const Text& text) {
         Idea* idea;

         if (findMetapatterns) {
            // Interpret the text and find all possible metapatterns    
            auto pattern = Interpret(text);
            while (FindMetapatterns(pattern))
               ;

            // Then build an idea from the complex pattern hierarchy    
            idea = Build(pattern);
         }
         else idea = BuildText(text);

         if (idea)
            coalesced << idea;
      });
   }
   else {
      // Anything else gets normalized conventionally using Neat        
      return mIdeas.CreateOne(this, Neat {data});
   }
   
   // Combine all the generated ideas into one and return               
   if (coalesced) {
      if (coalesced.GetCount() == 1)
         return coalesced[0];

      if (findMetapatterns) {
         // Find all possible metapatterns                              
         Many pattern = coalesced;
         bool metapatternsFound = false;
         while (FindMetapatterns(pattern))
            metapatternsFound = true;

         // Then build an idea from the complex pattern hierarchy       
         if (metapatternsFound)
            return mIdeas.CreateOne(this, Neat {pattern});
      }

      // Build an idea from the flat idea sequence                      
      return mIdeas.CreateOne(this, Neat {coalesced});
   }

   // If reached, then no idea was built (empty data?)                  
   return {};
}

/// Build a text pattern                                                      
///   @param text - pattern to build idea for                                 
///   @return the idea representing the text                                  
auto Ontology::BuildText(const Text& text) -> Idea* {
   // Everytime we build a text pattern, build a sanitized variant      
   // (no capital letters) and link it as an association implicitly.    
   //TODO sanitize away invalid symbols?
   auto lower = text.Lowercase();
   if (lower != text) {
      // Create two ideas and associate them                            
      // We can't afford to lose the original information, but we also  
      // want to be able to find loose matches                          
      auto i1 = mIdeas.CreateOne(this, text);
      auto i2 = mIdeas.CreateOne(this, lower);
      i1->Associate(i2);
      return i1;
   }

   // Text is already sanitized, just return its idea                   
   return mIdeas.CreateOne(this, text);
}

/// Interpret some text                                                       
///   @param text - text to interpret                                         
///   @attention text is assumed lowercased!                                  
///   @return the hierarchy of ideas in the text                              
auto Ontology::Interpret(const Text& text) const -> Many {
   // Since this is a natural language module, plausible interpret-     
   // ations may overlap, and are later weighted and filtered by        
   // context.                                                          
   Many result;

   for (Offset i = text.GetCount(); i > 0; --i) {
      // There are text.GetCount() possible head patterns               
      auto token = text.Select(0, i);
      auto lower = token.Lowercase();
      
      // Figure out the head pattern                                    
      Many head;
      if (token == lower) {
         auto idea = mIdeas.Find(token);
         if (idea)   head = idea;
         else        head = token;
      }
      else {
         auto idea1 = mIdeas.Find(token);
         auto idea2 = mIdeas.Find(lower);
         if (idea1 and idea2) {
            head << idea1 << idea2;
            head.MakeOr();
         }
         else if (idea1) {
            head << idea1;
         }
         else if (idea2) {
            head << token << idea2;
            head.MakeOr();
         }
         else head = token;
      }

      if (i < text.GetCount()) {
         // Nest for the tail - optimize whenever possible by grouping  
         // similar data                                                
         auto tail = Interpret(lower.Select(i));

         if (tail.IsSimilar(head) and not head.IsOr() and not tail.IsOr()) {
            if (tail.Is<Text>()) {
               // We can optimize further by concatenating texts        
               Text concatenated;
               head.ForEach([&concatenated](const Text& t) {
                  concatenated += t;
               });
               tail.ForEach([&concatenated](const Text& t) {
                  concatenated += t;
               });

               head = concatenated;
            }
            else head.InsertBlock(IndexBack, tail);
         }
         else {
            if (head.Is<Text>() and not head.IsOr() and tail.IsDeep()) {
               // Other optimization worthy cases, like:                
               // (text1, (text2, idea)) -> (text1+text2, idea)         
               Text concatenated;

               // Concat all texts in head                              
               head.ForEach([&concatenated](const Text& t) {
                  concatenated += t;
               });

               // Concat all texts in tail (deeply)                     
               tail.ForEachDeep<false, false>([&concatenated](Many& group) {
                  if (group.IsDeep() and not group.IsOr()) {
                     // Skip intermediate groups unless they branch     
                     return Loop::Continue;
                  }

                  if (not group.Is<Text>() or group.IsOr()) {
                     // Abort on type mismatch/branch                   
                     return Loop::Break;
                  }

                  group.ForEach([&concatenated](const Text& t) {
                     concatenated += t;
                  });

                  // Discard the group after being consumed from tail   
                  return Loop::Discard;
               });

               head = concatenated;
            }
            else if (head.Is<Idea>() and not head.IsOr() and tail.IsDeep()) {
               //TODO this can be generalized for any type i think
               // Other optimization worthy cases, like:                
               // (idea1, (idea2, idea3)) -> (idea1, idea2, idea3)      
               Many concatenated;

               // Concat all ideas in head                              
               head.ForEach([&concatenated](Idea* t) {
                  concatenated << t;
               });

               // Concat all ideas in tail (deeply)                     
               tail.ForEachDeep<false, false>([&concatenated](Many& group) {
                  if (group.IsDeep() and not group.IsOr()) {
                     // Skip intermediate groups unless they branch     
                     return Loop::Continue;
                  }

                  if (not group.Is<Idea>() or group.IsOr()) {
                     // Abort on type mismatch/branch                   
                     return Loop::Break;
                  }

                  group.ForEach([&concatenated](Idea* t) {
                     concatenated << t;
                  });

                  // Discard the group after being consumed from tail   
                  return Loop::Discard;
               });

               head = concatenated;
            }
            else {
               // Can't optimize anything                               
               head << tail;
            }
         }
      }
      
      // Push the interpretation, avoid duplicating                     
      result <<= head;
   }

   if (result.GetCount() > 1)
      result.MakeOr();

   // This is how 'result' should look like at this point:              
   //    [0]: TEXT                                                      
   // or [1]: TEX, T                                                    
   // or [2]: TE, XT                                                    
   //         [0]: TE, XT                                               
   //      or [1]: TE, X, T                                             
   // or [3]: T, EXT                                                    
   //         [0]: T, EXT                                               
   //      or [1]: T, EX, T                                             
   //      or [2]: T, E, XT                                             
   //              [0]: T, E, XT                                        
   //           or [1]: T, E, X, T                                      
   //                                                                   
   // Similar items in non-or sequences will be optimized and flattened 
   return result;
}

/// Seek for more complex patterns that have been registered in the ontology  
/// and substitute until nothing remains. This when applied repeatedly        
/// effectively results in data that is composed of the highest-order ideas   
/// available in the ontology. Very useful when building ideas on top of      
/// other ideas! When building ideas, data normalization destroys hierarchy,  
/// and the only way to restore it is by using deeper and deeper ideas.       
///   @param data - [in/out] the patterns to scan                             
///   @return true if at least one pattern was found inside 'data'            
bool Ontology::FindMetapatterns(Many& data) const {
   bool atLeastOneMetapatternFound = false;

   data.ForEachDeep([&](Many& group) {
      for (Offset i = group.GetCount(); i > 1; --i) {
         auto pattern = group.Select(0, i);
         auto idea = mIdeas.Find(Neat {pattern});
         if (not idea)
            continue;
         
         // Metapattern found                                           
         atLeastOneMetapatternFound = true;
         auto substitution = Many::FromState(pattern);
         substitution << idea;

         if (i != group.GetCount()) {
            if (group.Is<Idea>()) {
               // Optimize if possible                                  
               group.Select(i, group.GetCount() - i)
                  .ForEach([&](Idea* tail) { substitution << tail; });
            }
            else substitution << group.Select(i, group.GetCount() - i);
         }

         // Substitude the pattern for the idea and move on             
         // Repeat processing this group - type might've changed!       
         group = substitution;
         return Loop::Repeat;
      }

      return Loop::Continue;
   });

   //TODO must flatten ideas wherever possible
   return atLeastOneMetapatternFound;
}
