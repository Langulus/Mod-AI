///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"
#include <Langulus/AI.hpp>
#include <catch2/catch.hpp>


/// See https://github.com/catchorg/Catch2/blob/devel/docs/tostring.md        
CATCH_TRANSLATE_EXCEPTION(::Langulus::Exception const& ex) {
   const Text serialized {ex};
   return ::std::string {Token {serialized}};
}

SCENARIO("Idea creation", "[ai]") {
   static Allocator::State memoryState;
   
   for (int repeat = 0; repeat != 10; ++repeat) {
      GIVEN(std::string("Init and shutdown cycle #") + std::to_string(repeat)) {
         // Create root entity                                          
         auto root = Thing::Root("AI");
         auto mind = root.CreateUnit<A::Mind>();

         WHEN("An Idea was created") {
            auto idea = root.Run("create Idea(`one`)");

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(idea.GetCount() == 1);
            REQUIRE(idea.GetType()->mToken == "Idea*");
            REQUIRE(idea.IsSparse());
         }

         // Check for memory leaks after each cycle                     
         REQUIRE(memoryState.Assert());
      }
   }
}

