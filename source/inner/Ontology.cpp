#include "Ontology.hpp"
#include "Pattern.hpp"


/// Default ontology constructor                                              
Ontology::Ontology() {}

/// Ontology descriptor constructor                                           
///   @param d - descriptor                                                   
Ontology::Ontology(Describe&&) {
   TODO();
}

/// Detach all referenced contents, so that circular dependencies are severed 
void Ontology::Detach() {
   for (auto& idea : mIdeas)
      idea.Detach();
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
      auto i1 = mIdeas.CreateOne(this, Neat {text});
      auto i2 = mIdeas.CreateOne(this, Neat {lower});
      i1->Associate(i2);
      return i1;
   }

   // Text is already sanitized, just return its idea                   
   return mIdeas.CreateOne(this, Neat {text});
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
   result.MakeOr();

   for (Offset i = text.GetCount(); i > 0; --i) {
      // There are text.GetCount() possible head patterns               
      auto token = text.Select(0, i);
      auto lower = token.Lowercase();
      
      // Figure out the head pattern                                    
      Many head;
      if (token == lower) {
         auto idea = mIdeas.Find(Neat {token});
         head = idea ? idea : token;
      }
      else {
         auto idea1 = mIdeas.Find(Neat {token});
         auto idea2 = mIdeas.Find(Neat {lower});
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

         if (tail.GetType() == head.GetType()
         and not head.IsOr() and not tail.IsOr()) {
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
            //TODO other optimization-worthy cases, like:
            // (text1, (text2, idea)) -> (text1+text2, idea)
            // can probably be achieved by concatenating sequential non-or ForEachDeep groups of the same type?
            head << tail;
         }
      }
      
      // Push the interpretation, avoid duplicating                     
      result <<= head;
   }

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
   return result;
}
