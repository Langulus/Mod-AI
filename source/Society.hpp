///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Mind.hpp"


///                                                                           
///   A society                                                               
///                                                                           
/// Acts as an optimization by storing shared data and manages roles.         
/// Societies emerge when minds have a lot of shared knowledge.               
/// Roles emerge when minds start specializing and dividing labor.            
///                                                                           
struct Society final : A::AIUnit, ProducedFrom<AI> {
   LANGULUS(ABSTRACT) false;
   LANGULUS(PRODUCER) AI;
   LANGULUS_BASES(A::AIUnit);

private:
   // Shared history                                                    
   History mHistory;
   // Shared ontology                                                   
   Ontology mOntology;
   // Participating minds                                               
   TMany<Mind*> mMinds;

public:
   Society(AI*, Describe);
   ~Society();

   void Refresh();
};