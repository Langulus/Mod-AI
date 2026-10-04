///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "inner/Ontology.hpp"
#include <Langulus/TMap.hpp>
#include <Langulus/Verbs/Do.hpp>

using History = TMapSorted<Time, Many>;


///                                                                           
///   Mind component                                                          
///                                                                           
/// Gives the capability to associate complex notions and to record events    
/// that are happening to a Thing with this component.                        
///                                                                           
struct Mind final : Things::Mind, ProducedFrom<AI> {
   using CTTI_Abstract = No;
   using CTTI_Producer = AI;
   using CTTI_Bases    = Things::Mind;
   using CTTI_Ability  = Verbs::Do;

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

   static void DumpPatterns(Many const&);
   Many Compile(Many const&) const;

public:
   Mind(AI*, Many const&);

   void Do(Verb&);

   Many Interpret(Text const&);
   bool Update(Time);
   void Refresh() {};
   void Teardown();
};
