///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 02                                                                 
///                                                                           
/// For testing entity creation with properties, massive introductions        
/// and physical coordination, camera constraints, geometry, conditionals.    
///                                                                           
void Demo02(Thing& root) {
   auto ms = root.CreateChild("Demo02");
   ms->Say("hi, my name is Dimo");
   ms->Say("make a new game called Cube Tagger");
   ms->Say("generate a giant room");
   ms->Say("place 40 solid red cubes randomly in the room");
   ms->Say("place a solid blue cube at the center");
   ms->Say("let me control the blue cube, move the camera behind it, and attach it to the camera");
   ms->Say("the red cubes always run from the blue one, if it gets too close to them");
   ms->Say("if I touch the red cubes they disappear");
   ms->Say("if all red cubes disappear write \"You win!\" and stop the game");
}