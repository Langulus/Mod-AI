///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Pattern.hpp"
#include "../Mind.hpp"
#include "../AI.hpp"


namespace
{
   constexpr Offset InfiniteDepth = ::std::numeric_limits<Offset>::max();
}

/// Construct a pattern                                                       
///   @param onto - the pattern decoder used for decoding                     
///   @param data - the data of the pattern                                   
///   @param writable - whether or not the pattern is writable                
///   @param depth - the depth of the pattern                                 
Pattern::Pattern(Ontology& onto, const Many& data, bool writable, Offset depth)
   : mOntology       {onto}
   , mData           {data}
   , mDepth          {depth}
   , mWritePermitted {writable} {}

Pattern& Pattern::operator = (Moved<Pattern>&& rhs) {
   mSubpatterns = Move(rhs->mSubpatterns);
   mData = Move(rhs->mData);
   mDepth = rhs->mDepth;
   mWritePermitted = rhs->mWritePermitted;
   mIdea = rhs->mIdea;
   mResolved = Move(rhs->mResolved);
   mScannedCount = rhs->mScannedCount;
   return *this;
}

/// Interpret the idea, gathering data that is relevant to you                
/// Ideas are like fractals - the deeper you go, the more irrelevant data you 
/// get. When interpreting this fractal, you might get entire hierarchies as  
/// a result - it is your responsibility to cull the stuff you don't need     
///   @param filter - the type we're filtering against                        
///   @param relevantSolutions - [out] the filtered results                   
///   @return the depth at which relevantSolutions were gathered              
Offset Pattern::InnerInterpret(DMeta filter, Many& relevantSolutions) const {
   // Filter out relevant resolved and pattern data                     
   auto depth = Gather(filter, relevantSolutions);
   if (not mSubpatterns)
      return depth;

   if (mSubpatterns.IsOr()) {
      // Combine OR subpatterns in the same OR package                  
      auto resolved = Many::FromState(mSubpatterns);
      for (auto subpattern : mSubpatterns) {
         if (not subpattern->mResolved)
            continue;

         // Gather data from the subpattern                             
         Many temporary;
         auto localDepth = subpattern->Gather(filter, temporary);
         if (temporary) {
            resolved << Move(temporary);
            depth = ::std::min(depth, localDepth);
         }
      }

      if (resolved)
         relevantSolutions.SmartPush(IndexBack, resolved);
   }
   else for (auto subpattern : mSubpatterns) {
      // AND subpatterns                                                
      auto localDepth = subpattern->InnerInterpret(
         filter, relevantSolutions);
      depth = ::std::min(depth, localDepth);
   }

   return depth;
}

/// Interpret data. Unlike the compile service, this one does not solve verbs 
/// Only retrieves them along any other data from associations. This call is  
/// used mainly to build patterns                                             
///   @param filter - the type we're filtering against                        
///   @param limit - the biggest allowed depth of the idea                    
///   @param relevantSolutions - [out] the filtered results                   
///   @return the depth at which relevantSolutions were gathered              
Offset Pattern::Interpret(DMeta filter, Offset limit, Many& relevantSolutions) {
   // Collect all subpatterns inside this pattern                       
   // It is done in a while loop to avoid stack overflows               
   auto gap = mData.GetCount();
   while (gap)
      gap = Collect();

   // Resolve                                                           
   Resolve(limit);

   // Gather resolved data that matches the filter                      
   return InnerInterpret(filter, relevantSolutions);
}

/// Inner, nested pattern compilation                                         
///   @param solution - the flow that collects data and verbs                 
void Pattern::InnerCompile(Temporal& solution) const {
   if (mIdea) {
      // Commit only the main idea to the flow, if available            
      // It can be later interpreted to any specific data if required   
      solution.Push(*this);
      return;
   }

   // Nest-push all subpatterns, if any                                 
   if (mSubpatterns) {
      if (mSubpatterns.IsOr()) {
         // Coalesce OR subpatterns in the same package                 
         auto orpack = Many::FromState(mSubpatterns);
         for (auto subpattern : mSubpatterns) {
            if (subpattern->mIdea)
               orpack << subpattern;
            else
               orpack << subpattern->mData;
         }

         solution.Push(orpack);
      }
      else for (auto& subpattern : mSubpatterns) {
         // AND subpatterns are simply nested                           
         subpattern->InnerCompile(solution);
      }
   }
   else {
      // No idea available, nor subpatterns, so just push the           
      // original pattern data                                          
      solution.Push(mData);
   }
}

