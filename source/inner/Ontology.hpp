#pragma once
#include "Idea.hpp"
#include <Langulus/Verbs/Associate.hpp>
#include <Langulus/Verbs/Create.hpp>
#include <Langulus/Verbs/Select.hpp>
#include <Langulus/Flow/Factory.hpp>


///                                                                           
///   An ontology                                                             
///                                                                           
struct Ontology {
   LANGULUS_VERBS(Verbs::Create, Verbs::Select);

private:
   // The owning component                                              
   const A::AIUnit& mOwner;

   // The ideas                                                         
   TFactoryUnique<Idea> mIdeas;

   // Quick text indexer and auto-completer - iterating all text        
   // combinations in a prompt is costly - use that as an optimization  
   mutable TUnorderedMap<Text, Many> mCache;

   Count mLongestKnownText = 0;

   Text Self() const;

   template<class FOR>
   void OptimizeFor(Many&) const;

public:
   Ontology(const A::AIUnit&);
   Ontology(const A::AIUnit&, const Many&);

   void Create(Verb&);
   void Select(Verb&);

   auto Build(const Many&, bool findMetapatterns = true) -> Idea*;
   auto BuildText(const Text&) -> Idea*;
   auto Interpret(const Text&) const -> Many;
   //bool FindMetapatterns(Many&) const;
   void Teardown();
};
