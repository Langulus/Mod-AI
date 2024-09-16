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

   Text Self() const;

public:
   Ontology();
   Ontology(Describe&&);

   void Create(Verb&);
   void Detach();
   auto Build(const Many&) -> Idea*;
};