/// Compile a pattern by analyzing the verb hierarchy                         
///   @param solutions - [out] the resulting flow                             
void Pattern::Compile(Temporal& solution) {
   // Collect all subpatterns inside this pattern                       
   // It is done in a while loop to avoid stack overflows               
   auto gap = mData.GetCount();
   while (gap)
      gap = Collect();

   // Nest-push to solution                                             
   InnerCompile(solution);
}

/// A snippet for nesting in Pattern::Gather                                  
///   @param input - the block to nest                                        
///   @param depth - the depth at which the input appears                     
///   @param limit - [in/out] the depth at the first found match              
///   @param filter - what kind of data are we gathering?                     
///   @param output - [out] the results go here (type acts as filter)         
///   @return true if something was gathered                                  
bool Pattern::InnerGather(
   const Many& input, Offset depth, Offset& limit, DMeta filter, Many& output
) const {
   if (depth > limit)
      return false;

   bool pushed = false;
   if (input.IsDeep()) {
      // Nest                                                           
      if (input.IsOr()) {
         // Gather to a temporary OR container                          
         auto localOutput = Many::FromState(input);
         input.ForEach([&](const Many& subpack) {
            auto localLimit = InfiniteDepth;
            Many temporary;
            InnerGather(subpack, depth, localLimit, filter, temporary);
            if (temporary)
               localOutput << Move(temporary);
         });

         // Even if nothing was pushed, push an empty OR branch         
         // if there's already at least one available branch            
         if (localOutput.GetCount() == 1)
            localOutput << Many {};

         if (localOutput) {
            if (depth < limit and not output.IsEmpty()) {
               // Better match was gathered, so reset output            
               VERBOSE_AI_GATHER(Logger::DarkYellow,
                  "Better match found, discarding ", output);
               output.Clear();
            }

            VERBOSE_AI_GATHER(
               "Gathered deep OR scope (at depth ", depth, ") ",
               localOutput);
            output << Move(localOutput);
            limit = depth;
            pushed = true;
         }
      }
      else {
         // Gather to the main output                                   
         input.ForEach([&](const Many& subpack) {
            pushed |= InnerGather(subpack, depth + 1, limit, filter, output);
         });
      }

      return pushed;
   }

   // If this is reached, then the provided scope is flat               
   if (input.CastsToMeta(filter)) {
      // The input is compatible with the filter                        
      if (depth < limit and not output.IsEmpty()) {
         // Better match was gathered, so reset output                  
         VERBOSE_AI_GATHER(Logger::DarkYellow,
            "Better match found, discarding ", output);
         output.Clear();
      }

      pushed = output.SmartPush(IndexBack, Many {input});
      VERBOSE_AI_GATHER("Gathered (at depth ", depth, ") ", input);
      limit = depth;
      return pushed;
   }

   // Attempt interpretation before giving up                           
   Verbs::Interpret interpreter {filter};
   if (Flow::DispatchFlat(input, interpreter)) {
      if (depth < limit and not output.IsEmpty()) {
         // Better match was gathered, so reset output                  
         VERBOSE_AI_GATHER(Logger::DarkYellow,
            "Better match found, discarding ", output);
         output.Clear();
      }

      VERBOSE_AI_GATHER(
         "Gathered after interpretation (at depth ", depth, 
         ") from ", input, " to ", interpreter.GetOutput()
      );

      limit = depth;
      pushed = output.SmartPush(IndexBack, interpreter.GetOutput());
   }

   return pushed;
}

