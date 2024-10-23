///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

LANGULUS_RTTI_BOUNDARY(RTTI::MainBoundary)

int main(int argc, char* argv[]) {
   Logger::ToTXT logFile {"ai-test.txt"};
   Logger::AttachDuplicator(&logFile);

   Catch::Session session;
   auto result = session.run(argc, argv);

   Logger::DettachDuplicator(&logFile);
   return result;
}
