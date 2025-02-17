///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/AI.hpp>
#include <Langulus/Testing.hpp>


SCENARIO("Mind creation", "[ai]") {
   static Allocator::State memoryState;
   
   for (int repeat = 0; repeat != 10; ++repeat) {
      GIVEN(std::string("Init and shutdown cycle #") + std::to_string(repeat)) {
         // Create root entity                                          
         auto root = Thing::Root<false>("AI");

         WHEN("The mind is created via abstraction") {
            auto mind = root.CreateUnit<A::Mind>();

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(mind.GetCount() == 1);
            REQUIRE(mind.CastsTo<A::Mind>(1));
            REQUIRE(mind.IsSparse());

            REQUIRE(root.GetUnits().GetCount() == 1);
         }

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mind is created via token") {
            auto mind = root.CreateUnitToken("Mind");

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(mind.GetCount() == 1);
            REQUIRE(mind.CastsTo<A::Mind>(1));
            REQUIRE(mind.IsSparse());

            REQUIRE(root.GetUnits().GetCount() == 1);
         }
      #endif

         // Check for memory leaks after each cycle                     
         REQUIRE(memoryState.Assert());
      }
   }
}

