///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 17                                                                 
///                                                                           
/// Complex script with interactive scene, rendering, geometry content        
/// Tests keyboard/mouse input, AABB culling, multiple octaves, trait changes 
///                                                                           
void Demo17(Thing& root) {
   auto ms = root.CreateChild("Demo17");
   //todo make with language
   /*ms->DoGASM("create(AWindow, ACamera, AInstance, CController)");

   // Associate events with actions													
   ms->DoGASM("associate(KeyW > [move(Relative(yes), Normal(Forward))])");
   ms->DoGASM("associate(KeyS > [move(Relative(yes), Normal(Backward))])");
   ms->DoGASM("associate(KeyA > [move(Relative(yes), Normal(Left))])");
   ms->DoGASM("associate(KeyD > [move(Relative(yes), Normal(Right))])");
   ms->DoGASM("associate((KeySpace or KeyLShift) > [Move(Relative(yes), Normal(Up))])");
   ms->DoGASM("associate((KeyLControl or KeyC) > [Move(Relative(yes), Normal(Down))])");
   ms->DoGASM("associate(KeyMouseMoveHorizontal > [Move(Relative(yes), Yaw(.MousePosition.Derive.X))])");
   ms->DoGASM("associate(KeyMouseMoveVertical > [Move(Relative(yes), Pitch(.MousePosition.Derive.Y))])");
   ms->DoGASM("associate(KeyMouseScrollVertical > [Move(Relative(yes), udLevel(.MouseScroll.Derive.Y))])");

   // Create an entity looking like a grid and continuously snap it		
   ms->DoGASM("scope("
      "create(utcontext: Entity(CFractal, ARenderable, CGrid(Topology(ALine), Point3f, Resolution(Vec4(9, 9, 9, 0))), Scale(Vec4(9, 9, 9, 0))) = utcontext),"
      "snap^1(utcontext: select(utcontext: ACamera), Vec4(1, 1, 1, 0))"
   ")");

   // Create an entity looking like a planet										
   ms->DoGASM("create(Entity(AInstance(Position(0,0,6.5), utLevel(2), Scale(12.742,12.713,12.742)), ARenderable, CSphere(ATriangle, Point, Normal, udSampler, U32, Tesselation(3))))");

   // Create an entity looking like a sun											
   ms->DoGASM("create(Entity(AInstance(Position(0,0,-149.6), utLevel(3), Scale(1.391,1.391,1.391)), ARenderable, ALight, CSphere(ATriangle, Point, Normal, udSampler, U32, Tesselation(3))))");

   // Create some small entities														
   ms->DoGASM("create(Entity(AInstance(Position(3,0,0)), ARenderable, CSphere(ATriangle, Point, Normal, udSampler, U32, Tesselation(3))))");
   ms->DoGASM("create(Entity(AInstance(Position(-3,0,0)), ARenderable, CBox(ATriangle, Point, Normal)))");
   return ms;*/
}