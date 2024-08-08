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
auto Ontology::Build(const Bytes& pattern, bool& newlyBuilt) -> const Idea* {
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
   const auto addition = CreateIdea(1, pattern);
   VERBOSE_AI_BUILD(Logger::Green, "Built pattern ", pattern);
   newlyBuilt = true;
   return addition;
}

/// Recall cached pattern                                                     
///   @param pattern - the pattern to search for                              
///   @return the found idea, or nullptr if not found                         
auto Ontology::Seek(const Bytes& pattern) -> const Idea* {
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

/// Produce a new idea                                                        
///   @param rating - crumb's initial rating                                  
///   @param pattern - the pattern                                            
///   @return the new crumb                                                   
auto Ontology::CreateIdea(Real rating, const Bytes& pattern) -> Idea* {
   return mCrumbs.New(nullptr, 0, pattern);
}

/// Delete an idea                                                            
///   @param idea - the idea to remove                                        
void Ontology::DeleteIdea(Idea* idea) {
   LANGULUS_ASSUME(DevAssumes, idea, "Idea must be valid");

   // Mark everything under that crumb for deletion                     
   // Crumbs are never deleted at runtime - this is instead batched and 
   // done upon serialization/optimization.                             
   for (auto& n : idea->mChildren)
      DeleteIdea(n);

   idea->mChildren.Reset();
   idea->mParents.Reset();
   idea->mAssociations.Reset();
   idea->mDisassociations.Reset();
   idea->mData.Reset();
}

/// Represent a pattern as an idea                                            
///   @param pattern - the pattern to represent                               
///   @return the idea representing the pattern                               
auto Ontology::Represent(const Many& pattern) -> const Idea* {
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
///   @param left - the left pattern                                          
///   @param right - the right pattern                                        
void Ontology::Associate(
   const bool invert, const Many& left, const Many& right
) {
   AssociateIdeas(invert, Represent(left), Represent(right));
}

/// Associate two ideas                                                       
///   @param invert - whether to associate or disassociate                    
///   @param left - the left idea                                             
///   @param right - the right idea                                           
void Ontology::AssociateIdeas(
   const bool invert, const Idea* lhs, const Idea* rhs
) {
   if (lhs == rhs) {
      Logger::Verbose(
         "Skipping ", (invert ? "disassociation - " : "association - "),
         Logger::PushWhite, lhs, Logger::Pop, " to itself");
      return;
   }
   else LANGULUS_ASSERT(lhs and rhs, AI, "Can't associate with bad ideas");

   if (invert) {
      lhs->Disassociate(rhs);
      rhs->Disassociate(lhs);
   }
   else {
      lhs->Associate(rhs);
      rhs->Associate(lhs);
   }

   Logger::Verbose(
      (invert ? "Disassociated " : "Associated "),
      Logger::PushWhite, lhs, Logger::Pop, " with ", Logger::PushWhite, rhs
   );
}

/// Deserialize a crumb, gathering its parents backwards                      
///   @param index - the idea to begin with                                   
///   @param output - [out] the data gets gathered in this container          
Many Ontology::Deserialize(const Idea* idea) const {
   // Collect the bytes in all crumbs, starting with index itself...    
   VERBOSE_AI_ASSEMBLE_TAB("Assembling ", Logger::Cyan, idea);
   Bytes serialized = idea->mData;

   // ... and then all parents up to the first orphan                   
   if (idea->mParents) {
      auto parent = idea->mParents[0]; //TODO does it allow for hypergraphs then???
      while (parent) {
         serialized >> parent->mData;

         if (not parent->mParents)
            break;
         parent = parent->mParents[0];
      }
   }

   // Deserialize it	into output container                              
   Many output;
   serialized.Deserialize(output);
   VERBOSE_AI_ASSEMBLE("Assembled ",
      Logger::PushCyan, idea, Logger::Pop, " to ", output);
   return Abandon(output);
}