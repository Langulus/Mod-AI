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
   // The owning component                                              
   const A::AIUnit& mOwner;

   // The ideas                                                         
   TFactoryUnique<Idea> mIdeas;

   // Quick text indexer and auto-completer - iterating all text        
   // combinations in a prompt is costly - use that as an optimization  
   mutable TUnorderedMap<Text, Many> mCache;

   Text Self() const;

public:
   Ontology(const A::AIUnit&);
   Ontology(const A::AIUnit&, const Many&);

   void Create(Verb&);
   auto Build(const Many&, bool findMetapatterns = true) -> Idea*;
   auto BuildText(const Text&) -> Idea*;
   auto Interpret(const Text&) const -> Many;
   bool FindMetapatterns(Many&) const;
   void Teardown();
};
