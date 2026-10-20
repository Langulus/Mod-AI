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
#include <Langulus/Verbs/Create.hpp>


///                                                                           
///   AI module, producer of minds and societies                              
///                                                                           
struct AI final : Things::AIModule {
   using CTTI_Abstract = No;
   LANGULUS_BASES(Things::AIModule);
   LANGULUS_VERBS(Verbs::Create);

private:
   // List of created minds                                             
   TFactory<Mind> mMinds;
   // List of created societies                                         
   TFactory<Society> mSocieties;

public:
   AI(Runtime*, Many const&);

   bool Update(Time);
   void Create(Verb&);
   void Teardown();
};