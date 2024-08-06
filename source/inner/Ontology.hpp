#pragma once
#include "Crumb.hpp"
#include <Anyness/THive.hpp>
#include <Flow/Verbs/Associate.hpp>


///                                                                           
///   An ontology                                                             
///                                                                           
struct Ontology {
   LANGULUS_VERBS(Verbs::Associate);

private:
   // The crumbs - each crumb is an idea codepoint                      
   THive<Crumb> mCrumbs;


   Crumb* Build(const Bytes&, Offset progress, Offset end, bool& newlyBuilt);
   Crumb* Seek (const Bytes&, Offset progress, Offset end);

public:
   Ontology(Describe&&);

   void Associate(Verb&);

   NOD() const Crumb* Build(const Bytes&, bool& newlyBuilt);
   NOD() const Crumb* Seek(const Bytes&);
   NOD() const Crumb* Represent(const Many&);

   void Associate(bool invert, const Many&, const Many&);
   void AssociateIdeas(bool invert, const Crumb*, const Crumb*);
   Many Deserialize(const Crumb*) const;

   auto CreateCrumb(Rating, const Bytes&) -> const Crumb*;
   void DeleteCrumb(const Crumb*);
};
