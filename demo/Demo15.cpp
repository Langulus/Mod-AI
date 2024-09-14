///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 15                                                                 
///                                                                           
/// Building, GUI                                                             
///                                                                           
void Demo15(Thing& root) {
   auto ms = root.CreateChild("Demo15");
   //todo ontology definitions
   ms->Say("make a game");
   ms->Say("let me control the camera");
   ms->Say("add gui");
   ms->Say("let me build");
}