///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "inner/Ontology.hpp"
#include <Flow/Verbs/Do.hpp>
#include <Flow/Verbs/Create.hpp>
#include <Anyness/TMap.hpp>

using History = TOrderedMap<Time, Many>;


///                                                                           
///   Mind component                                                          
///                                                                           
/// Gives the capability to associate complex notions and to record events    
/// that are happening to a Thing with this component.                        
///                                                                           
struct Mind final : A::Mind, ProducedFrom<AI> {
   LANGULUS(ABSTRACT) false;
   LANGULUS(PRODUCER) AI;
   LANGULUS_BASES(A::Mind);
   LANGULUS_VERBS(Verbs::Do, Verbs::Create);

private:
   // Societies this mind is part of                                    
   TMany<Society*> mSocieties;

   // All events the Mind has witnessed, relative to the Mind's time    
   // This can't be Flow::Temporal for various reasons:                 
   // 1. That would imply that a Mind will outright know whether        
   //    something happens periodically or not, which is cheating.      
   // 2. Pushing to a temporal always attempts to execute, and this can 
   //    lead to an infinite regress.                                   
   History mHistory;

   // Mind's private ontology                                           
   Ontology mOntology;

   // The mind's lifetime counter                                       
   // Will not increment while passed out                               
   Time mLifetime;

public:
    Mind(AI*, const Neat&);

   void Do(Verb&);
   void Create(Verb&);
   bool Update(Time);
   void Refresh();
   void Detach();
};
