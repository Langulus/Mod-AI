///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Society.hpp"
#include "AI.hpp"


/// Society construction                                                      
///   @param producer the AI module that produced the society                 
///   @param descriptor instructions for configuring the society              
Society::Society(AI const* producer, Neat const& descriptor)
   : Downcastable {this}
   , ProducedFrom {producer, descriptor}
   , mOntology    {*this} {
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
