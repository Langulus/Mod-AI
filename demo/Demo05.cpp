///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 05                                                                 
///                                                                           
/// Userless complex system creation from memory, acting on missing symbols,  
/// interaction caching, resuming.                                            
///                                                                           
void Demo05(Thing& root) {
   auto ms = root.CreateChild("Demo05");
   // "solar" and "system" shouldn't be in the ontology!                
   ms->Say("create a game with the solar system");
   ms->Say("let me control the camera");
   // Asking for missing symbols will continue until you define them,   
   // or until you ignore them. After defining, you should see all      
   // cached interaction up until the point of definition execute.      
   // If you ignore the requests, the cached interaction is simply      
   // discarded from the flow.                                          
}