/// Specialized gather, that uses the resolved data, if any                   
/// Each idea is a fractal, so if output is typed, we extract only as deep    
/// as the filter type is satisfied, skipping all the rest                    
///   @param filter - what kind of data are we gathering?                     
///   @param output - [out] results go here                                   
///   @return the depth at which output was gathered                          
Offset Pattern::Gather(DMeta filter, Many& output) const {
   if (not filter or filter->mIsDeep) {
      // No filter, so push everything inside without filtering         
      output << mData << mResolved;
      return 0;
   }

   // First gather inside the pattern itself                            
   if (mData.CastsToMeta(filter)) {
      // The data is compatible with the filter                         
      if (output.SmartPush(IndexBack, mData)) {
         VERBOSE_AI_GATHER("Gathered (from pattern) ", mData);
         return 0;
      }
   }

   auto limit = InfiniteDepth;
   InnerGather(mResolved, 0, limit, filter, output);
   return limit;
}

/// Get a pattern corresponding to a region of custom data                    
///   @param data - the data to generate patterns for                         
///   @return the new pattern                                                 
Pattern Pattern::FocusOn(const Many& data) const {
   return {mOntology, data, mWritePermitted, mDepth + 1};
}

/// Merge a subpattern                                                        
///   @param subpattern - the subpattern to push                              
void Pattern::Push(Pattern&& subpattern) {
   Ref<Pattern> newPattern;
   newPattern.New(Forward<Pattern>(subpattern));
   mSubpatterns << newPattern;
}

/// Replace a subpattern, retaining original data                             
///   @param subpattern - the subpattern to use for replacement               
void Pattern::ReplaceWith(Pattern&& subpattern) {
   mSubpatterns = Move(subpattern.mSubpatterns);
   mIdea = subpattern.mIdea;
}

/// Convenience function that combines all bases of all elements into one     
/// dense package                                                             
///   @param base - the base to scan for                                      
///   @param range - the original data                                        
///   @return all the bases in the original data, in a dense package          
Many CoalesceBases(const RTTI::Base& base, const Many& range) {
   if (base.mType->mIsAbstract or range.Is<Trait>())
      return {};

   // Bases might not be mapped explicitly to the original type         
   // We need to separate each base and push it individually to         
   // a separate dedicated container                                    
   auto baseRange = Many::FromMeta(base.mType, range.GetUnconstrainedState());
   for (Count e = 0; e < range.GetCount(); ++e) {
      auto element = range.GetElement(e);
      auto baseBlock = element.GetBaseMemory(base.mType, base);
      baseRange.InsertBlock(IndexBack, baseBlock);
   }

   return baseRange;
}

