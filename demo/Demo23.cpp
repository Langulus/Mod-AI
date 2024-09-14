///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 23                                                                 
///                                                                           
/// Shader generation (text -> GLSL interpretation)                           
///                                                                           
void Demo23(Thing& root) {
   auto ms = root.CreateChild("Demo23");
   //todo do with text
   /*ms->DoGASM("Create(AWindow, ACamera, CController, AInstance)");

	// Create renderable material														
	ms->DoGASM(
	"Create(Entity(ARenderable, AMaterial(PerPixel, ["
		"Project(ViewTransform),"
		"Raymarch(CBox),"
		"Illuminate(Normal(0.5, 0.5, 0.5))"
	"])))");

	// Define inputs																		
	ms->DoGASM("associate(KeyW > [Move(Relative(yes), Normal(Forward))])");
	ms->DoGASM("associate(KeyS > [Move(Relative(yes), Normal(Backward))])");
	ms->DoGASM("associate(KeyA > [Move(Relative(yes), Normal(Left))])");
	ms->DoGASM("associate(KeyD > [Move(Relative(yes), Normal(Right))])");
	ms->DoGASM("associate((KeySpace or KeyLShift) > [Move(Relative(yes), Normal(Up))])");
	ms->DoGASM("associate((KeyLControl or KeyC) > [Move(Relative(yes), Normal(Down))])");
	ms->DoGASM("associate(KeyMouseMoveHorizontal > [Move(Relative(yes), Yaw(.MousePosition.Derive.x))])");
	ms->DoGASM("associate(KeyMouseMoveVertical > [Move(Relative(yes), Pitch(.MousePosition.Derive.y))])");
	ms->DoGASM("associate(KeyMouseScrollVertical > [Move(Relative(yes), udLevel(.MouseScroll.Derive.y))])");*/
}