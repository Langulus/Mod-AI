///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 14                                                                 
///                                                                           
/// Even bigger stress and performance test                                   
///                                                                           
void Demo14(Thing& root) {
   auto ms = root.CreateChild("Demo14");
   //todo ontology definitions
   ms->Say("make 2000 objects");
   ms->Say("how much time did it take?");
}