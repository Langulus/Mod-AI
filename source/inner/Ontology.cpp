#include "Ontology.hpp"


/// Integrate a new pattern inside the crumb's kids                           
///   @param pd - the pattern decoder                                         
///   @param pattern - the byte pattern to build                              
///   @param progress - the starting index inside pattern bytes               
///   @param end - the ending index inside pattern bytes                      
///   @param newlyBuilt - [out] whether or not a new crumb was built          
///   @return the new idea, or badIdea on error                               
Idea::Id Crumb::Build(Mind* pd, const Bytes& pattern, Offset progress, const Offset end, bool& newlyBuilt) {
   // Check how many bytes match                                        
   const auto tsize = end - progress;
   const auto matches = pattern.Select(progress, tsize)
      .Matches();

   pcMatchBytes(
      pattern.GetRaw() + progress, tsize,
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

/// Get a crumb interface with a given idea index									
///	@param index - the idea index														
///	@return the crumb or nullptr if index is badIdea							
Crumb* Ontology::GetCrumb(IdeaID index) noexcept {
   return !index ? nullptr : &mCrumbs[index.mID - 1];
}

/// Get a crumb interface with a given idea index (const)						
///	@param index - the idea index														
///	@return the crumb or nullptr if index is badIdea							
const Crumb* Ontology::GetCrumb(IdeaID index) const noexcept {
   return !index ? nullptr : &mCrumbs[index.mID - 1];
}

/// Associate/disassociate patterns                                           
///   @param verb - association verb                                          
void Ontology::Associate(Verb& verb) {
   const bool inverted = verb.GetMass() < 0;

   Many left;
   verb.Gather(left, Block::Past);
   if (left.IsInvalid()) {
      throw Except::AI(pcLogFuncError, "Bad left argument inside " << argument);
   }

   Many right;
   argument.Gather(right, Block::Future);
   if (right.IsInvalid()) {
      throw Except::AI(pcLogFuncError
         << "Bad right argument inside " << argument
         << " (left argument was " << left << ")");
   }

   Associate(inverted, left, right);
   verb.Done();
}

/// Dissect a pattern and integrate it in the available tissue                
///   @param pattern - the pattern to build                                   
///   @param newlyBuilt - [out] for debugging only                            
///   @return the built idea index                                            
Idea::Id Ontology::Build(const Bytes& pattern, bool& newlyBuilt) {
   for (auto& crumb : mCrumbs) {
      if (!crumb.IsOrphan())
         continue;

      const auto result = crumb.Build(this, pattern, 0, pattern.GetCount(), newlyBuilt);
      if (result) {
         BUILD_VERBOSE(if (newlyBuilt)
            pcLogVerbose << ccGreen << "Built pattern " << pattern);
         return result;
      }
   }

   // Pattern wasn't built as a child, so do it as an orphan            
   const auto cached = Cache(pattern);
   const auto addition = CreateCrumb(1, cached, cached + pattern.GetCount());
   BUILD_VERBOSE(pcLogVerbose << ccGreen << "Built pattern " << pattern);
   newlyBuilt = true;
   return addition->mID;
}

/// Recall cached pattern                                                     
///   @param pattern - the pattern to search for                              
///   @return the found idea index (or badIdea if not found)                  
Idea::Id Ontology::Seek(const Bytes& pattern) {
   for (auto& crumb : mCrumbs) {
      if (!crumb.IsOrphan())
         continue;

      const auto result = crumb.Seek(this, pattern, 0, pattern.GetCount());
      if (result) {
         SEEK_VERBOSE(ccGreen << "Pattern found " << pattern);
         return result;
      }
   }

   SEEK_VERBOSE(ccRed << "Seeking pattern failed " << pattern);
   return BadIdea;
}

/// Produce a new crumb                                                       
///   @param rating - crumb's initial rating                                  
///   @param pstart - starting byte (index in mRaw)                           
///   @param pend - ending byte (index in mRaw)                               
///   @return the new crumb                                                   
Crumb* Ontology::CreateCrumb(const real rating, const pcptr pstart, const pcptr pend) {
   // Push a new crumb                                                  
   const auto cid = mCrumbs.GetCount() + 1;
   mCrumbs << Crumb(cid, 0, rating, pstart, pend);
   mHeader = mCrumbs.GetCount();
   return &mCrumbs.Last();
}

/// Delete a crumb                                                            
///   @param index - the crumb index to remove                                
void Ontology::DeleteCrumb(const Idea::Id index) {
   auto crumb = GetCrumb(index);
   if (not crumb)
      return;

   // Mark everything under that crumb for deletion, included           
   // Since crumbs are rarely deleted, this is batched and done         
   // upon optimization of the database                                 
   crumb->mRating = 0;
   for (auto& n : crumb->mChildren)
      DeleteCrumb(n);
}

/// Cache some bytes into the pattern cache                                   
///   @param bytes - the bytes to cache                                       
///   @return the index at which the pattern has been cached inside mRaw      
Offset Ontology::Cache(const Bytes& bytes) {
   const auto starters = mRaw.GetCount();
   mRaw.Insert(bytes.GetBytes(), bytes.GetCount());
   return starters;
}

/// Represent a pattern as an idea                                            
///   @param pattern - the pattern to represent                               
///   @return the idea representing the pattern                               
Idea::Id Ontology::Represent(const Many& pattern) {
   Pattern result {this, pattern, true};

   // Collect all subpatterns inside the pattern                        
   // It is done in a while loop to avoid stack overflows               
   auto gap = pattern.GetCount();
   while (gap)
      gap = result.Collect();
   return result.Reduce();
}

/// Associate two patterns                                                    
///   @param invert - whether to associate or disassociate                    
///   @param leftpack - the left pattern                                      
///   @param rightpack - the right pattern                                    
void Ontology::Associate(const bool invert, const Many& leftpack, const Many& rightpack) {
   AssociateIdeas(invert, Represent(leftpack), Represent(rightpack));
}

/// Associate two ideas                                                       
///   @param left - the left idea                                             
///   @param right - the right idea                                           
void Ontology::AssociateIdeas(const bool invert, const Idea::Id& left, const Idea::Id& right) {
   if (left == right) {
      pcLogFuncVerbose << "Skipping " << (invert ? "disassociation - " : "association - ")
         << ccPush << ccWhite << left << ccPop << " to itself";
      return;
   }
   else if (not left or not right)
      throw Except::AI(pcLogFuncError << "Can't associate with bad ideas");

   if (invert) {
      GetCrumb(left)->Disassociate(right);
      GetCrumb(right)->Disassociate(left);
   }
   else {
      GetCrumb(left)->Associate(right);
      GetCrumb(right)->Associate(left);
   }

   pcLogFuncVerbose << (invert ? "Disassociated " : "Associated ")
      << ccPush << ccWhite << left
      << ccPop << " with "
      << ccPush << ccWhite << right;
}

/// Deserialize a crumb, gathering its parents backwards                      
///   @param index - the idea to begin with                                   
///   @param output - [out] the data gets gathered in this container          
void Ontology::Deserialize(const Idea::Id index, Many& output) const {
   ASSEMBLE_VERBOSE(ScopedTab tab; pcLogFuncVerbose
      << "Assembling " << ccCyan << index << tab);

   // Collect the bytes in all crumbs, starting with index itself...    
   auto crumb = GetCrumb(index);
   Bytes serialized;
   serialized.Insert(mRaw.GetBytes() + crumb->mDataStart, crumb->GetLength());

   // ... and then all parents up to the first orphan                   
   if (!crumb->mParents.IsEmpty()) {
      auto parent = GetCrumb(crumb->mParents[0]);
      while (parent) {
         serialized.Insert(mRaw.GetBytes() + parent->mDataStart, parent->GetLength(), uiFront);
         if (parent->mParents.IsEmpty())
            break;

         parent = GetCrumb(parent->mParents[0]);
      }
   }

   // Deserialize it	into output container                              
   output = pcDeserialize(serialized);
   ASSEMBLE_VERBOSE(pcLogVerbose
      << "Assembled " << ccPush << ccCyan << index << ccPop << " to " << output);
}