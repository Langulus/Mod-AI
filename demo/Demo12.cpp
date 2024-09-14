///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 12                                                                 
///                                                                           
/// GUI testing - interpreting data as GUI components                         
///                                                                           
void Demo12(Thing& root) {
   auto ms = root.CreateChild("Demo12");
   //todo ontology definitions
   // Build GUI on top of Demo08                                        
   Demo08(root);
   ms->Say("also, can you display a list of all the objects");
}