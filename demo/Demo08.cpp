///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 08                                                                 
///                                                                           
/// Advanced geometry, shading, heightmapping, multilevel physics,            
/// multilevel rendering.                                                     
///                                                                           
void Demo08(Thing& root) {
   auto ms = root.CreateChild("Demo08");
   //todo ontology definitions
   ms->Say("generate a random planet");
   ms->Say("let me move the camera");
   ms->Say("place a big cube next to the camera");
   ms->Say("place another smaller in orbit around the bigger one");
}