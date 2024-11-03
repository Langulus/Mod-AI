///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 01                                                                 
///                                                                           
/// For testing user/session introduction, property setting, entity creation, 
/// basic geometry, contextual chain, controller setup                        
///                                                                           
void Demo01(Thing& root) {
   auto ms = root.CreateChild("Demo01");
   ms->Say("is Dimo");
   //ms->Say("hi, my name is Dimo");
   //ms->Say("let's make a game called Test Game");
   //ms->Say("create a grid");
   //ms->Say("create a universe");
   //ms->Say("let me control the camera with mouse WSAD, space and left control");
}