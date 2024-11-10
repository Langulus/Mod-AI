#include "Ontology.hpp"
#include "Pattern.hpp"


/// Default ontology constructor                                              
Ontology::Ontology(const A::AIUnit& c) : mOwner {c} {}

/// Ontology descriptor constructor                                           
///   @param d - descriptor                                                   
Ontology::Ontology(const A::AIUnit& c, const Many&) : mOwner {c} {
   TODO();
}

/// Ideas have their own hierarchy and circular references, and need to be    
/// teared down before we're able to reset them                               
Text Ontology::Self() const {
   return mOwner.Self();
}

/// Ideas have their own hierarchy and circular references, and need to be    
/// teared down before we're able to reset them                               
void Ontology::Teardown() {
   mCache.Reset();
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
   // Clear the cache every time we build a new pattern                 
   mCache.Clear();

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

         /*if (findMetapatterns) { //TODO use nested Interpet instead - should be more generic
            // Interpret the text and find all possible metapatterns    
            auto pattern = Interpret(text);
            while (FindMetapatterns(pattern))
               ;

            // Then build an idea from the complex pattern hierarchy    
            idea = mIdeas.CreateOne(this, pattern);
         }
         else*/ idea = BuildText(text);

         if (idea)
            coalesced << idea;
      });
   }
   else {
      // Anything else gets normalized conventionally using Neat        
      return mIdeas.CreateOne(this, data);
   }
   
   // Combine all the generated ideas into one and return               
   if (coalesced) {
      if (coalesced.GetCount() == 1)
         return coalesced[0];

      /*if (findMetapatterns) { //TODO use nested Interpet instead - should be more generic
         // Find all possible metapatterns                              
         Many pattern = coalesced;
         bool metapatternsFound = false;
         while (FindMetapatterns(pattern))
            metapatternsFound = true;

         // Then build an idea from the complex pattern hierarchy       
         if (metapatternsFound)
            return mIdeas.CreateOne(this, pattern);
      }*/

      // Build an idea from the flat idea sequence                      
      return mIdeas.CreateOne(this, coalesced);
   }

   // If reached, then no idea was built (empty data?)                  
   return {};
}

/// Build a text pattern                                                      
///   @param text - pattern to build idea for                                 
///   @return the idea representing the text                                  
auto Ontology::BuildText(const Text& text) -> Idea* {
   // Keep track of the longest known text pattern                      
   if (text.GetCount() > mLongestKnownText)
      mLongestKnownText = text.GetCount();

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

/// Optimize a sequence, extracting sequential entries of the given type and  
/// concatenating them and reappending them on the front, discarding the      
/// original from the provided data. Here's an example for text:              
/// (text1, (text2, idea)) -> (text1+text2, idea)                             
///   @param data - the sequence to optimize                                  
template<class FOR>
void Ontology::OptimizeFor(Many& data) const {
   if (data.IsOr())
      return;

   // Make sure FOR is concatenable (wrap it in a TMany if not)         
   Conditional<CT::Block<FOR>, FOR, TMany<FOR>> concatenated;

   data.template ForEachDeep<false, false>([&concatenated](Many& group) {
      if (group.IsOr())
         return Loop::Break;     // Abort on groups that branch         
      else if (group.IsDeep())
         return Loop::Continue;  // Just continue on otherwise deep     

      if (group.template IsSimilar<FOR>()) {
         // Consume any sequential data (even if deep)                  
         auto& g = reinterpret_cast<TMany<FOR>&>(group);
         for (const auto& relevant : g)
            concatenated += relevant;
         return Loop::Discard;
      }
      else return Loop::Break;
   });

   if constexpr (CT::Text<FOR>) {
      // Interpret the concatenated text again                          
      auto idea = mIdeas.Find(concatenated);
      if (idea) {
         data >> idea;
         return;
      }
   }

   if (data and concatenated)
      data >> Abandon(concatenated);
   else if (concatenated)
      data = Abandon(concatenated);
}

/// Interpret some text                                                       
///   @param text - text to interpret                                         
///   @return the hierarchy of ideas in the text                              
auto Ontology::Interpret(const Text& text) const -> Many {
   if (text.IsEmpty())
      return {};

   // Is the text available in the cache? Directly return it if so      
   VERBOSE_AI_INTERPRET_TAB("Interpreting: ", text);
   const auto cached = mCache.FindIt(text);
   if (cached) {
      VERBOSE_AI_INTERPRET("Cached: ", cached.GetValue());
      return cached.GetValue();
   }

   // Since this is a natural language module, plausible interpret-     
   // ations may overlap, and are later weighted and filtered by        
   // context.                                                          
   Many result;
   for (Offset i = 1; i <= text.GetCount(); ++i) {
      Many pattern;
      Text token = text.Select(0, i);
      auto lower = token.Lowercase();
      
      // Figure out the pattern                                         
      if (token == lower) {
         auto idea = mIdeas.Find(token);
         if (idea)
            pattern = idea;
      }
      else {
         auto idea1 = mIdeas.Find(token);
         auto idea2 = mIdeas.Find(lower);
         if (idea1 and idea2) {
            pattern << idea1 << idea2;
            pattern.MakeOr();
         }
         else if (idea1) {
            pattern << idea1;
         }
         else if (idea2) {
            pattern << token << idea2;
            pattern.MakeOr();
         }
      }

      if (not pattern and i != 1)
         continue;

      if (not pattern)
         pattern << token;

      // If an idea was found, than this is a worthy pattern            
      // Otherwise we push it ONLY if token is the smallest             
      // Nest for the tail - optimize whenever possible by grouping     
      // similar data                                                   
      if (i < text.GetCount()) {
         auto tail = Interpret(text.Select(i));
         pattern << Abandon(tail);

         OptimizeFor<Text>(pattern);
         OptimizeFor<Idea*>(pattern);
         pattern.Optimize(); //TODO remove this when auto-optimization starts to happen properly on Loop::Discard
      }

      // Cache and merge the interpretation                             
      VERBOSE_AI_INTERPRET("Final (previous): ", text, " -> ", result);
      VERBOSE_AI_INTERPRET("Final (optimized): ", text, " -> ", pattern);
      mCache.Insert(text, pattern);
      
      // Avoid duplication                                              
      bool found = false;
      result.ForEachDeep<false, false>([&](const Many& group) {
         if (group == pattern) {
            found = true;
            return Loop::Break;
         }
         return Loop::Continue;
      });

      if (not found) {
         // Push to front, because each new subtoken is longer          
         // and thus more likely to be the best one                     
         result >> Abandon(pattern);
      }
      else VERBOSE_AI_INTERPRET(Logger::Red, "Discarded: ", pattern);
   }

   if (result.GetCount() > 1)
      result.MakeOr();
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
/*bool Ontology::FindMetapatterns(Many& data) const {
   bool atLeastOneMetapatternFound = false;

   data.ForEachDeep([&](Many& group) {
      for (Offset i = group.GetCount(); i > 1; --i) {
         auto pattern = group.Select(0, i);
         auto idea = mIdeas.Find(pattern);
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
}*/
