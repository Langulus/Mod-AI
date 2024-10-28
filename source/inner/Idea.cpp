///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Idea.hpp"
#include "Ontology.hpp"
#include <cstring>


/// Constructor                                                               
///   @param producer - the idea producer                                     
///   @param data - the idea's descriptor                                     
Idea::Idea(Ontology* producer, const Many& data)
   : ProducedFrom {producer, data} {
   VERBOSE_AI_BUILD("Defining idea for: ", data);
}

/// Tear apart all ideas before destroying them to avoid circular dependencies
void Idea::Teardown() {
   mDisassociations.Reset();
   mAssociations.Reset();
}

/// Get the ontology interface                                                
///   @return the ontology that produced this idea                            
auto Idea::GetOntology() const -> Ontology* {
   return mProducer;
}

/// Associate/Disassociate with a single idea                                 
///   @tparam ASSOCIATE - true to associate, false to disassociate            
///   @param idea - the idea to (dis)associate with                           
///   @return true if the idea was added as (dis)association                  
template<bool ASSOCIATE>
bool Idea::LinkIdea(Idea* idea) {
   if (idea->GetOntology() != GetOntology()) {
      Logger::Error(
         "Can't (dis)associate ideas produced from different ontologies."
         "Trying to (dis)associate ", *this, " from/with ", *idea
      );
      return false;  //TODO transfer idea somehow? 
                     // should generally happen through communication though
   }

   // Always symmetrical                                                
   if constexpr (ASSOCIATE) {
      mAssociations <<= idea;
      idea->mAssociations <<= this;
      Logger::Info(Logger::Green, "Associated ", *this, " with ", *idea);
   }
   else {
      mDisassociations <<= idea;
      idea->mDisassociations <<= this;
      Logger::Info(Logger::Green, "Disassociated ", *this, " from ", *idea);
   }

   return true;
}

/// Inner (dis)association                                                    
///   @tparam ASSOCIATE - true to associate, false to disassociate            
///   @param verb - the verb to satisfy                                       
template<bool ASSOCIATE>
void Idea::AssociateInner(Verb& verb) {
   // Notice, that an idea can have the same associations and dis-      
   // associations. This hints at a paradox and is handled in a         
   // context-dependent way later, when using the ideas.                
   // Nothing really forces us to not allow paradoxes to exist.         
   // Our formal system has to consider them - they're part of reality  
   // after all. Remember our motto: "if you can say it, we have to     
   // be able to represent it"... so here you are - a paradox in        
   // the context of Langulus is defined as an idea that points to      
   // conflicting associations and disassociations.                     

   // Build an idea representing the argument, and (dis)associate       
   // with it instead                                                   
   auto idea = GetOntology()->Build(verb.GetArgument());
   if (LinkIdea<ASSOCIATE>(idea))
      verb << this;
}

/// Associate/disassociate ideas                                              
///   @param verb - the association verb                                      
void Idea::Associate(Verb& verb) {
   if (verb.GetMass() > 0)
      AssociateInner<true>(verb);
   else if (verb.GetMass() < 0)
      AssociateInner<false>(verb);
}

/// Compare ideas                                                             
/// Ideas are the same, if RHS is the same, or an association to LHS          
/// They don't need to match exactly. The natural language equivalent to this 
/// is simply asking "is swan a bird?". It will be, if "swan" contains "bird".
/// Here's how this looks in code:                                            
///                                                                           
///   { ##swan == ##bird } will return ##swan if true, or nothing if not      
///                                                                           
///   @param verb - the equals verb                                           
void Idea::Equal(Verb& verb) const {
   Count matches = 0;

   // Check if the required associations are available                  
   if (verb.IsDeep()) {
      TODO();     // must generate a new idea and check against that
                  // preserve hierarchy!
   }
   else verb.ForEach([&](Idea* idea) {
      if (this != idea and (not mAssociations.Contains(idea)
                            or  mDisassociations.Contains(idea))
      ) {
         // First order mismatch found, so ideas are not plainly similar
         // We have to do an advanced graph-walking comparison to make  
         // sure that there doesn't exist any indirect associations.    
         IdeaSet mask;
         if (not AdvancedCompare(idea, mask)) {
            // Full mismatch found, no point in going further           
            verb.Done();
            matches = 0;
            return Loop::Break;
         }
      }
      
      ++matches;
      return Loop::Continue;
   });

   if (matches)
      verb << this;
}

