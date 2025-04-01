///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "AI.hpp"
#include <Langulus/Entity/Event.hpp>
#include <Langulus/Math/Color.hpp>
#include <Langulus/Math/Range.hpp>
#include <Langulus/Math/Number.hpp>

LANGULUS_DEFINE_MODULE(
   AI, 0, "AI",
   "Artificial intelligence module - "
   "allows for simulating minds and societies", "",
   AI, Society, Mind, Idea
)


/// Module construction                                                       
///   @param runtime - the runtime that owns the module                       
///   @param descriptor - instructions for configuring the module             
AI::AI(Runtime* runtime, const Many&)
   : Resolvable {this}
   , A::Module  {runtime} {
   VERBOSE_AI("Initializing...");
   Langulus::RegisterEvents();
   Langulus::Math::RegisterColors();
   Langulus::Math::RegisterVectors();
   Langulus::Math::RegisterRanges();
   Langulus::Math::RegisterNumbers();
   VERBOSE_AI("Initialized");
}

/// First stage destruction                                                   
void AI::Teardown() {
   mMinds.Teardown();
   mSocieties.Teardown();
}

/// Module update routine                                                     
///   @param deltaTime - time between updates                                 
///   @return false                                                           
bool AI::Update(Time deltaTime) {
   LANGULUS(PROFILE);
   for (auto& minds : mMinds)
      minds.Update(deltaTime);
   return false;
}

/// Create/Destroy minds and societies                                        
///   @param verb - the creation/destruction verb                             
void AI::Create(Verb& verb) {
   mMinds.Create(this, verb);
   mSocieties.Create(this, verb);
}