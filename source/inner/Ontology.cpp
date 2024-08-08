#include "Ontology.hpp"
#include "Pattern.hpp"


/// Ontology descriptor constructor                                           
///   @param d - descriptor                                                   
Ontology::Ontology(Describe&&) {
   TODO();
}

/// Dissect a pattern and integrate it in the available tissue                
///   @param pattern - the pattern to build                                   
///   @param newlyBuilt - [out] for debugging only                            
///   @return the built idea                                                  
const Crumb* Ontology::Build(const Bytes& pattern, bool& newlyBuilt) {
   for (auto& crumb : mCrumbs) {
      if (not crumb.IsOrphan())
         continue;

      const auto result = crumb.Build(
         *this, pattern, 0, pattern.GetCount(), newlyBuilt);
      if (result) {
         if (newlyBuilt)
            VERBOSE_AI_BUILD(Logger::Green, "Built pattern ", pattern);
         return result;
      }
   }

   // Pattern wasn't built as a child, so do it as an orphan            
   const auto addition = CreateCrumb(1, pattern);
   VERBOSE_AI_BUILD(Logger::Green, "Built pattern ", pattern);
   newlyBuilt = true;
   return addition;
}

/// Recall cached pattern                                                     
///   @param pattern - the pattern to search for                              
///   @return the found idea, or nullptr if not found                         
const Crumb* Ontology::Seek(const Bytes& pattern) {
   for (auto& crumb : mCrumbs) {
      if (not crumb.IsOrphan())
         continue;

      const auto result = crumb.Seek(*this, pattern, 0, pattern.GetCount());
      if (result) {
         VERBOSE_AI_SEEK(Logger::Green, "Pattern found ", pattern);
         return result;
      }
   }

   VERBOSE_AI_SEEK(Logger::Red, "Seeking pattern failed ", pattern);
   return nullptr;
}

/// Produce a new crumb                                                       
///   @param rating - crumb's initial rating                                  
///   @param pattern - the pattern                                            
///   @return the new crumb                                                   
Crumb* Ontology::CreateCrumb(Real rating, const Bytes& pattern) {
   return mCrumbs.New(nullptr, 0, pattern);
}

/// Delete a crumb                                                            
///   @param crumb - the crumb to remove                                      
void Ontology::DeleteCrumb(Crumb* crumb) {
   LANGULUS_ASSUME(DevAssumes, crumb, "Crumb must be valid");

   // Mark everything under that crumb for deletion                     
   // Crumbs are never deleted at runtime - this is instead batched and 
   // done upon serialization/optimization.                             
   for (auto& n : crumb->mChildren)
      DeleteCrumb(n);

   crumb->mChildren.Reset();
   crumb->mParents.Reset();
   crumb->mAssociations.Reset();
   crumb->mDisassociations.Reset();
   crumb->mData.Reset();
}

/// Represent a pattern as an idea                                            
///   @param pattern - the pattern to represent                               
///   @return the idea representing the pattern                               
const Crumb* Ontology::Represent(const Many& pattern) {
   Pattern result {*this, pattern, true};

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