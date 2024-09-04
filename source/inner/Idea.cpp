///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Idea.hpp"
#include "Ontology.hpp"


/// Constructor                                                               
///   @param producer - the idea producer                                     
///   @param data - the idea's descriptor                                     
Idea::Idea(Ontology* producer, const Neat& data)
   : ProducedFrom {producer, data} {
   VERBOSE_AI_BUILD("Defining idea for: ", data);
}

/// Detach all ideas before destroying them to avoid circular dependencies    
void Idea::Detach() {
   mAssociations.Reset();
   mDisassociations.Reset();
   mDescriptor.Reset();
}

/// Get the ontology interface                                                
///   @return the ontology that produced this idea                            
auto Idea::GetOntology() const -> Ontology* {
   return mProducer;
}

/// Associate/disassociate ideas                                              
///   @param verb - the association verb                                      
void Idea::Associate(Verb& verb) {
   // Notice, that an idea can have the same associations and dis-      
   // associations. This hints at a paradox and is handled in a         
   // context-dependent way later, when using the ideas.                
   // Nothing really forces us to not allow paradoxes to exist.         
   // Our formal system has to consider them - they're part of reality  
   // after all. Remember our motto: "if you can say it, we have to     
   // be able to represent it"... so here you are - a paradox in        
   // the context of Langulus is defined as an idea that points to the  
   // same associations and disassociations.                            
   if (verb.GetMass() > 0) {
      // Associating                                                    
      if (verb.IsDeep()) {
         TODO();     // must generate a new idea and associate with that
                     // preserve hierarchy!
      }
      else verb.ForEach([&](Idea* idea) {
         if (idea->GetOntology() != GetOntology()) {
            Logger::Error(
               "Can't associate ideas produced from different ontologies."
               "Trying to associate ", *this, " with ", *idea
            );
            return;  //TODO transfer idea somehow? 
                     // should generally happen through communication though
         }

         // Associate symmetrically                                     
         mAssociations <<= idea;
         idea->mAssociations <<= this;
         verb << this;
         Logger::Info(Logger::Green, "Associated ", *this, " with ", *idea);
      });
   }
   else if (verb.GetMass() < 0) {
      // Disassociating                                                 
      if (verb.IsDeep()) {
         TODO();     // must generate a new idea and disassociate with that 
                     // preserve hierarchy!
      }
      else verb.ForEach([&](Idea* idea) {
         if (idea->GetOntology() != GetOntology()) {
            Logger::Error(
               "Can't disassociate ideas produced from different ontologies."
               "Trying to disassociate ", *this, " from ", *idea
            );
            return;  //TODO transfer idea somehow? 
                     // should generally happen through communication though
         }

         // Disassociate symmetrically                                  
         mDisassociations <<= idea;
         idea->mDisassociations <<= this;
         verb << this;
         Logger::Info(Logger::Green, "Disassociated ", *this, " from ", *idea);
      });
   }
}

