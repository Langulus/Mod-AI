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
   Ontology(Describe&&);

   NOD() const Idea* Build(const Bytes&, bool& newlyBuilt);
   NOD() const Idea* Seek(const Bytes&);
   NOD() const Idea* Represent(const Many&);

   void Associate(bool invert, const Many&, const Many&);
   void AssociateIdeas(bool invert, const Idea*, const Idea*);
   NOD() Many Deserialize(const Idea*) const;

   auto CreateCrumb(Rating, const Bytes&) -> Idea*;
   void DeleteCrumb(Idea*);
};