/// Collect all subpatterns inside a pattern                                  
/// Generates the list of subpatterns and their respective ideas, if any      
/// This function never builds any patterns, only relies on prebuilt ones     
/// You have to call it multiple times if there are undefined gaps            
///   @return the number of elements that are undefined                       
Offset Pattern::Collect() {
   // If idea is set, then pattern is completely collected              
   if (mIdea)
      return 0;

   // If subpatterns are available, then pattern has been scanned       
   if (mSubpatterns) {
      Offset gap = 0;
      for (auto subpattern : mSubpatterns)
         gap = ::std::max(subpattern->Collect(), gap);
      return gap;
   }
   else if (mScannedCount) {
      // If pattern has been collected at least once and no idea is     
      // available at this point, split the pattern in two, skip        
      // first part, and focus on the latter                            
      if (mData.GetCount() > mScannedCount) {
         auto attempt = FocusOn(mData.Select(
            mScannedCount, mData.GetCount() - mScannedCount));
         const auto gap = attempt.Collect();

         if (attempt.mSubpatterns) {
            auto gapPattern = mData.Select(0, mScannedCount);
            VERBOSE_AI_COLLECT(Logger::Red, "Gap formed: ", gapPattern);
            Push(FocusOn(gapPattern));
            Push(Move(attempt));
         }
         else ++mScannedCount;
         return gap;
      }
      else return 0;
   }

   // If this is reached, then we're collecting a fresh pattern         
   if (mData.Is<Code>()) {
      // If contained data is GASM code - parse it                      
      // It's the first thing we do, because it might produce stuff     
      auto allParsed = Many::FromState(mData);
      mData.ForEach([&allParsed](const Code& block) {
         auto parsed = block.Parse();
         if (parsed.IsValid())
            allParsed << Move(parsed);
      });
      mData = Move(allParsed);
   }

   if (mData.Is<Idea>() and mData.GetCount() == 1) {
      // Data is already an idea, no need to search for patterns        
      mIdea = mData.Get<Idea*>();
      return 0;
   }

   if (mData.IsDeep()) {
      // Nest if deep, in order to serialize hierarchy correctly        
      // There is no escape from this scope                             
      Offset gap = 0;
      VERBOSE_AI_COLLECT_TAB("Analyzing deep ", *this);
      mSubpatterns.AddState(mData.GetUnconstrainedState());
      mData.ForEach([&](const Many& subData) {
         auto subpattern = FocusOn(subData);
         gap = ::std::max(gap, subpattern.Collect());
         Push(Move(subpattern));
      });
      return gap;
   }

   // If this is reached, then pattern is surely flat                   
   // Find the biggest subpattern                                       
   VERBOSE_AI_COLLECT_TAB("Analyzing flat ", *this);
   Offset gap = 0;
   auto biggest = FocusOn({});
   for (Offset size = 1; size <= mData.GetCount(); ++size) {
      // Seek each subpattern                                           
      auto subpattern = FocusOn(mData.Select(0, size));
      const auto found = subpattern.InnerSeek();
      if (found) {
         // Subpattern was found                                        
         biggest = Move(subpattern);
         continue;
      }

      // No match found for that length, but we can dig deeper, by      
      // also comparing bases                                           
      const auto& range = subpattern.mData;
      for (auto& base : range.GetType()->mBases) {
         const auto coalesced = CoalesceBases(base, range);
         if (coalesced.IsEmpty())
            continue;

         // Nest the collector in a new layer                           
         auto basesSubpattern = FocusOn(coalesced);
         auto baseGap = basesSubpattern.Collect();
         if (basesSubpattern.IsValid()) {
            subpattern.ReplaceWith(Move(basesSubpattern));
            biggest = Move(subpattern);
            gap = ::std::max(gap, baseGap);
         }
      }
   }

   const auto biggestSize = biggest.mData.GetCount();
   const auto fullSize = mData.GetCount();
   if (biggestSize) {
      Push(Move(biggest));

      if (biggestSize < fullSize) {
         // Append the rest (if any) as a tail                          
         auto missing = mData.Select(biggestSize, fullSize - biggestSize);
         Push(FocusOn(missing));
         VERBOSE_AI_COLLECT(Logger::Red, "Missing: ", missing);
      }
   }
   else VERBOSE_AI_COLLECT(Logger::Red, "Missing: ", mData);

   ++mScannedCount;
   return ::std::max(gap, fullSize - biggestSize);
}

/// Get a single idea representation of the pattern. If collection is         
/// complex and writing is allowed - try to represent it as	a single idea     
/// This function might build more patterns                                   
///   @return the idea                                                        
auto Pattern::Reduce() -> const Idea* {
   if (mIdea)
      return mIdea;

   const bool built = mWritePermitted and not mData.IsDeep();
   if (built)
      Build();

   // Handle some special cases                                         
   if (not mSubpatterns) {
      // No nested ideas, just return this idea                         
      return mIdea;
   }
   else if (mSubpatterns.GetCount() == 1) {
      // Nested idea is exactly one...                                  
      if (built) {
         // ... associate newly built idea with the subidea             
         mOntology.AssociateIdeas(false, 
            const_cast<Idea*>(mIdea), 
            const_cast<Idea*>(mSubpatterns[0]->Reduce())
         );
      }
      else {
         // ... not allowed to associate, reduce and return subidea     
         mIdea = mSubpatterns[0]->Reduce();
      }

      return mIdea;
   }

   // Collect the subpattern ideas                                      
   TMany<const Idea*> ideas;
   if (mSubpatterns) {
      ideas.AddState(mSubpatterns.GetUnconstrainedState());
      for (auto subpattern : mSubpatterns)
         ideas << subpattern->Reduce();
   }

   // Serialize and build a metapattern                                 
   auto metapattern = FocusOn(ideas);
   auto metaGap = ideas.GetCount();
   while (metaGap)
      metaGap = metapattern.Collect();
   metapattern.Reduce();

   if (built) {
      // Associate newly built idea with the metapattern                
      mOntology.AssociateIdeas(false,
         const_cast<Idea*>(mIdea),
         const_cast<Idea*>(metapattern.mIdea)
      );
   }
   else {
      // Not allowed to associate, reduce and return metapattern        
      mIdea = metapattern.mIdea;
   }
   return mIdea;
}

