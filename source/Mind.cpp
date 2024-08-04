///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Mind.hpp"
#include "AI.hpp"

#define MIND_VERBOSE(...)     //Logger::Verbose(__VA_ARGS__)
#define BUILD_VERBOSE(...)    //Logger::Verbose(__VA_ARGS__)
#define SEEK_VERBOSE(...)     //Logger::Verbose(__VA_ARGS__)
#define ASSEMBLE_VERBOSE(...)


/// Listener construction                                                     
///   @param producer - the producer                                          
///   @param descriptor - instructions for configuring the listener           
Mind::Mind(AI* producer, const Neat& descriptor)
   : Resolvable   {this}
   , ProducedFrom {producer, descriptor} {
   VERBOSE_AI("Initializing...");
   Couple(descriptor);
   VERBOSE_AI("Initialized");
}

/// A mind records everything that happens around it                          
void Mind::Do(Verb&) {

}

/// React on environmental change                                             
void Mind::Refresh() {

}

/// Execute code inside a mind's context                                      
///   @param code - code to execute in the mind's context                     
///   @return whether or not execution was a success                          
/*bool Mind::Run(const Code& code) {
	auto parsed = code.Parse();
	MIND_VERBOSE(Logger::White, gasm);
	MIND_VERBOSE(parsed);
	Any context { GetBlock() };
	return Verb::ExecuteScope(context, parsed);
}*/

/// Interpret a pattern                                                       
///   @param input - the input data                                           
///   @param output - [out] the interpretation result                         
/*void Mind::DoAIAD(const Text& input, CFlow& output) {
	if (input.IsEmpty())
		return;

	MIND_VERBOSE("Compiling ", input, " to ", output);
	Pattern pattern { const_cast<CMind*>(this), input, false };
	pattern.Compile(std::numeric_limits<pcptr>::max(), output);
}*/

/// Associate/disassociate patterns                                           
///   @param verb - association verb                                          
/*void Mind::Associate(Verb& verb) {
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
}*/

/// Dissect a pattern and integrate it in the available tissue                
///   @param pattern - the pattern to build                                   
///   @param newlyBuilt - [out] for debugging only                            
///   @return the built idea index                                            
Idea::Id Mind::Build(const Bytes& pattern, bool& newlyBuilt) {
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
Idea::Id Mind::Seek(const Bytes& pattern) {
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
Crumb* Mind::CreateCrumb(const real rating, const pcptr pstart, const pcptr pend) {
	// Push a new crumb                                                  
	const auto cid = mCrumbs.GetCount() + 1;
	mCrumbs << Crumb(cid, 0, rating, pstart, pend);
	mHeader = mCrumbs.GetCount();
	return &mCrumbs.Last();
}

/// Delete a crumb                                                            
///   @param index - the crumb index to remove                                
void Mind::DeleteCrumb(const Idea::Id index) {
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
Offset Mind::Cache(const Bytes& bytes) {
	const auto starters = mRaw.GetCount();
	mRaw.Insert(bytes.GetBytes(), bytes.GetCount());
	return starters;
}

/// Represent a pattern as an idea                                            
///   @param pattern - the pattern to represent                               
///   @return the idea representing the pattern                               
Idea::Id Mind::Represent(const Many& pattern) {
	Pattern result { this, pattern, true };

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
void Mind::Associate(const bool invert, const Many& leftpack, const Many& rightpack) {
	AssociateIdeas(invert, Represent(leftpack), Represent(rightpack));
}

/// Associate two ideas                                                       
///   @param left - the left idea                                             
///   @param right - the right idea                                           
void Mind::AssociateIdeas(const bool invert, const Idea::Id& left, const Idea::Id& right) {
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
void Mind::Deserialize(const Idea::Id index, Many& output) const {
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