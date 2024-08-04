///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Common.hpp"
#include <Flow/Verbs/Do.hpp>
#include <Flow/Verbs/Create.hpp>


///                                                                           
///   Mind component                                                          
///                                                                           
/// Gives the capability to associate complex notions and to record events    
/// that are happening to a Thing with this component.                        
///                                                                           
struct Mind final : A::Unit, ProducedFrom<AI> {
   LANGULUS(ABSTRACT) false;
   LANGULUS(PRODUCER) AI;
   LANGULUS_BASES(A::Unit);
   LANGULUS_VERBS(Verbs::Do);

private:
   // All events the Mind has witnessed                                 
   Temporal mHistory;
   // Societies this mind is part of                                    
   TMany<Society*> mSocieties;
   // Mind's private ontology                                           
   Ontology mOntology;

public:
   Mind(AI*, const Neat&);

   void Do(Verb&);
   void Create(Verb&);

   void Refresh();
};
