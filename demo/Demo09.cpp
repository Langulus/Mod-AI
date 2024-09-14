///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 09                                                                 
///                                                                           
/// Sentence segmentation, context group separation, case sensitivity tests.  
///                                                                           
void Demo09(Thing& root) {
   auto ms = root.CreateChild("Demo09");
   //todo ontology definitions
   ms->Say(
      "i need 40 squids, and 30 whales."
      "Squids move chaotically, while whales move in LARGE groups."
      "squids move by .8 m/s, and Whales by 1.2 m/s."
      "Place them in an underwater scene. Let me see how they evolve."
   );
}