/// Attempts to build this pattern's data and all non-abstract bases of these 
/// elements, as reflected                                                    
void Pattern::Build() {
   // Build the original data first. This includes empty containers     
   // with state, like '?'                                              
   InnerBuild();

   // No bases if range is empty or untyped (when ? for example)        
   if (mData.IsUntyped() or not mData)
      return;

   // Then scan all bases                                               
   for (auto& base : mData.GetType()->mBases) {
      if (base.mType->mIsDeep)
         continue;

      const auto baseRange = CoalesceBases(base, mData);
      if (not baseRange)
         continue;

      // Nest building bases                                            
      auto subpattern = FocusOn(baseRange);
      subpattern.Collect();
      subpattern.Reduce();
   }
}

/// Serialize the data if not yet serialized                                  
void Pattern::Serialize() {
   if (mDataSerialized)
      return;
   mData.Serialize(mDataSerialized);
}

/// Create a pattern and seek it                                              
///   @return the found idea, or badIdea if not found                         
auto Pattern::InnerSeek() -> const Idea* {
   Serialize();
   mIdea = mOntology.Seek(mDataSerialized);

   if (mIdea) {
      // Found the pattern as-is                                        
      VERBOSE_AI_COLLECT(Logger::Green, "Found ", *this);
   }
   else {
      // Attempt isolating the pattern, by surrounding it with noise    
      // Noise should be predefined for each type individually          
      //TODO Text noise is hardcoded here for now                       
      //Note: works amazingly!                                          
      if (mData.IsExact<Letter>()) {
         Many isolatedData = Clone(mData);

         if (isolatedData.Get<Letter>(0) != ' ')
            isolatedData >> Letter(' ');
         if (isolatedData.Get<Letter>(isolatedData.GetCount() - 1) != ' ')
            isolatedData << Letter(' ');

         Bytes isolatedSerializedData;
         isolatedData.Serialize(isolatedSerializedData);
         mIdea = mOntology.Seek(isolatedSerializedData);

         if (mIdea) {
            VERBOSE_AI_COLLECT(Logger::Green,
               "Found (after isolating ", isolatedData, " ) ", *this);
         }
      }
   }

   return mIdea;
}

/// Create a pattern and build it                                             
///   @return the built idea, or badIdea if unable to build                   
auto Pattern::InnerBuild() -> const Idea* {
   Serialize();
   bool newlyBuilt = true;
   mIdea = mOntology.Build(mDataSerialized, newlyBuilt);
   if (mIdea and newlyBuilt)
      VERBOSE_AI_BUILD(Logger::DarkYellow, "Built ", *this);
   return mIdea;
}

/// Resolve a final collection into a deserialized data sequence              
/// Nested. Nesting depth depends on energy. More energy means more data.     
/// Knowledge is a fractal - the deeper you dig, the more fine detail appears 
/// If you had unlimited energy, you may "pull out" everything, and it would  
/// be likely, that you would extract every data ever collected, that is      
/// even remotely related to the reference point. This, of course, results    
/// primarily in potential confusion. Intelligent systems must ignore data,   
/// to make adequate and efficient conclusions.                               
///   @param limit - the biggest allowed depth of the idea                    
void Pattern::Resolve(const Offset limit) {
   if (mResolved)
      return;

   // Always push the original pattern first                            
   VERBOSE_AI_RESOLVE("Resolve - adding original data: ", mData);
   mResolved << mData;

   if (mSubpatterns) {
      // Resolve all subpatterns                                        
      for (auto subpattern : mSubpatterns)
         subpattern->Resolve(limit);
   }
   else if (mIdea) {
      // Assemble the crumb corresponding to the idea                   
      // This includes associations and deductions/inductions           
      IdeaSet mask;
      Many assembled;
      AssembleData(0, limit, mIdea, assembled, mask);

      // Flatten the assembled data a bit                               
      assembled.Optimize();

      // Original data should be always closer to depth 0 than the      
      // assembled data                                                 
      mResolved << Many::Wrap(assembled);
   }

   VERBOSE_AI_RESOLVE_TAB("Resolved to... ");
   bool moar = false;
   Offset counter = 0;
   do {
      moar = DumpIdeaFractal(mResolved, counter, 0);
      ++counter;
   }
   while (moar);
}

