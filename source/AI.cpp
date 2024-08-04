///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "AI.hpp"

LANGULUS_DEFINE_MODULE(
   AI, 0, "AI",
   "Artificial intelligence module - "
   "allows for simulating minds and societies", "",
   AI, Society, Mind
)


/// Module construction                                                       
///   @param runtime - the runtime that owns the module                       
///   @param descriptor - instructions for configuring the module             
AI::AI(Runtime* runtime, const Neat&)
   : Resolvable {this}
   , A::Module  {runtime}
   , mMinds     {this}
   , mSocieties {this} {
   VERBOSE_AI("Initializing...");
   VERBOSE_AI("Initialized");
}

///                                                                           
AI::~AI() {}

/// Module update routine                                                     
///   @param deltaTime - time between updates                                 
///   @return true                                                            
bool AI::Update(Time deltaTime) {
   return true;
}

/// Create/Destroy minds and societies                                        
///   @param verb - the creation/destruction verb                             
void AI::Create(Verb& verb) {
   mMinds.Create(verb);
   mSocieties.Create(verb);
}