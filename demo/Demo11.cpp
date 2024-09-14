///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 11                                                                 
///                                                                           
/// Advanced memory recall, inline associations, advanced context juggling    
/// Casual speech (mistakes are intentional)                                  
///                                                                           
void Demo11(Thing& root) {
   auto ms = root.CreateChild("Demo11");
   //todo ontology definitions
   // Moon shouldn't be defined in the ontology                         
   ms->Say("do you know what a moon is?");

   // Should respond with "no"                                          
   ms->Say("its kinda like a smaller plant that orbits another planet");
   ms->Say("can you make one?");

   // Should respond with "yes"                                         
   ms->Say("then do it.");

   // Should induce a planet, and create a moon that orbits it          
}