///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 04                                                                 
///                                                                           
/// Userless requests, chained massive introductions, verb coupling           
///                                                                           
void Demo04(Thing& root) {
   auto ms = root.CreateChild("Demo04");
   ms->Say("create a game with 50 red cubes and a single blue one");
   ms->Say("let me control the camera");
}