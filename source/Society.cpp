///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Society.hpp"
#include "AI.hpp"


/// Gatherer construction                                                     
///   @param producer - the system producer                                   
///   @param descriptor - instructions for configuring the GUI                
Society::Society(AI* producer, const Many& descriptor)
   : Resolvable   {this}
   , ProducedFrom {producer, descriptor} {
   VERBOSE_AI("Initializing...");
   Couple(descriptor);
   VERBOSE_AI("Initialized");
}

/// Shutdown the module                                                       
Society::~Society() {

}

/// React on environmental change                                             
void Society::Refresh() {

}
