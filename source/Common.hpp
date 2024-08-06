///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus.hpp>
#include <Flow/Temporal.hpp>
#include <Flow/Factory.hpp>
#include <Langulus/AI.hpp>

using namespace Langulus;
using namespace Anyness;
using Temporal = Flow::Temporal;

struct AI;
struct Society;
struct Mind;

#if 1
   #define VERBOSE_AI_ENABLED() 1
   #define VERBOSE_AI(...)      Logger::Info(Self(), __VA_ARGS__)
   #define VERBOSE_AI_TAB(...)  const auto tab = Logger::InfoTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_ENABLED() 0
   #define VERBOSE_AI(...)      LANGULUS(NOOP)
   #define VERBOSE_AI_TAB(...)  LANGULUS(NOOP)
#endif