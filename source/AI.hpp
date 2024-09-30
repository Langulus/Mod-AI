///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Society.hpp"
#include "Mind.hpp"
#include <Flow/Verbs/Create.hpp>


///                                                                           
///   AI module, producer of minds and societies                              
///                                                                           
struct AI final : A::AIModule {
   LANGULUS(ABSTRACT) false;
   LANGULUS_BASES(A::AIModule);
   LANGULUS_VERBS(Verbs::Create);

private:
   // List of created minds                                             
   TFactory<Mind> mMinds;
   // List of created societies                                         
   TFactory<Society> mSocieties;

public:
   AI(Runtime*, const Many&);
   ~AI();

   bool Update(Time);
   void Create(Verb&);
};