/// A nested idea deserializer                                                
///   @param depth - the current depth of the idea hierarchy                  
///   @param limit - the biggest allowed depth of the idea                    
///   @param input - input data                                               
///   @param output - [out] deserialized data goes there                      
///   @param mask - [i/o] mask for infinite regress protection                
void Pattern::AssembleData(
   const Offset depth, const Offset limit,
   const Many& input, Many& output, IdeaSet& mask
) const {
   if (depth > limit)
      return;

   // Preserve energy if branching                                      
   auto local = Many::FromState(input);
   if (input.IsDeep()) {
      // Input has hierarchy and we need to nest it                     
      // There is no escape from this scope                             
      VERBOSE_AI_ASSEMBLE_TAB("Assembling scope ", input);
      input.ForEach([&](const Many& part) {
         AssembleData(depth, limit, part, local, mask);
      });

      if (local)
         output << Move(local);
   }
   else if (input.IsSimilar<Idea*>()) {
      // If this is reached, then input is flat                         
      // Assemble each idea, or propagate data                          
      const bool flatten = not input.IsOr() and input.GetCount() > 1;
      VERBOSE_AI_ASSEMBLE("Assembling ",
         (flatten ? "(and flattening) " : ""), "idea(s) ", input);

      // Deserialize ideas                                              
      auto& ideas = ReinterpretCast<Ideas>(input);
      for (auto& idea : ideas) {
         if (not idea) {
            if (input.IsOr())
               return;
            continue;
         }

         // Check mask                                                  
         if (mask.Contains(idea)) {
            // Idea was already mentioned somewhere prior in hierarchy  
            if (input.IsOr())
               return;
            continue;
         }
         else mask << idea;

         // Deserialize and assemble the main idea                      
         auto deserialized = mOntology.Deserialize(idea);
         Many everything;
         Many assembledIdea;
         AssembleData(depth, limit, deserialized, assembledIdea, mask);

         if (assembledIdea) {
            if (flatten)
               output.InsertBlock(IndexBack, Move(assembledIdea));
            else
               everything << Move(assembledIdea);
         }

         // Nest-assemble all associations                              
         Many associations;
         AssembleData(depth + 1, limit, idea->GetAssociations(), associations, mask);

         if (associations) {
            if (flatten)
               output.InsertBlock(IndexBack, Move(associations));
            else
               everything << Move(associations);
         }

         if (everything) {
            if (flatten)
               output.InsertBlock(IndexBack, Move(everything));
            else
               local << Move(everything);
         }
      }

      if (local) {
         if (flatten)
            output.InsertBlock(IndexBack, Move(local));
         else
            output << Move(local);
      }
   }
   else if (input) {
      // Directly push data that is already deserialized                
      VERBOSE_AI_ASSEMBLE("Propagating data: ", input);
      output << input;
   }

   VERBOSE_AI_ASSEMBLE("Result: ", Logger::Green, output);
}

/// Dump a pretty resolved data fractal level                                 
bool Pattern::DumpIdeaFractal(const Many& data, Offset target, Offset depth) {
   if (target < depth)
      return true;

   if (data.IsDeep() and not data.IsOr()) {
      bool found = false;
      data.ForEach([&](const Many& subdata) {
         found |= Pattern::DumpIdeaFractal(subdata, target, depth + 1);
      });
      return found;
   }
   else if (target == depth)
      return not data.IsOr();
   return false;
}

/// Check if the pattern is valid                                             
///   @return true if pattern has been resolved to a single idea, or contains 
///           subpatterns                                                     
bool Pattern::IsValid() const noexcept {
   return mIdea or mSubpatterns;
}

/// Used for logging                                                          
///   @return text representing this pattern                                  
Text Pattern::Self() const {
   TODO();
   return {};
}
