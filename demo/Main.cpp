///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"
#include <Flow/Time.hpp>
#include <Langulus/AI.hpp>
#include <thread>
#include <iostream>

using namespace Langulus;

LANGULUS_RTTI_BOUNDARY(RTTI::MainBoundary)


int main(int, char**) {
   Logger::ToHTML logFile {"ai-demo.htm"};
   Logger::AttachDuplicator(&logFile);

   // Create root entity                                                
   auto root = Thing::Root("AI");

   // Create the mind and build a common ontology for all demos         
   // Let's prove how scalable this strategy really is ;)               
   auto mind = root.CreateUnit<A::Mind>();
   BuildOntology(root);

   // Pick a demo                                                       
   Logger::Prompt("Pick a demo from 1 to 24, or 0 for a free-talk session");
   Logger::Prompt("See README.md file for information about the available demos");
   Logger::Prompt("> ");
   int demoId = 0;
   std::cin >> demoId;
   switch (demoId) {
   case 0:
      Logger::Info("Free talk mode initiated (TODO)");
      return 0;
   case 1:   Demo01(root); break;
   case 2:   Demo02(root); break;
   case 3:   Demo03(root); break;
   case 4:   Demo04(root); break;
   case 5:   Demo05(root); break;
   case 6:   Demo06(root); break;
   case 7:   Demo07(root); break;
   case 8:   Demo08(root); break;
   case 9:   Demo09(root); break;
   case 10:  Demo10(root); break;
   case 11:  Demo11(root); break;
   case 12:  Demo12(root); break;
   case 13:  Demo13(root); break;
   case 14:  Demo14(root); break;
   case 15:  Demo15(root); break;
   case 16:  Demo16(root); break;
   case 17:  Demo17(root); break;
   case 18:  Demo18(root); break;
   case 19:  Demo19(root); break;
   case 20:  Demo20(root); break;
   case 21:  Demo21(root); break;
   case 22:  Demo22(root); break;
   case 23:  Demo23(root); break;
   case 24:  Demo24(root); break;
   default:
      Logger::Info("No such demo. Shutting down...");
      return 0;
   }

   // Loop until quit                                                   
   Framerate<60> fps;
   while (root.Update(fps.GetDeltaTime()))
      fps.Tick();

   Logger::DettachDuplicator(&logFile);
   return 0;
}
