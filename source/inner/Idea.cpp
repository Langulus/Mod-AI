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
///   @param parent - the parent idea                                         
///   @param rating - the idea's rating                                       
///   @param data - the idea's data                                           
Idea::Idea(Idea* parent, Rating rating, const Bytes& data)
   : mRating {rating}
   , mData   {data} {
   if (parent) {
      LANGULUS_ASSUME(DevAssumes, not mParents.Contains(parent),
         "Parent is already linked");
      LANGULUS_ASSUME(DevAssumes, not parent->mChildren.Contains(this),
         "Child is already linked");
      mParents << parent;
      parent->mChildren << this;
   }
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

/// Check if the crumb has parents                                            
///   @return true if crumb has parents                                       
bool Idea::IsOrphan() const noexcept {
   return mParents.IsEmpty();
}

/// Check if the crumb has children                                           
///   @return true if crumb has children                                      
bool Idea::IsStump() const noexcept {
   return mChildren.IsEmpty();
}

/// Get the bytesize of the crumb                                             
///   @return the size of the crumb in bytes                                  
auto Idea::GetData() const noexcept -> const Bytes& {
   return mData;
}

/// Get the list of associations                                              
///   @return the contained associations                                      
auto Idea::GetAssociations() const noexcept -> const Ideas& {
   return mAssociations;
}

/// Check if crumb has a specific child                                       
///   @param crumb - the crumb to search for                                  
///   @return true if this crumb has the child                                
bool Idea::HasChild(const Idea* crumb) const {
   return mChildren.Contains(crumb);
}

/// Check if crumb has a specific parent                                      
///   @param crumb - the crumb to search for                                  
///   @return true if this crumb has the parent                               
bool Idea::HasParent(const Idea* crumb) const {
   return mParents.Contains(crumb);
}

/// Add a child crumb                                                         
///   @param crumb - the child crumb to add                                   
void Idea::AddChild(Idea* crumb) {
   if (HasChild(crumb))
      return;
   crumb->mParents << this;
   mChildren << crumb;
}

/// Remove child crumb                                                        
///   @param crumb - the child crumb to remove                                
void Idea::RemoveChild(Idea* crumb) {
   mChildren.Remove(crumb);
   crumb->mParents.Remove(this);
}

/// Remove parent                                                             
///   @param crumb - the parent crumb to remove                               
void Idea::RemoveParent(Idea* crumb) {
   mParents.Remove(crumb);
   crumb->mChildren.Remove(this);
}

/// Reset all parents                                                         
void Idea::ResetParents() {
   for (auto& parent : mParents)
      parent->mChildren.Remove(this);
   mParents.Clear();
}

/// Reset all children                                                        
void Idea::ResetChildren() {
   for (auto& child : mChildren)
      child->mParents.Remove(this);
   mChildren.Clear();
}

/// Set the crumb parent                                                      
///   @param parents - the parents to overwrite with                          
void Idea::SetParents(const Ideas& parents) {
   ResetParents();
   for (auto& p : parents)
      AddParent(p);
}

/// Add crumb parent                                                          
///   @param parent - the parent to add                                       
void Idea::AddParent(Idea* parent) {
   if (HasParent(parent))
      return;

   parent->mChildren << this;
   mParents << parent;
}

/// Set the children of the crumb                                             
///   @param children - the children to overwrite with                        
void Idea::SetChildren(const Ideas& children) {
   ResetChildren();
   for (auto& p : children)
      AddChild(p);
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
Idea* Idea::Build(
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
      auto newcrumb = pd.CreateCrumb(1, mData.Select(0, matches));
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

      auto branch = pd.CreateCrumb(1, pattern.Select(progress, leftover));
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
   auto branch = pd.CreateCrumb(1, pattern.Select(progress, leftover));
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
}