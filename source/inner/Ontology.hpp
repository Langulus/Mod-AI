#pragma once
#include "Idea.hpp"
#include <Anyness/THive.hpp>
#include <Flow/Verbs/Associate.hpp>


///                                                                           
///   An ontology                                                             
///                                                                           
struct Ontology {
private:
   // The crumbs - each crumb is an idea codepoint                      
   THive<Idea> mCrumbs;

public:
   Ontology() = default;
   Ontology(Describe&&);

   NOD() auto Build(const Bytes&, bool& newlyBuilt) -> const Idea*;
   NOD() auto Seek(const Bytes&) -> const Idea*;
   NOD() auto Represent(const Many&) -> const Idea*;

   void Associate(bool invert, const Many&, const Many&);
   void AssociateIdeas(bool invert, Idea*, Idea*);
   NOD() auto Deserialize(const Idea*) const -> Many;

   auto CreateIdea(Rating, const Bytes&) -> Idea*;
   void DeleteIdea(Idea*);

private:
   Text Self() const;
};
