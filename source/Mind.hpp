///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "inner/Ontology.hpp"
#include <Langulus/Verbs/Do.hpp>
#include <Langulus/Anyness/TMap.hpp>

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
   LANGULUS_VERBS(Verbs::Do);

private:
   // The mind's lifetime counter                                       
   // Will not increment while passed out                               
   Time mLifetime;

   // Mind's private ontology                                           
   // @attention has to be destroyed last                               
   Ontology mOntology;

   // All events the Mind has witnessed, relative to the Mind's time    
   // This can't be Flow::Temporal for various reasons:                 
   // 1. That would imply that a Mind will outright know whether        
   //    something happens periodically or not, which is cheating.      
   // 2. Pushing to a temporal always attempts to execute, and this can 
   //    lead to an infinite regress.                                   
   History mHistory;

   // Societies this mind is part of                                    
   TMany<Society*> mSocieties;

   static void DumpPatterns(const Many&);
   Many Compile(const Many&) const;

public:
   Mind(AI*, const Many&);

   void Do(Verb&);

   Many Interpret(const Text&);
   bool Update(Time);
   void Refresh() {};
   void Teardown();
};
