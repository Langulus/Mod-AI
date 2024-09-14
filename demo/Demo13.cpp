///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 13                                                                 
///                                                                           
/// Stress and performance test                                               
///                                                                           
void Demo13(Thing& root) {
   auto ms = root.CreateChild("Demo13");
   //todo ontology definitions
   ms->Say("create 100 expensive objects");
   ms->Say("how much time did it take?");
}