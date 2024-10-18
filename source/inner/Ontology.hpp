#pragma once
#include "Idea.hpp"
#include <Flow/Verbs/Associate.hpp>
#include <Flow/Factory.hpp>


///                                                                           
///   An ontology                                                             
///                                                                           
struct Ontology {
   LANGULUS_VERBS(Verbs::Create);

private:
   // The ideas                                                         
   TFactoryUnique<Idea> mIdeas;

   //TODO a quick text indexer and auto-completer - iterating all text combinations is costly, use that as an optimization

public:
   Ontology();
   Ontology(Describe&&);

   void Create(Verb&);
   auto Build(const Many&, bool findMetapatterns = true) -> Idea*;
   auto BuildText(const Text&) -> Idea*;
   auto Interpret(const Text&) const -> Many;
   bool FindMetapatterns(Many&) const;
   void Teardown();
};
