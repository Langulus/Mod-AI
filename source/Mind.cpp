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
