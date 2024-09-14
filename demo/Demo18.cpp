///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 18                                                                 
///                                                                           
/// Execute shadertoy code in renderer with minimal code (no interaction)     
/// Based on https://www.shadertoy.com/view/WdXGRj                            
///                                                                           
void Demo18(Thing& root) {
   auto ms = root.CreateChild("Demo18");
   //todo make with language
   // ms->DoGASM("create(AWindow, Entity(ARenderable, AFile(`clouds.frag`)))");
}