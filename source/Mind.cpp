///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Mind.hpp"
#include "AI.hpp"


/// Listener construction                                                     
///   @param producer - the producer                                          
///   @param descriptor - instructions for configuring the mind               
Mind::Mind(AI* producer, const Neat& descriptor)
   : Resolvable   {this}
   , ProducedFrom {producer, descriptor} {
   VERBOSE_AI("Initializing...");
   Couple(descriptor);
   VERBOSE_AI("Initialized");
}

/// A mind records everything that happens around it                          
/// This happens through a dispatching Do verb                                
void Mind::Do(Verb&) {
   TODO();
}

/// A mind can produce ideas                                                  
void Mind::Create(Verb&) {
   TODO();
}

/// React on environmental change                                             
void Mind::Refresh() {

}