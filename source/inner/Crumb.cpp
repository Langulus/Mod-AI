///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Crumb.hpp"
#include "../AI.hpp"
#include <Math/Number.hpp>

#define CRUMB_SEEK_VERBOSE(...)  //Logger::Verbose(__VA_ARGS__)
#define CRUMB_BUILD_VERBOSE(...) //Logger::Verbose(__VA_ARGS__)


/// Constructor                                                               
///   @param id - the ID of the crumb                                         
///   @param parent - the parent of the crumb                                 
///   @param rating - the crumb's rating                                      
///   @param pstart - the start of the crumb's data in decoder raw data       
///   @param pend - the end of the crumb's data in decoder raw data           
Crumb::Crumb(const Idea::Id& id, const Idea::Id& parent, Real rating, Offset pstart, Offset pend)
   : mID {id}
   , mDataStart {pstart}
   , mDataEnd {pend}
   , mRating {rating}
   , mLongestPath {pend - pstart} {
   if (parent)
      mParents << parent;
}

/// Integrate a new pattern inside the crumb's kids                           
///   @param pd - the pattern decoder                                         
///   @param pattern - the byte pattern to build                              
///   @param progress - the starting index inside pattern bytes               
///   @param end - the ending index inside pattern bytes                      
///   @param newlyBuilt - [out] whether or not a new crumb was built          
///   @return the new idea, or badIdea on error                               
Idea::Id Crumb::Build(Mind* pd, const Bytes& pattern, Offset progress, const Offset end, bool& newlyBuilt) {
   // Check how many characters match                                   
   const auto tsize = end - progress;
   const auto matches = pcMatchBytes(
      pattern.GetBytes() + progress, tsize,
      pd->mRaw.GetRaw() + mDataStart, GetLength()
   );

   if (matches == 0)
      return 0;

   progress += matches;

   // Do an early return if match is full for whole pattern             
   if (progress == end and matches == GetLength()) {
      newlyBuilt = false;
      return mID;
   }

   if (matches < GetLength()) {
      // Match is only partial in this. We need to split this crumb     
      // The current one will become the next one. Everything remains   
      // linked to this crumb so that we don't turn our ideas into mush 
      // because of id mismatches.                                      
      const auto ratio = real(matches) / real(GetLength());
      auto newcrumb = pd->CreateCrumb(1, mDataStart, mDataStart + matches);
      CRUMB_BUILD_VERBOSE("Splitting " << mID << " into " << newcrumb->mID << " and " << mID);

      newcrumb->SetParents(pd, mParents);
      ResetParents(pd);
      AddParent(pd, newcrumb->mID);
      mDataStart += matches;
      newcrumb->mRating += mRating * (1 - ratio);
      mRating += ratio;

      // The rest of the pattern is attached to newcrumb                
      const auto leftover = end - progress;
      if (leftover == 0) {
         // Match was partial, but the whole pattern is traversed       
         CRUMB_BUILD_VERBOSE("Splitting was enough");
         newlyBuilt = true;
         return newcrumb->mID;
      }

      const auto cached = pd->Cache(pattern.Crop(progress, leftover));
      auto branch = pd->CreateCrumb(1, cached, cached + leftover);
      branch->AddParent(pd, newcrumb->mID);
      CRUMB_BUILD_VERBOSE(pcLogVerbose
         << "Attached " << branch->mID << " to " << newcrumb->mID);
      newlyBuilt = true;
      return branch->mID;
   }

   // At this point match is full                                       
   // Delegate build process to crumbs, they might adopt it             
   for (const auto& child : mChildren) {
      auto childPtr = pd->GetCrumb(child);
      const auto result = childPtr->Build(pd, pattern, progress, end, newlyBuilt);
      if (result) {
         // Pattern was built somewhere inside children                 
         return result;
      }
   }

   // At this point no child was able to adopt the rest of pattern      
   // and we have to branch off in a new child                          
   const auto leftover = end - progress;
   const auto cached = pd->Cache(pattern.Crop(progress, leftover));
   auto branch = pd->CreateCrumb(1, cached, cached + leftover);
   branch->AddParent(pd, mID);
   CRUMB_BUILD_VERBOSE("Attached " << branch->mID << " to " << mID);
   newlyBuilt = true;
   return branch->mID;
}

/// Seek a pattern and collect all data associations on the way               
///   @param pd - the pattern decoder                                         
///   @param pattern - the byte pattern to seek                               
///   @param progress - the starting index inside pattern bytes               
///   @param end - the ending index inside pattern bytes                      
///   @return sought idea, or badIdea if not found                            
Idea::Id Crumb::Seek(Mind* pd, const Bytes& pattern, Offset progress, const Offset end) {
   // Check how many characters match                                   
   const auto tsize = end - progress;
   const auto matches = pcMatchBytes(
      pattern.GetBytes() + progress, tsize,
      pd->mRaw.GetRaw() + mDataStart, GetLength()
   );

   const auto ratio = real(matches) / real(GetLength());
   mRating += ratio;

   if (matches == GetLength()) {
      progress += matches;
      if (progress == end) {
         // Full match here, so we found what we seek                   
         return mID;
      }

      // Partial match found, so propagate to children                  
      for (const auto& child : mChildren) {
         auto childPtr = pd->GetCrumb(child);
         const auto result = childPtr->Seek(pd, pattern, progress, end);
         if (result) {
            // Full match somewhere in children                         
            return result;
         }
      }
   }

   return 0;
}

