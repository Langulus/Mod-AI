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

public:
   Ontology();
   Ontology(Describe&&);

   void Create(Verb&);

   /*NOD() auto Build(const Bytes&, bool& newlyBuilt) -> const Idea*;
   NOD() auto Seek(const Bytes&) -> const Idea*;
   NOD() auto Represent(const Many&) -> const Idea*;

   void Associate(bool invert, const Many&, const Many&);
   void AssociateIdeas(bool invert, Idea*, Idea*);
   NOD() auto Deserialize(const Idea*) const -> Many;

   auto CreateIdea(Rating, const Bytes&) -> Idea*;
   void DeleteIdea(Idea*);*/

private:
   Text Self() const;
};
