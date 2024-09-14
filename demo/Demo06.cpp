///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2024 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Main.hpp"


///                                                                           
///   Demo 06                                                                 
///                                                                           
/// Complex system association, number parsing, arithmetic operators, casual  
/// speech, advanced context juggle.                                          
///                                                                           
void Demo06(Thing& root) {
   auto ms = root.CreateChild("Demo06");
   //todo ontology definitions
   ms->Say("the solar system is made of 9 primary objects");
   ms->Say("the first one is the sun, of course");
   ms->Say("its radius is 696,000 kilometers and its mass is 1.989 x 10^30 kilograms");
   ms->Say("the second body is planet Mercury, with radius of 4,878 km, and mass of 330 x 10^23 kg");
   ms->Say("its perihelion is at 46,001,009 km, and its aphelion is at 69,817,445");
   ms->Say("third body is planet Venus, with radius of 6,052 km, and mass of 4.86732 x 10^24 kg");
   ms->Say("its perihelion is at 107.48x10^6 km, and its aphelion is at 108.94x10^6");
   ms->Say("fourth body is planet Earth, with equatorial radius of 6378.1 km, and polar radius of 6356.8 km");
   ms->Say("its mass is 5.9726 x 10^24 kg");
   ms->Say("its perihelion is at 147.09x10^6 km, and its aphelion is at 152.10x10^6");
   ms->Say("fifth body is planet Mars, with equatorial radius of 3396.2 km, and polar radius of 3376.2 km");
   ms->Say("its mass is 0.64174 x 10^24 kg");
   ms->Say("its perihelion is at 206.62x10^6 km, and its aphelion is at 249.23x10^6");
   ms->Say("sixth body is planet Jupiter, with equatorial radius of 71,492 km, and polar radius of 66,854 km");
   ms->Say("its mass is 1,898.3 x 10^24 kg");
   ms->Say("its perihelion is at 740.52x10^6 km, and its aphelion is at 816.62x10^6");
   ms->Say("next is planet Saturn, with equatorial radius of 60,268 km, and polar radius of 54,364 km");
   ms->Say("its mass is 568.36 x 10^24 kg");
   ms->Say("its perihelion is at 1,352.55x10^6 km, and its aphelion is at 1,514.50x10^6");
   ms->Say("next is planet Uranus, with equatorial radius of 25,559 km, and polar radius of 24,973 km");
   ms->Say("its mass is 86.816 x 10^24 kg");
   ms->Say("its perihelion is at 2,741.30x10^6 km, and its aphelion is at 3,003.62x10^6");
   ms->Say("last is planet Neptune, with equatorial radius of 24,764 km, and polar radius of 24,341 km");
   ms->Say("its mass is 102.42 x 10^24 kg");
   ms->Say("its perihelion is at 4,444.45x10^6 km, and its aphelion is at 4,545.67x10^6");
}