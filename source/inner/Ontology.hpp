#pragma once
#include "Idea.hpp"
#include <Langulus/Factory.hpp>
#include <Langulus/Verbs/Associate.hpp>
#include <Langulus/Verbs/Create.hpp>
#include <Langulus/Verbs/Select.hpp>


///                                                                           
///   An ontology                                                             
///                                                                           
struct Ontology {
   using CTTI_Ability = Types<Verbs::Create, Verbs::Select>;

private:
   // The owning component                                              
   const Things::AIUnit& mOwner;

   // The ideas                                                         
   TFactoryUnique<Idea> mIdeas;

   // Quick text indexer and auto-completer - iterating all text        
   // combinations in a prompt is costly - use that as an optimization  
   mutable TMapUnsorted<Text, Many> mCache;

   size_t mLongestKnownText = 0;

   Text Self() const;

   template<class FOR>
   void OptimizeFor(Many&) const;

public:
   Ontology(const Things::AIUnit&);
   Ontology(const Things::AIUnit&, Many const&);

   void Create(Verb&);
   void Select(Verb&);

   auto Build(Many const&, bool findMetapatterns = true) -> Idea*;
   auto BuildText(Text const&) -> Idea*;
   auto Interpret(Text const&) const -> Many;
   //bool FindMetapatterns(Many&) const;
   void Teardown();
};
