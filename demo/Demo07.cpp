///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 07                                                                 
///                                                                           
/// Designing a Game Of Life with words                                       
///                                                                           
void Demo07(Thing& root) {
   auto ms = root.CreateChild("Demo07");
   //todo ontology definitions
   ms->Say("create a new 2d game and call it Conway's Game of Life");
   ms->Say("create a 2d grid with scale of 50 pixels");
   ms->Say("snap any cubes to the grid");
   ms->Say("pan the camera when holding right click and moving the mouse");
   ms->Say("let me create yellow cubes with left click");
   ms->Say("pause the following when a press space:");
   ms->Say("1. if a cube touches less than two or more than three cubes - destroy it");
   ms->Say("2. if an empty cell touches three cubes - fill it");
}