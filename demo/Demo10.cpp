///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 10                                                                 
///                                                                           
/// Memory recall, reverse associations                                       
///                                                                           
void Demo10(Thing& root) {
   auto ms = root.CreateChild("Demo10");
   //todo ontology definitions
   // Planet definition should be defined in the ontology               
   ms->Say("what's a planet?");
}