/// Compare ideas                                                             
/// Ideas are the same, if RHS is the same, or an association to LHS          
/// They don't need to match exactly. The natural language equivalent to this 
/// is simply asking "is swan a bird?". It will be, if "swan" contains "bird".
/// Here's how this looks in code:                                            
///                                                                           
///   { ##swan == ##bird } will return ##swan if true, or nothing if not      
///                                                                           
///   @param verb - the compare verb                                          
void Idea::Compare(Verb& verb) const {
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
/// for an idea                                                               
///   @param what - the idea to search for                                    
///   @param mask - a set of covered ideas to avoid infinite regresses        
///   @return the idea which contained the association (if found)             
const Idea* Idea::AdvancedCompare(const Idea* what, IdeaSet& mask) const {
   if (mask.Contains(this))
      return nullptr;

   mask << this;
   
   // Make sure that the idea is never found in any disassociations     
   for (auto idea : mDisassociations) {
      if (idea == what or idea->AdvancedCompare(what, mask))
         return nullptr;
   }

   // Check if idea is found down the associations rabbit hole          
   for (auto idea : mAssociations) {
      if (idea == what or idea->AdvancedCompare(what, mask))
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

/// Integrate a new pattern inside the crumb's kids                           
///   @param pd - the ontology where new crumbs will be built                 
///   @param pattern - the byte pattern to build                              
///   @param progress - the starting index inside pattern bytes               
///   @param end - the ending index inside pattern bytes                      
///   @param newlyBuilt - [out] whether or not a new crumb was built          
///   @return the new idea, or badIdea on error                               
/*Idea* Idea::Build(
   Ontology& pd, const Bytes& pattern,
   Offset progress, const Offset end, bool& newlyBuilt
) {
   // Check how many bytes match                                        
   const auto tsize = end - progress;
   const auto matches = pattern.Select(progress, tsize).Matches(mData);
   if (matches == 0)
      return nullptr;

   progress += matches;

   // Do an early return if match is full for whole pattern             
   if (progress == end and matches == mData.GetCount()) {
      newlyBuilt = false;
      return this;
   }

   if (matches < mData.GetCount()) {
      // Match is only partial in this crumb, so we need to split it    
      // The current one will become the next one. Everything remains   
      // linked to this crumb so that we don't turn our ideas into mush 
      // because of id mismatches - memory isn't allowed to move.       
      const auto ratio = static_cast<Real>(matches)
                       / static_cast<Real>(mData.GetCount());
      auto newcrumb = pd.CreateIdea(1, mData.Select(0, matches));
      VERBOSE_AI_BUILD("Splitting ", this, ": ", newcrumb, " <-> ", this);

      newcrumb->SetParents(mParents);
      ResetParents();
      AddParent(newcrumb);
      mData = mData.Select(matches, mData.GetCount() - matches);
      newcrumb->mRating += mRating * (1 - ratio);
      mRating += ratio;

      // The rest of the pattern is attached to newcrumb                
      const auto leftover = end - progress;
      if (leftover == 0) {
         // Match was partial, but the whole pattern is traversed       
         VERBOSE_AI_BUILD("Splitting was enough");
         newlyBuilt = true;
         return newcrumb;
      }

      auto branch = pd.CreateIdea(1, pattern.Select(progress, leftover));
      branch->AddParent(newcrumb);
      VERBOSE_AI_BUILD("Attached ", branch, " to ", newcrumb);
      newlyBuilt = true;
      return branch;
   }

   // At this point match is full                                       
   // Delegate build process to crumbs, they might adopt it             
   for (auto& child : mChildren) {
      const auto result = child->Build(pd, pattern, progress, end, newlyBuilt);
      if (result) {
         // Pattern was built somewhere inside children                 
         return result;
      }
   }

   // At this point no child was able to adopt the rest of pattern      
   // and we have to branch off in a new child                          
   const auto leftover = end - progress;
   auto branch = pd.CreateIdea(1, pattern.Select(progress, leftover));
   branch->AddParent(this);
   VERBOSE_AI_BUILD("Attached ", branch, " to ", this);
   newlyBuilt = true;
   return branch;
}

/// Seek a pattern and collect all data associations on the way               
///   @param pd - the ontology where new crumbs will be built                 
///   @param pattern - the byte pattern to seek                               
///   @param progress - the starting index inside pattern bytes               
///   @param end - the ending index inside pattern bytes                      
///   @return sought idea, or badIdea if not found                            
Idea* Idea::Seek(
   Ontology& pd, const Bytes& pattern, Offset progress, const Offset end
) {
   // Check how many characters match                                   
   const auto tsize = end - progress;
   const auto matches = pattern.Select(progress, tsize).Matches(mData);
   const auto ratio = static_cast<Real>(matches)
                    / static_cast<Real>(mData.GetCount());
   mRating += ratio;

   if (matches == mData.GetCount()) {
      progress += matches;
      if (progress == end) {
         // Full match here, so we found what we seek                   
         return this;
      }

      // Partial match found, so propagate to children                  
      for (auto& child : mChildren) {
         const auto result = child->Seek(pd, pattern, progress, end);
         if (result) {
            // Full match somewhere in children                         
            return result;
         }
      }
   }

   return nullptr;
}*/

/// Used for logging                                                          
///   @return text representing this idea                                     
Text Idea::Self() const {
   return (operator Text()) + ": ";
}

/// Convert an Idea to Text                                                   
Idea::operator Text() const {
   return IdentityOf("Idea", this);
}
