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

/// Associate/disassociate ideas                                              
///   @param verb - the association verb                                      
void Idea::Associate(Verb& verb) {
   TODO();
}

/// Compare ideas                                                             
///   @param verb - the compare verb                                          
void Idea::Compare(Verb& verb) const {
   TODO();
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
