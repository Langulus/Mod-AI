///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 03                                                                 
///                                                                           
/// Via Alexander Ivanov (2017)                                               
///                                                                           
void Demo03(Thing& root) {
   auto ms = root.CreateChild("Demo03");
   ms->Say("create game called color cars");
   ms->Say("the player should be able to move the cars using WASD and randomly each second one of the cars turns into a bomb");
   ms->Say("you get 10 points if hitting a car and you lose if you hit a bomb");
   ms->Say("you win if you have 100 points");
}