///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 16                                                                 
///                                                                           
/// Testing simple pattern recognition and solving simple equations           
///                                                                           
void Demo16(Thing& root) {
   auto ms = root.CreateChild("Demo16");
   //todo ontology definitions
   ms->Say("x =5+y");
   ms->Say("if y= 2, then how much is x?");
}