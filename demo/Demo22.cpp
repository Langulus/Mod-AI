///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 22                                                                 
///                                                                           
/// Multiple projects loaded at the same time - capsulation tests             
///                                                                           
void Demo22(Thing& root) {
   auto ms = root.CreateChild("Demo22");
   Demo17(*ms);
   Demo20(*ms);
}