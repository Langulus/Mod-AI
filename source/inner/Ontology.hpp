///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Idea.hpp"
#include <Langulus/Factory.hpp>
#include <Langulus/Verbs/Associate.hpp>
#include <Langulus/Verbs/Create.hpp>
#include <Langulus/Verbs/Select.hpp>


///                                                                           
///   An ontology                                                             
struct Ontology {
   using CTTI_Ability = Types<Verbs::Create, Verbs::Select>;

private:
   // The owning component                                              
   [[maybe_unused]] Things::AIPart const& mOwner;

   // The ideas                                                         
   TFactoryUnique<Idea> mIdeas;

   // Quick text indexer and auto-completer - iterating all text        
   // combinations in a prompt is costly - use that as an optimization  
   mutable TMapUnsorted<Text, Many> mCache;

   size_t mLongestKnownText = 0;

   //Text Self() const;

   template<class FOR>
   void OptimizeFor(Many&) const;

public:
   Ontology(Things::AIPart const&);
   Ontology(Things::AIPart const&, Neat const&);

   void Create(Verb&);
   void Select(Verb&);

   auto Build(Neat const&, bool findMetapatterns = true) -> Idea*;
   auto BuildText(Text const&) -> Idea*;
   auto Interpret(Text const&) const -> Many;
   //bool FindMetapatterns(Many&) const;
   void Teardown();
};
