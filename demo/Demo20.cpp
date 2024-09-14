///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 20                                                                 
///                                                                           
/// Execute shadertoy code in renderer. This one with texture and movement    
/// support. Shader based on https://www.shadertoy.com/view/MtGSzD            
///                                                                           
void Demo20(Thing& root) {
   auto ms = root.CreateChild("Demo20");
   /*ms->DoGASM("Create(AWindow, ACamera, AInstance, CController)");
   ms->DoGASM("Create(Entity(ARenderable, AFile(`pebbles.png`), AFile(`canyon.frag`)))");

   // Define inputs																		
   ms->DoGASM("associate(KeyW > [move(Relative(yes), Normal(Forward))])");
   ms->DoGASM("associate(KeyS > [move(Relative(yes), Normal(Backward))])");
   ms->DoGASM("associate(KeyA > [move(Relative(yes), Normal(Left))])");
   ms->DoGASM("associate(KeyD > [move(Relative(yes), Normal(Right))])");
   ms->DoGASM("associate((KeySpace or KeyLShift) > [Move(Relative(yes), Normal(Up))])");
   ms->DoGASM("associate((KeyLControl or KeyC) > [Move(Relative(yes), Normal(Down))])");
   ms->DoGASM("associate(KeyMouseMoveHorizontal > [Move(Relative(yes), Yaw(.MousePosition.Derive.x))])");
   ms->DoGASM("associate(KeyMouseMoveVertical > [Move(Relative(yes), Pitch(.MousePosition.Derive.y))])");
   ms->DoGASM("associate(KeyMouseScrollVertical > [Move(Relative(yes), udLevel(.MouseScroll.Derive.y))])");*/
}