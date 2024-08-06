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

#define PATTERN_INTERPRET_VERBOSE_TAB(...) \
	const auto scope = Logger::VerboseTab(__VA_ARGS__)
#define PATTERN_INTERPRET_VERBOSE(...) \
	Logger::Verbose(__VA_ARGS__)


/// Construct a pattern                                                       
///   @param decoder - the pattern decoder used for decoding                  
///   @param data - the data of the pattern                                   
///   @param writable - whether or not the pattern is writable                
///   @param depth - the depth of the pattern                                 
Pattern::Pattern(Mind* decoder, const Many& data, bool writable, pcptr depth)
   : mDecoder {decoder}
   , mData {data}
   , mDepth {depth}
   , mWritePermitted {writable} {}

constexpr pcptr InfiniteDepth = std::numeric_limits<pcptr>::max();

/// Interpret the idea, gathering data that is relevant to you                
/// Ideas are like fractals - the deeper you go, the more irrelevant data you 
/// get. When interpreting this fractal, you might get entire hierarchies as  
/// a result - it is your responsibility to cull the stuff you don't need     
///   @param verb - the interpretation verb                                   
void Pattern::Interpret(Verb& verb) {
   Many result;
   auto depth = InfiniteDepth;

   const auto inner = [&](DMeta meta) {
      if (meta->Is<GASM>() or meta->Is<Debug>()) {
         // Use reflected operators instead										
         return;
      }

      PATTERN_INTERPRET_VERBOSE_TAB("Interpreting to " << meta);
      Many output;
      auto localDepth = Interpret(meta, depth, output);
      if (!output.IsEmpty()) {
         if (localDepth >= depth) {
            // Attempt to integrate the new interpretation with the old	
            if (IntegrateInterpretation(output, result)) {
               PATTERN_INTERPRET_VERBOSE(ccGreen
                  << "Interprets (after integration) as " << output
                  << " at depth " << depth);
               result = pcMove(output);
            }
            else {
               PATTERN_INTERPRET_VERBOSE(ccRed
                  << "Interprets as " << output
                  << " but was discarded due to depth of " << localDepth);
            }
            return;
         }

         // Interpretations found													
         depth = pcMin(localDepth, depth);
         PATTERN_INTERPRET_VERBOSE(ccGreen
            << "Interprets as " << output
            << " at depth " << depth);
         result = pcMove(output);
      }
      else {
         // Interpretations not found												
         PATTERN_INTERPRET_VERBOSE(ccRed
            << "Nothing found that resembles " << meta);
      }
      };

   // Interpret each type inside verb argument									
   verb.GetArgument().ForEachDeep(
      [&inner](const Block& group) {
         EitherDoThis
            group.ForEach(
               [&inner](const MetaData* t) { inner(t); }
            )
            OrThis
            group.ForEach(
               [&inner](const DataID& t) { inner(t.GetMeta()); }
            );
      }
   );

   // Output results if any															
   if (not result.IsEmpty())
      verb << Move(result);
}

/// Inner, nested pattern compilation														
///	@param filter - the type we're filtering against								
///	@param relevantSolutions - [out] the filtered results							
///	@return the depth at which relevantSolutions were gathered					
pcptr Pattern::InnerInterpret(DMeta filter, Many& relevantSolutions) const {
   // Filter out relevant resolved and pattern data							
   auto depth = Gather(filter, relevantSolutions);
   if (mSubpatterns.IsEmpty())
      return depth;

   if (mSubpatterns.IsOr()) {
      // Combine OR subpatterns in the same OR package						
      auto resolved = Any::FromStateOf(mSubpatterns);
      for (auto subpattern : mSubpatterns) {
         if (subpattern->mResolved.IsEmpty())
            continue;

         // Gather data from the subpattern										
         Any temporary;
         auto localDepth = subpattern->Gather(filter, temporary);
         if (!temporary.IsEmpty()) {
            resolved << pcMove(temporary);
            depth = pcMin(depth, localDepth);
         }
      }

      if (!resolved.IsEmpty())
         relevantSolutions.SmartPush(resolved);
   }
   else for (auto subpattern : mSubpatterns) {
      // AND subpatterns																
      auto localDepth = subpattern->InnerInterpret(filter, relevantSolutions);
      depth = pcMin(depth, localDepth);
   }

   return depth;
}