/// Iterates through all nested associations and disassociations in search    
/// for an idea. The name of the game is: find if we can walk from this idea  
/// to 'what' and vice versa, without hitting any disassociation on the way   
///   @param what - the idea to search for                                    
///   @param mask - a set of covered ideas to avoid infinite regresses        
///   @return the idea which contained the association (if found)             
auto Idea::AdvancedCompare(const Idea* what, IdeaSet& mask) const -> const Idea* {
   if (mask.Contains(this))
      return nullptr;

   mask << this;
   
   // Make sure that the idea is never found in any disassociations     
   for (auto idea : mDisassociations) {
      if (idea == what)
         return nullptr;
   }
   for (auto idea : what->mDisassociations) {
      if (idea == this)
         return nullptr;
   }

   // Check if idea is found down the associations rabbit hole          
   for (auto idea : mAssociations) {
      if (idea == what or (idea->AdvancedCompare(what, mask)
                       and what->AdvancedCompare(idea, mask)))
         return this;
   }

   // If reached, then no association was found                         
   return nullptr;
}

/// Compare crumb ratings                                                     
///   @param other - the crumb to compare against                             
///   @return true if left crumb has the higher rating                        
bool Idea::operator > (const Idea& other) const noexcept {
   return mRating > other.mRating;
}

/// Compare crumb ratings                                                     
///   @param other - the crumb to compare against                             
///   @return true if right crumb has the higher rating                       
bool Idea::operator < (const Idea& other) const noexcept {
   return mRating < other.mRating;
}

/// Get the list of associations                                              
///   @return the contained associations                                      
auto Idea::GetAssociations() const noexcept -> const Ideas& {
   return mAssociations;
}

/// Check if crumb has a given association                                    
///   @param n - the idea to check if inside associations                     
///   @return true if the idea is inside list of associations                 
bool Idea::HasAssociation(const Idea* n) const {
   return mAssociations.Contains(n);
}

/// Check if crumb has a given disassociation                                 
///   @param n - the idea to check if inside disassociations                  
///   @return true if the idea is inside list of disassociations              
bool Idea::HasDisassociation(const Idea* n) const {
   return mDisassociations.Contains(n);
}

/// Associate this crumb with some data. Symmetic association                 
///   @param n - the idea to insert in associations                           
void Idea::Associate(Idea* n) {
   if (HasAssociation(n))
      return;

   mAssociations << n;
   VERBOSE_AI_SEEK("Decoder: ", Logger::Cyan, this, Logger::Gray,
                   " now synonym to ", Logger::Cyan, n);
}

/// Associate this crumb with some data. Symmetic association                 
///   @param n - the idea to insert in disassociations                        
void Idea::Disassociate(Idea* n) {
   if (HasDisassociation(n))
      return;

   mDisassociations << n;
   VERBOSE_AI_SEEK("Decoder: ", Logger::Cyan, this, Logger::Gray,
                   " now antonym to ", Logger::Cyan, n);
}

/// Used for logging                                                          
///   @return text representing this idea                                     
Text Idea::Self() const {
   return (operator Text()) + ": ";
}

/// Convert an Idea to Text                                                   
Idea::operator Text() const {
   if (mDescriptor.Is<Text>() and mDescriptor.GetCount() == 1) {
      // Write a more meaningful representation                         
      auto& token = mDescriptor.Get<Text>();
      bool contains_symbols = false;
      for (auto c : token) {
         if (not ::std::isalpha(c)) {
            contains_symbols = true;
            break;
         }
      }

      if (not contains_symbols)
         return "##" + token;
      else
         return "##`" + token + "`";
   }

   // Write the identity if idea is complex                             
   return IdentityOf("Idea", this);
}