/// Check if crumb has a child with the given ID                              
///   @param id - the idea to search for                                      
///   @return true if this crumb has the given idea as a child                
bool Crumb::HasChild(const Idea::Id& id) const {
   return mChildren.Contains(id);
}

/// Check if crumb has a parent with the given ID                             
///   @param id - the idea to search for                                      
///   @return true if this crumb has the given idea as a parent               
bool Crumb::HasParent(const Idea::Id& id) const {
   return mParents.Contains(id);
}

/// Recalculate mLongestPath based on children (not nested)                   
///   @param decoder - the decoder to use to get crumbs                       
void Crumb::RecalculateLongestPath(Mind* decoder) {
   Crumb* crumb;
   mLongestPath = GetLength();
   for (const auto& childId : mChildren) {
      crumb = decoder->GetCrumb(childId);
      mLongestPath = ::std::max(mLongestPath, crumb->mLongestPath);
   }
}

/// Add a child crumb                                                         
///   @param decoder - the pattern decoder to use                             
///   @param id - the idea to add as child                                    
void Crumb::AddChild(Mind* decoder, const Idea::Id& id) {
   if (HasChild(id))
      return;

   auto crumb = decoder->GetCrumb(id);
   if (crumb->HasParent(mID))
      return;

   crumb->mParents << mID;
   mChildren << id;
   mLongestPath = ::std::max(mLongestPath, GetLength() + crumb->mLongestPath);
}

/// Remove child                                                              
///   @param decoder - the pattern decoder to use                             
///   @param id - the idea to remove from children                            
void Crumb::RemoveChild(Mind* decoder, const Idea::Id& id) {
   mChildren.Remove(id);
   auto crumb = decoder->GetCrumb(id);
   crumb->mParents.Remove(mID);
   RecalculateLongestPath(decoder);
}

/// Remove child                                                              
///   @param decoder - the pattern decoder to use                             
///   @param id - the idea to remove from parents                             
void Crumb::RemoveParent(Mind* decoder, const Idea::Id& id) {
   mParents.Remove(id);
   auto crumb = decoder->GetCrumb(id);
   crumb->mChildren.Remove(mID);
   crumb->RecalculateLongestPath(decoder);
}

/// Reset all parents                                                         
///   @param decoder - the pattern decoder to use                             
void Crumb::ResetParents(Mind* decoder) {
   for (auto& parent : mParents) {
      auto parentId = decoder->GetCrumb(parent);
      parentId->RemoveChild(decoder, mID);
   }
}

/// Reset all children                                                        
///   @param decoder - the pattern decoder to use                             
void Crumb::ResetChildren(Mind* decoder) {
   for (auto& child : mChildren) {
      auto childId = decoder->GetCrumb(child);
      childId->RemoveParent(decoder, mID);
   }

   mLongestPath = GetLength();
}

/// Set the crumb parent                                                      
///   @param decoder - the pattern decoder to use                             
///   @param ids - the ideas to set as parents                                
void Crumb::SetParents(Mind* decoder, const Ideas& ids) {
   ResetParents(decoder);
   for (auto& p : ids)
      AddParent(decoder, p);
}

/// Set the crumb parent                                                      
///   @param decoder - the pattern decoder to use                             
///   @param id - the idea to add to parents                                  
void Crumb::AddParent(Mind* decoder, const Idea::Id& id) {
   if (HasParent(id))
      return;

   auto parent = decoder->GetCrumb(id);
   if (parent->HasChild(mID))
      return;

   parent->mChildren << mID;
   parent->mLongestPath = ::std::max(parent->mLongestPath, parent->GetLength() + mLongestPath);
   mParents << id;
}

/// Set the children of the crumb                                             
///   @param decoder - the pattern decoder to use                             
///   @param ids - the ideas to set as children                               
void Crumb::SetChildren(Mind* decoder, const Ideas& ids) {
   ResetChildren(decoder);
   for (auto& p : ids)
      AddChild(decoder, p);
}

/// Check if crumb has a given association                                    
///   @param n - the idea to check if inside associations                     
///   @return true if the idea is inside list of associations                 
bool Crumb::HasAssociation(const Idea::Id& n) const {
   return mAssociations.Contains(n);
}

/// Check if crumb has a given disassociation                                 
///   @param n - the idea to check if inside disassociations                  
///   @return true if the idea is inside list of disassociations              
bool Crumb::HasDisassociation(const Idea::Id& n) const {
   return mDisassociations.Contains(n);
}

/// Associate this crumb with some data. Symmetic association                 
///   @param aid - the idea to insert in associations                         
void Crumb::Associate(const Idea::Id& aid) {
   if (HasAssociation(aid))
      return;

   mAssociations << aid;
   CRUMB_SEEK_VERBOSE("Decoder: ", Logger::Cyan, mID, Logger::Gray,
                      " now synonym to ", Logger::Cyan, aid);
}

/// Associate this crumb with some data. Symmetic association                 
///   @param aid - the idea to insert in disassociations                      
void Crumb::Disassociate(const Idea::Id& aid) {
   if (HasDisassociation(aid))
      return;

   mDisassociations << aid;
   CRUMB_SEEK_VERBOSE("Decoder: ", Logger::Cyan << mID, Logger::Gray,
                      " now antonym to ", Logger::Cyan, aid);
}