/// Interpret data. Unlike the compile service, this one does not solve verbs	
/// Only retrieves them along any other data from associations. This call is	
/// used mainly to build patterns															
///	@param filter - the type we're filtering against								
///	@param limit - the biggest allowed depth of the idea							
///	@param relevantSolutions - [out] the filtered results							
///	@return the depth at which relevantSolutions were gathered					
pcptr Pattern::Interpret(DMeta filter, const pcptr limit, Any& relevantSolutions) {
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

/// Snippet for checking if a block has a past point									
bool HasPastPoint(const Block& b) {
   if (b.IsMissing() && b.IsLeft())
      return true;

   bool hasMissingPast = false;
   EitherDoThis
      b.ForEach([&hasMissingPast](const Trait& t) {
      if (HasPastPoint(t))
         hasMissingPast = true;
      return !hasMissingPast;
         })
      OrThis
      b.ForEach([&hasMissingPast](const Construct& t) {
      if (HasPastPoint(t.GetAll()))
         hasMissingPast = true;
      return !hasMissingPast;
         })
      OrThis
      b.ForEach([&hasMissingPast](const Verb& t) {
      if (
         HasPastPoint(t.GetSource()) ||
         HasPastPoint(t.GetArgument()) ||
         HasPastPoint(t.GetOutput()))
         hasMissingPast = true;
      return !hasMissingPast;
         });
   return hasMissingPast;
}

/// Integrate any missing elements in newScope, with stuff provided in old		
///	@param newScope - the scope to integrate											
///	@param oldScope - the data used for integration of newScope					
///	@return true if oldScope was integrated inside newScope						
bool Pattern::IntegrateInterpretation(Any& newScope, const Any& oldScope) {
   // We must guarantee, that a missing past is available					
   bool hasMissingPast = false;
   newScope.ForEachDeep([&hasMissingPast](const Block& b) {
      if (HasPastPoint(b))
         hasMissingPast = true;
      return !hasMissingPast;
      });

   if (!hasMissingPast)
      return false;

   // Then attempt pushing old and new scope. It will success only		
   // if the past point we guaranteed has been satisfied						
   CFlow temporary;
   temporary.Push(oldScope);
   return temporary.Push(newScope);
}


/// Inner, nested pattern compilation														
///	@param solution - the flow that collects data and verbs						
void Pattern::InnerCompile(CFlow& solution) const {
   if (mIdea) {
      // Commit only the main idea to the flow, if available				
      // It can be later interpreted to any specific data if required	
      solution.Push(*this);
      return;
   }

   // Nest-push all subpatterns, if any											
   if (!mSubpatterns.IsEmpty()) {
      if (mSubpatterns.IsOr()) {
         // Coalesce OR subpatterns in the same package						
         auto orpack = Any::FromStateOf(mSubpatterns);
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
///	@param limit - the biggest allowed depth of the idea							
///	@param solutions - [out] the resulting flow										
void Pattern::Compile(const pcptr, CFlow& solution) {
   // Collect all subpatterns inside this pattern								
   // It is done in a while loop to avoid stack overflows					
   auto gap = mData.GetCount();
   while (gap)
      gap = Collect();

   // Nest-push to solution															
   InnerCompile(solution);
}

#define PATTERN_GATHER_VERBOSE_TAB(a) \
	ScopedTab tab; pcLogVerbose << a << tab
#define PATTERN_GATHER_VERBOSE(a) \
	pcLogVerbose << a

/// A snippet for nesting in Pattern::Gather												
///	@param input - the block to nest														
///	@param depth - the depth at which the input appears							
///	@param limit - [in/out] the depth at the first found match					
///	@param filter - what kind of data are we gathering?							
///	@param output - [out] the results go here (type acts as filter)			
///	@return true if something was gathered												
bool InnerGather(const Block& input, pcptr depth, pcptr& limit, DMeta filter, Any& output) {
   if (depth > limit)
      return false;

   SuccessTrap pushed;
   if (input.IsDeep()) {
      // Nest																				
      if (input.IsOr()) {
         // Gather to a temporary OR container									
         auto localOutput = Any::FromStateOf(input);
         input.ForEach(
            [&](const Block& subpack) {
               auto localLimit = InfiniteDepth;
               Any temporary;
               InnerGather(subpack, depth, localLimit, filter, temporary);
               if (!temporary.IsEmpty())
                  localOutput << pcMove(temporary);
            }
         );

         // Even if nothing was pushed, push an empty OR branch			
         // if there's already at least one available branch				
         if (localOutput.GetCount() == 1)
            localOutput << Any();

         if (!localOutput.IsEmpty()) {
            if (depth < limit && !output.IsEmpty()) {
               // Better match was gathered, so reset output				
               PATTERN_GATHER_VERBOSE(ccDarkYellow <<
                  "Better match found, discarding " << output);
               output.Clear();
            }

            PATTERN_GATHER_VERBOSE(
               "Gathered deep OR scope (at depth " << depth << ") " << localOutput);
            output << pcMove(localOutput);
            limit = depth;
            pushed = true;
         }
      }
      else {
         // Gather to the main output												
         input.ForEach(
            [&](const Block& subpack) {
               pushed = InnerGather(subpack, depth + 1, limit, filter, output);
            }
         );
      }

      return pushed;
   }

   // If this is reached, then the provided scope is flat					
   if (input.InterpretsAs(filter)) {
      // The input is compatible with the filter								
      if (depth < limit && !output.IsEmpty()) {
         // Better match was gathered, so reset output						
         PATTERN_GATHER_VERBOSE(ccDarkYellow <<
            "Better match found, discarding " << output);
         output.Clear();
      }

      pushed = output.SmartPush(Any {input});
      PATTERN_GATHER_VERBOSE(
         "Gathered (at depth " << depth << ") " << input);
      limit = depth;
      return pushed;
   }

   // Attempt interpretation before giving up									
   auto interpreter = Verb::From<Verbs::Interpret>({}, filter);
   if (Verb::DispatchFlat(const_cast<Block&>(input), interpreter)) {
      if (depth < limit && !output.IsEmpty()) {
         // Better match was gathered, so reset output						
         PATTERN_GATHER_VERBOSE(ccDarkYellow <<
            "Better match found, discarding " << output);
         output.Clear();
      }

      PATTERN_GATHER_VERBOSE(
         "Gathered after interpretation (at depth " << depth
         << ") from " << input << " to " << interpreter.GetOutput());
      limit = depth;
      pushed = output.SmartPush(interpreter.GetOutput());
   }

   return pushed;
}

/// Specialized gather, that uses the resolved data, if any							
/// Each idea is a fractal, so if output is typed, we extract only as deep		
/// as the filter type is satisfied, skipping all the rest							
///	@param filter - what kind of data are we gathering?							
///	@param output - [out] results go here												
///	@return the depth at which output was gathered									
pcptr Pattern::Gather(DMeta filter, Any& output) const {
   if (!filter || filter->IsDeep()) {
      // No filter, so push everything inside without filtering			
      output << mData << mResolved;
      return 0;
   }

   // First gather inside the pattern itself										
   if (mData.InterpretsAs(filter)) {
      // The data is compatible with the filter									
      Any shallowCopy = mData;
      if (output.SmartPush(shallowCopy)) {
         PATTERN_GATHER_VERBOSE(
            "Gathered (from pattern) " << mData);
         return 0;
      }
   }

   auto limit = InfiniteDepth;
   InnerGather(mResolved, 0, limit, filter, output);
   return limit;
}

/// Get a pattern corresponding to a region of custom data							
///	@param data - the data to generate patterns for									
///	@return the new pattern																	
Pattern Pattern::FocusOn(const Block& data) const {
   return Pattern {mDecoder, data, mWritePermitted, mDepth + 1};
}

/// Merge a subpattern																			
///	@param subpattern - the subpattern to push										
void Pattern::Push(Pattern&& subpattern) {
   auto newPattern = Ptr<Pattern>::New(pcForward<Pattern>(subpattern));
   mSubpatterns << newPattern;
}

/// Replace a subpattern, retaining original data										
///	@param subpattern - the subpattern to use for replacement					
void Pattern::ReplaceWith(Pattern&& subpattern) {
   mSubpatterns = pcMove(subpattern.mSubpatterns);
   mIdea = subpattern.mIdea;
}

/// Convenience function that combines all bases of all elements into one		
/// dense package																					
///	@param base - the base to scan for													
///	@param range - the original data														
///	@return all the bases in the original data, in a dense package				
Any CoalesceBases(const LinkedBase& base, const Any& range) {
   if (base.mBase->IsAbstract() || range.Is<Trait>())
      return {};

   // Bases might not be mapped explicitly to the original type			
   // We need to separate each base and push it individually to			
   // a separate dedicated container												
   auto baseRange = Any::From(base.mBase, range.GetUnconstrainedState());
   baseRange.Allocate(base.mStaticBase.mCount * range.GetCount());
   for (pcptr e = 0; e < range.GetCount(); ++e) {
      auto element = range.GetElement(e);
      auto baseBlock = element.GetBaseMemory(base.mBase, base);
      baseRange.InsertBlock(baseBlock);
   }

   return baseRange;
}

#define PATTERN_COLLECT_VERBOSE_ENABLED
#ifdef PATTERN_COLLECT_VERBOSE_ENABLED
#define PATTERN_COLLECT_VERBOSE(a) a
#else
#define PATTERN_COLLECT_VERBOSE(a)
#endif

/// Collect all subpatterns inside a pattern												
/// Generates the list of subpatterns and their respective ideas, if any		
/// This function never builds any patterns, only relies on prebuilt ones		
/// You have to call it multiple times if there are undefined gaps				
///	@return the number of elements that are undefined								
pcptr Pattern::Collect() {
   // If idea is set, then pattern is completely collected					
   if (mIdea)
      return 0;

   // If subpatterns are available, then pattern has been scanned			
   if (!mSubpatterns.IsEmpty()) {
      pcptr gap = 0;
      for (auto subpattern : mSubpatterns)
         gap = pcMax(subpattern->Collect(), gap);
      return gap;
   }
   else if (mScannedCount) {
      // If pattern has been collected at least once and no idea is		
      // available at this point, split the pattern in two, skip			
      // first part, and focus on the latter										
      if (mData.GetCount() > mScannedCount) {
         auto attempt = FocusOn(
            mData.Crop(mScannedCount, mData.GetCount() - mScannedCount)
         );
         const auto gap = attempt.Collect();
         if (!attempt.mSubpatterns.IsEmpty()) {
            auto gapPattern = mData.Crop(0, mScannedCount);
            PATTERN_COLLECT_VERBOSE(pcLogFuncVerbose
               << ccRed << "Gap formed: " << gapPattern);
            Push(FocusOn(gapPattern));
            Push(pcMove(attempt));
         }
         else ++mScannedCount;
         return gap;
      }
      else return 0;
   }

   // If this is reached, then we're collecting a fresh pattern			
   if (mData.Is<GASM>()) {
      // If contained data is GASM code - parse it								
      // It's the first thing we do, because it might produce stuff		
      auto allParsed = Any::FromStateOf(mData);
      mData.ForEach(
         [&allParsed](const GASM& block) {
            auto parsed = block.Parse();
            if (parsed.IsValid())
               allParsed << pcMove(parsed);
         }
      );
      mData = pcMove(allParsed);
   }

   if (mData.Is<IdeaID>() && mData.GetCount() == 1) {
      // Data is already an idea, no need to search for patterns			
      mIdea = mData.Get<IdeaID>();
      return 0;
   }

   if (mData.IsDeep()) {
      // Nest if deep, in order to serialize hierarchy correctly			
      // There is no escape from this scope										
      pcptr gap {};
      PATTERN_COLLECT_VERBOSE(pcLogFuncVerbose << "Analyzing deep " << *this);
      PATTERN_COLLECT_VERBOSE(ScopedTab tab; pcLog << tab);
      mSubpatterns.ToggleState(mData.GetUnconstrainedState());
      mData.ForEach(
         [&](const Block& subData) {
            auto subpattern = FocusOn(subData);
            gap = pcMax(gap, subpattern.Collect());
            Push(pcMove(subpattern));
         }
      );

      return gap;
   }

   // If this is reached, then pattern is surely flat							
   // Find the biggest subpattern													
   PATTERN_COLLECT_VERBOSE(pcLogFuncVerbose << "Analyzing flat " << *this);
   PATTERN_COLLECT_VERBOSE(ScopedTab tab; pcLog << tab);
   pcptr gap = 0;
   auto biggest = FocusOn({});
   for (pcptr size = 1; size <= mData.GetCount(); ++size) {
      // Seek each subpattern															
      auto subpattern = FocusOn(mData.Crop(0, size));
      const auto found = subpattern.InnerSeek();
      if (found) {
         // Subpattern was found														
         biggest = pcMove(subpattern);
         continue;
      }

      // No match found for that length, but we can dig deeper, by		
      // also comparing bases															
      const auto& range = subpattern.mData;
      for (auto& base : range.GetMeta()->GetBaseList()) {
         const auto coalesced = CoalesceBases(base, range);
         if (coalesced.IsEmpty())
            continue;

         // Nest the collector in a new layer									
         auto basesSubpattern = FocusOn(coalesced);
         auto baseGap = basesSubpattern.Collect();
         if (basesSubpattern.IsValid()) {
            subpattern.ReplaceWith(pcMove(basesSubpattern));
            biggest = pcMove(subpattern);
            gap = pcMax(gap, baseGap);
         }
      }
   }

   const auto biggestSize = biggest.mData.GetCount();
   const auto fullSize = mData.GetCount();
   if (biggestSize) {
      Push(pcMove(biggest));
      if (biggestSize < fullSize) {
         // Append the rest (if any) as a tail									
         auto missing = mData.Crop(biggestSize, fullSize - biggestSize);
         Push(FocusOn(missing));
         PATTERN_COLLECT_VERBOSE(pcLogFuncVerbose
            << ccRed << "Missing: " << missing);
      }
   }
   else {
      PATTERN_COLLECT_VERBOSE(pcLogFuncVerbose
         << ccRed << "Missing: " << mData);
   }

   ++mScannedCount;
   return pcMax(gap, fullSize - biggestSize);
}

/// Get a single idea representation of the pattern. If collection is			
/// complex and writing is allowed - try to represent it as	a single idea		
/// This function might build more patterns												
///	@return the idea																			
IdeaID Pattern::Reduce() {
   if (mIdea)
      return mIdea;

   const bool built = mWritePermitted && !mData.IsDeep();
   if (built)
      Build();

   // Handle some special cases														
   if (mSubpatterns.IsEmpty()) {
      // No nested ideas, just return this idea									
      return mIdea;
   }
   else if (mSubpatterns.GetCount() == 1) {
      // Nested idea is exactly one...												
      if (built) {
         // ... associate newly built idea with the subidea					
         mDecoder->AssociateIdeas(false, mIdea, mSubpatterns[0]->Reduce());
      }
      else {
         // ... not allowed to associate, reduce and return subidea		
         mIdea = mSubpatterns[0]->Reduce();
      }

      return mIdea;
   }

   // Collect the subpattern ideas													
   Ideas ideas;
   if (!mSubpatterns.IsEmpty()) {
      ideas.ToggleState(mSubpatterns.GetUnconstrainedState());
      ideas.Allocate(mSubpatterns.GetCount());
      for (auto subpattern : mSubpatterns)
         ideas << subpattern->Reduce();
   }

   // Serialize and build a metapattern											
   auto metapattern = FocusOn(ideas);
   auto metaGap = ideas.GetCount();
   while (metaGap)
      metaGap = metapattern.Collect();
   metapattern.Reduce();

   if (built)
      // Associate newly built idea with the metapattern						
      mDecoder->AssociateIdeas(false, mIdea, metapattern.mIdea);
   else
      // Not allowed to associate, reduce and return metapattern			
      mIdea = metapattern.mIdea;

   return mIdea;
}

/// Attempts to build this pattern's data and all non-abstract bases of these	
/// elements, as reflected																		
void Pattern::Build() {
   // Build the original data first. This includes empty containers		
   // with state, like '?'																
   InnerBuild();

   // No bases if range is empty or untyped (when ? for example)			
   if (mData.IsUntyped() || mData.IsEmpty())
      return;

   // Then scan all bases																
   for (auto& base : mData.GetMeta()->GetBaseList()) {
      if (base.mBase->IsDeep())
         continue;

      const auto baseRange = CoalesceBases(base, mData);
      if (baseRange.IsEmpty())
         continue;

      // Nest building bases															
      auto subpattern = FocusOn(baseRange);
      subpattern.Collect();
      subpattern.Reduce();
   }
}

/// Serialize the data if not yet serialized												
void Pattern::Serialize() {
   if (!mDataSerialized.IsEmpty())
      return;
   mDataSerialized = pcSerialize<Bytes>(mData);
}

/// Create a pattern and seek it																
///	@return the found idea, or badIdea if not found									
IdeaID Pattern::InnerSeek() {
   Serialize();
   mIdea = mDecoder->Seek(mDataSerialized);
   if (mIdea) {
      // Found the pattern as-is														
      PATTERN_COLLECT_VERBOSE(pcLogFuncVerbose
         << ccGreen << "Found " << *this);
   }
   else {
      // Attempt isolating the pattern, by surrounding it with noise		
      // Noise should be predefined for each type individually				
      //TODO Text noise is hardcoded here for now								
      //Note: works amazingly!														
      if (mData.Is<char8>()) {
         auto isolatedData = mData.Clone();

         if (isolatedData.Get<char8>(0) != ' ')
            isolatedData >> char8(' ');
         if (isolatedData.Get<char8>(isolatedData.GetCount() - 1) != ' ')
            isolatedData << char8(' ');

         auto isolatedSerializedData = pcSerialize<Bytes>(isolatedData);
         mIdea = mDecoder->Seek(isolatedSerializedData);

         if (mIdea) {
            PATTERN_COLLECT_VERBOSE(pcLogFuncVerbose
               << ccGreen << "Found (after isolating "
               << isolatedData << " ) " << *this);
         }
      }
   }

   return mIdea;
}

/// Create a pattern and build it															
///	@return the built idea, or badIdea if unable to build							
IdeaID Pattern::InnerBuild() {
   Serialize();
   bool newlyBuilt = true;
   mIdea = mDecoder->Build(mDataSerialized, newlyBuilt);
#ifdef PATTERN_COLLECT_VERBOSE_ENABLED
   if (mIdea && newlyBuilt)
      pcLogFuncVerbose << ccDarkYellow << "Built " << *this;
#endif
   return mIdea;
}

#define PATTERN_RESOLVE_VERBOSE(a) a

/// Resolve a final collection into a deserialized data sequence					
/// Nested. Nesting depth depends on energy. More energy means more data.		
/// Knowledge is a fractal - the deeper you dig, the more fine detail appears	
/// If you had unlimited energy, you may "pull out" everything, and it would	
/// be likely, that you would extract every data ever collected, that is		
/// even remotely related to the reference point. This, of course, results		
/// primarily in potential confusion. Intelligent systems must ignore data,	
/// to make adequate and efficient conclusions.											
///	@param limit - the biggest allowed depth of the idea							
void Pattern::Resolve(const pcptr limit) {
   if (!mResolved.IsEmpty())
      return;

   // Always push the original pattern first										
   PATTERN_RESOLVE_VERBOSE(pcLogSelfVerbose
      << "Resolve - adding original data: " << mData);
   mResolved << mData;

   if (!mSubpatterns.IsEmpty()) {
      // Resolve all subpatterns														
      for (auto subpattern : mSubpatterns)
         subpattern->Resolve(limit);
   }
   else if (mIdea) {
      // Assemble the crumb corresponding to the idea							
      // This includes associations and deductions/inductions				
      IdeaMask mask;
      Any assembled;
      AssembleData(0, limit, mIdea, assembled, mask);

      // Flatten the assembled data a bit											
      assembled.Optimize();

      // Original data should be always closer to depth 0 than the		
      // assembled data																	
      mResolved << Any::Wrap(assembled);
   }

   PATTERN_RESOLVE_VERBOSE(
      ScopedTab tab;
   pcLogSelfVerbose << "Resolved to... " << tab;
   bool moar = false;
   pcptr counter = 0;
   do {
      pcLogVerbose << counter << ")  ";
      moar = DumpIdeaFractal(mResolved, counter, 0);
      ++counter;
   } while (moar);
      );
}

#define PATTERN_ASSEMBLE_VERBOSE_TAB(a) \
	//ScopedTab tab; pcLogVerbose << a << tab
#define PATTERN_ASSEMBLE_VERBOSE(a) \
	//pcLogVerbose << a

/// A nested crumb deserializer																
///	@param depth - the current depth of the idea hierarchy						
///	@param limit - the biggest allowed depth of the idea							
///	@param input - input data																
///	@param output - [out] deserialized data goes there								
///	@param mask - [i/o] mask for infinite regress protection						
void Pattern::AssembleData(const pcptr depth, const pcptr limit, const Any& input, Any& output, IdeaMask& mask) const {
   if (depth > limit)
      return;

   // Preserve energy if branching													
   auto local = Any::FromStateOf(input);
   if (input.IsDeep()) {
      // Input has hierarchy and we need to nest it							
      // There is no escape from this scope										
      PATTERN_ASSEMBLE_VERBOSE_TAB("Assembling scope " << input);
      input.ForEach(
         [&](const Block& part) {
            AssembleData(depth, limit, part, local, mask);
         }
      );

      if (!local.IsEmpty())
         output << pcMove(local);
   }
   else if (input.Is<IdeaID>()) {
      // If this is reached, then input is flat									
      // Assemble each idea, or propagate data									
      ScopedTab tab;
      const bool flatten = !input.IsOr() && input.GetCount() > 1;
      PATTERN_ASSEMBLE_VERBOSE("Assembling "
         << (flatten ? "(and flattening) " : "") << "idea(s) " << input);
      if (!flatten) pcLog << tab;

      // Deserialize ideas																
      auto& ideas = pcReinterpret<Ideas>(input);
      for (auto& idea : ideas) {
         if (!idea) {
            if (input.IsOr())
               return;
            continue;
         }

         // Check mask																	
         const auto& found = mask.FindKey(idea);
         if (found != uiNone && depth > mask.GetValue(found)) {
            // Idea is already mentioned somewhere prior in hierarchy	
            if (input.IsOr())
               return;
            continue;
         }

         // Update mask																	
         mask.Add(idea, depth);
         const auto crumb = mDecoder->GetCrumb(idea);

         // Deserialize the idea														
         Any deserialized;
         mDecoder->Deserialize(idea, deserialized);

         // Assemble the main idea													
         Any everything;

         // Assemble the main idea													
         Any assembledIdea;
         AssembleData(depth, limit, deserialized, assembledIdea, mask);
         if (!assembledIdea.IsEmpty()) {
            if (flatten)
               output.InsertBlock(pcMove(assembledIdea));
            else
               everything << pcMove(assembledIdea);
         }

         // Nest-assemble all associations										
         Any associations;
         AssembleData(depth + 1, limit, crumb->GetAssociations(), associations, mask);
         if (!associations.IsEmpty()) {
            if (flatten)
               output.InsertBlock(pcMove(associations));
            else
               everything << pcMove(associations);
         }

         if (!everything.IsEmpty()) {
            if (flatten)
               output.InsertBlock(pcMove(everything));
            else
               local << pcMove(everything);
         }
      }

      if (!local.IsEmpty()) {
         if (flatten)
            output.InsertBlock(pcMove(local));
         else
            output << pcMove(local);
      }
   }
   else if (!input.IsEmpty()) {
      // Directly push data that is already deserialized						
      PATTERN_ASSEMBLE_VERBOSE("Propagating data: " << input);
      output << input;
   }

   PATTERN_ASSEMBLE_VERBOSE("Result: " << ccGreen << output);
}

/// Serialize pattern to GASM, by simply serializing the idea id					
Pattern::operator GASM() const {
   GASM result;
   result += mIdea;
   return result;
}

/// Stringify pattern																			
Pattern::operator Debug() const {
   GASM result;
   result += DataID::Of<Pattern>;
   result += GASM::OpenScope;
   result += mDepth;
   result += GASM::AndSeparator;
   result += mData.GetToken();
   result += GASM::AndSeparator;
   if (mIdea)
      result += mIdea;
   else
      result += "<no idea yet>";
   result += GASM::AndSeparator;
   result += pcSerialize<Debug>(mData);
   result += GASM::CloseScope;
   return {result};
}

/// Dump a pretty resolved data fractal level											
bool Pattern::DumpIdeaFractal(const Block& data, pcptr target, pcptr depth) {
   if (target < depth)
      return true;

   if (data.IsDeep() && !data.IsOr()) {
      SuccessTrap found;
      data.ForEach(
         [&](const Block& subdata) {
            found = Pattern::DumpIdeaFractal(subdata, target, depth + 1);
         }
      );
      return found;
   }
   else if (target == depth) {
      pcLog << ccCyan << data << ccDarkYellow << "; ";
      return !data.IsOr();
   }

   return false;
}