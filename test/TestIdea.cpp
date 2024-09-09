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

namespace Catch
{
   template<CT::Stringifiable T>
   struct StringMaker<T> {
      static std::string convert(T const& value) {
         return ::std::string {Token {static_cast<Text>(value)}};
      }
   };

   /// Save catch2 from doing infinite recursions with Block types            
   template<CT::Block T>
   struct is_range<T> {
      static const bool value = false;
   };
}


SCENARIO("Idea creation", "[ai]") {
   static Allocator::State memoryState;
   
   for (int repeat = 0; repeat != 10; ++repeat) {
      GIVEN(std::string("Init and shutdown cycle #") + std::to_string(repeat)) {
         // Create root entity                                          
         auto root = Thing::Root("AI");
         auto mind = root.CreateUnit<A::Mind>();

         WHEN("An Idea was created") {
            auto idea = root.Run("? create Idea(`one`)");

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

SCENARIO("Associating ideas", "[ai]") {
   static Allocator::State memoryState;

   GIVEN("Some explicitly created ideas") {
      auto root = Thing::Root("AI");
      auto mind = root.CreateUnit<A::Mind>();

      auto idea_one    = root.Run("thing? create Idea(`one`)");
      auto idea_1      = root.Run("thing? create Idea(1)");
      auto idea_two    = root.Run("thing? create Idea(`two`)");
      auto idea_2      = root.Run("thing? create Idea(2)");
      auto idea_number = root.Run("thing? create Idea(Number)");

      REQUIRE(1 == idea_one.GetCount());
      REQUIRE(1 == idea_1.GetCount());
      REQUIRE(1 == idea_two.GetCount());
      REQUIRE(1 == idea_2.GetCount());
      REQUIRE(1 == idea_number.GetCount());

      REQUIRE(root.Run("##(`one`)")  == idea_one);
      REQUIRE(root.Run("##`one`")    == idea_one);
      REQUIRE(root.Run("##one")      == idea_one);
      REQUIRE(root.Run("##(1)")      == idea_1);
      REQUIRE(root.Run("##two")      == idea_two);
      REQUIRE(root.Run("##`two`")    == idea_two);
      REQUIRE(root.Run("##(2)")      == idea_2);
      REQUIRE(root.Run("##(number)") == idea_number);

      WHEN("Associating and disassociating ideas, building a graph") {
         root.Run("##one = ##(1)");
         root.Run("##two = ##(2)");
         root.Run("##(1) = ##(number)");
         root.Run("##(2) = ##(number)");
         root.Run("##(2) ~ ##(1)");

         REQUIRE(root.Run("##one == ##one"));
         REQUIRE(root.Run("##one == ##(1)"));
         REQUIRE(root.Run("##one == ##(number)"));
         REQUIRE(root.Run("##two == ##two"));
         REQUIRE(root.Run("##two == ##(2)"));
         REQUIRE(root.Run("##two == ##(number)"));
         REQUIRE(root.Run("##(1) == ##one"));
         REQUIRE(root.Run("##(1) == ##(1)"));
         REQUIRE(root.Run("##(1) == ##(number)"));
         REQUIRE(root.Run("##(2) == ##two"));
         REQUIRE(root.Run("##(2) == ##(2)"));
         REQUIRE(root.Run("##(2) == ##(number)"));
         REQUIRE(root.Run("##(number) == ##(number)"));
         REQUIRE(root.Run("##(number) == ##(1)"));
         REQUIRE(root.Run("##(number) == ##(2)"));
         REQUIRE(root.Run("##(number) == ##one"));
         REQUIRE(root.Run("##(number) == ##two"));

         REQUIRE_FALSE(root.Run("##one == ##(2)"));
         REQUIRE_FALSE(root.Run("##two == ##(1)"));
         REQUIRE_FALSE(root.Run("##one == ##two"));
      }
   }

   GIVEN("Some implicitly created ideas") {
      auto root = Thing::Root("AI");
      auto mind = root.CreateUnit<A::Mind>();

      auto idea_one    = root.Run("##one");
      auto idea_1      = root.Run("##(1)");
      auto idea_two    = root.Run("##two");
      auto idea_2      = root.Run("##(2)");
      auto idea_number = root.Run("##(Number)");

      REQUIRE(1 == idea_one.GetCount());
      REQUIRE(1 == idea_1.GetCount());
      REQUIRE(1 == idea_two.GetCount());
      REQUIRE(1 == idea_2.GetCount());
      REQUIRE(1 == idea_number.GetCount());

      WHEN("Associating ideas") {
         root.Run("##one = ##(1)");
         root.Run("##two = ##(2)");
         root.Run("##(1) = ##(number)");
         root.Run("##(2) = ##(number)");
         root.Run("##(2) ~ ##(1)");

         /*REQUIRE(root.Run("##one == ##one"));*/
         REQUIRE(root.Run("##one == ##(1)"));
         REQUIRE(root.Run("##one == ##(number)"));
         REQUIRE(root.Run("##two == ##two"));
         REQUIRE(root.Run("##two == ##(2)"));
         REQUIRE(root.Run("##two == ##(number)"));
         REQUIRE(root.Run("##(1) == ##one"));
         REQUIRE(root.Run("##(1) == ##(1)"));
         REQUIRE(root.Run("##(1) == ##(number)"));
         REQUIRE(root.Run("##(2) == ##two"));
         REQUIRE(root.Run("##(2) == ##(2)"));
         REQUIRE(root.Run("##(2) == ##(number)"));
         /*REQUIRE(root.Run("##(number) == ##(number)"));
         REQUIRE(root.Run("##(number) == ##(1)"));
         REQUIRE(root.Run("##(number) == ##(2)"));
         REQUIRE(root.Run("##(number) == ##one"));
         REQUIRE(root.Run("##(number) == ##two"));

         REQUIRE_FALSE(root.Run("##one == ##(2)"));
         REQUIRE_FALSE(root.Run("##two == ##(1)"));
         REQUIRE_FALSE(root.Run("##one == ##two"));*/
      }
   }

   // Check for memory leaks after each cycle                           
   REQUIRE(memoryState.Assert());
}

