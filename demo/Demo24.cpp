///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 24                                                                 
///                                                                           
/// Another, more complex shader generation (text -> GLSL interpretation)     
///                                                                           
void Demo24(Thing& root) {
   auto ms = root.CreateChild("Demo24");
   //todo do with text
   /*ms->DoGASM("Create(AWindow, CController)");

	// Create renderable material														
	ms->DoGASM(
	"Create(Entity(ARenderable, AMaterial(PerPixel, ["
		"Project(),"
		"Rasterize("
		"   Bilateral(yes),"
		"   ["
		"      Create(CRect as Context),"
		"      Move(Interpolator(Cerp), Time(.Time.Modulate(5))),"
		"      Move@0(Yaw(180), Sizer(275.5, 381), Point(400, 400)),"
		"      Move@1(Roll(3), Sizer(551, 762), Point(.MousePosition, 0)),"
		"      Move@1.5(Roll(3), Sizer(551, 762), Point(.MousePosition, 0)),"
		"      Move@3(Sizer(551, 762), Point(.MousePosition, 0)),"
		"      Move@4(Yaw(180), Sizer(275.5, 381), Point(400, 400)),"
		"   ]"
		"),"
		"Texturize(FrontFace, AFile(`pebbles.png`)),"
		"Texturize(BackFace, AFile(`border2.png`)),"
		"Texturize(["
		"   FBM*4(vec2(.utSampler.x, -(.Time * 8.75 - .utSampler.y ^ 2)), ["
		"      Randomize(AReal)"
		"   ])"
		"]),"
		"Illuminate(Normal(0.5, 0.5, 0.5))"
	"])))");
	return ms;*/
}