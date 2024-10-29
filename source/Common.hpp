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

LANGULUS_EXCEPTION(AI);

using namespace Langulus;
using namespace Anyness;
using Temporal = Flow::Temporal;

struct AI;
struct Society;
struct Mind;

#if 1
   #define VERBOSE_AI_ENABLED() 1
   #define VERBOSE_AI(...)               Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_AI_TAB(...)           const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_ENABLED() 0
   #define VERBOSE_AI(...)               LANGULUS(NOOP)
   #define VERBOSE_AI_TAB(...)           LANGULUS(NOOP)
#endif

#if 1 and VERBOSE_AI_ENABLED()
   #define VERBOSE_AI_SEEK(...)          Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_AI_SEEK_TAB(...)      const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_SEEK(...)          LANGULUS(NOOP)
   #define VERBOSE_AI_SEEK_TAB(...)      LANGULUS(NOOP)
#endif

#if 1 and VERBOSE_AI_ENABLED()
   #define VERBOSE_AI_BUILD(...)         Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_AI_BUILD_TAB(...)     const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_BUILD(...)         LANGULUS(NOOP)
   #define VERBOSE_AI_BUILD_TAB(...)     LANGULUS(NOOP)
#endif

#if 1 and VERBOSE_AI_ENABLED()
   #define VERBOSE_AI_RESOLVE(...)       Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_AI_RESOLVE_TAB(...)   const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_RESOLVE(...)       LANGULUS(NOOP)
   #define VERBOSE_AI_RESOLVE_TAB(...)   LANGULUS(NOOP)
#endif

#if 1 and VERBOSE_AI_ENABLED()
   #define VERBOSE_AI_ASSEMBLE(...)      Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_AI_ASSEMBLE_TAB(...)  const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_ASSEMBLE(...)      LANGULUS(NOOP)
   #define VERBOSE_AI_ASSEMBLE_TAB(...)  LANGULUS(NOOP)
#endif

#if 1 and VERBOSE_AI_ENABLED()
   #define VERBOSE_AI_COLLECT(...)       Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_AI_COLLECT_TAB(...)   const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_COLLECT(...)       LANGULUS(NOOP)
   #define VERBOSE_AI_COLLECT_TAB(...)   LANGULUS(NOOP)
#endif

#if 1 and VERBOSE_AI_ENABLED()
   #define VERBOSE_AI_GATHER(...)        Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_AI_GATHER_TAB(...)    const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_GATHER(...)        LANGULUS(NOOP)
   #define VERBOSE_AI_GATHER_TAB(...)    LANGULUS(NOOP)
#endif

#if 1 and VERBOSE_AI_ENABLED()
   #define VERBOSE_AI_INTERPRET(...)     Logger::Verbose(Self(), __VA_ARGS__)
   #define VERBOSE_AI_INTERPRET_TAB(...) const auto tab = Logger::VerboseTab(Self(), __VA_ARGS__)
#else
   #define VERBOSE_AI_INTERPRET(...)     LANGULUS(NOOP)
   #define VERBOSE_AI_INTERPRET_TAB(...) LANGULUS(NOOP)
#endif