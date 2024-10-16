///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Common.hpp"
#include <Anyness/TSet.hpp>
#include <Flow/Verbs/Associate.hpp>
#include <Flow/Verbs/Equal.hpp>

struct Idea;
struct Ontology;

using Ideas   = TMany<Idea*>;
using IdeaSet = TSet<const Idea*>;
using Rating  = Real;


///                                                                           
///   An idea                                                                 
///                                                                           
struct Idea : Referenced, ProducedFrom<Ontology> {
   LANGULUS_CONVERTS_TO(Text);
   LANGULUS(PRODUCER) Ontology;
   LANGULUS_VERBS(Verbs::Equal, Verbs::Associate);

protected:
   friend struct Ontology;

   // Usage and relevance ratings                                       
   Rating mRating = 0;
   // Associations                                                      
   // Facilitates pattern connections, synonimity and equivalence       
   Ideas mAssociations;
   // Disassociations                                                   
   // Facilitates inhibitory connections and suppresses equivalence     
   Ideas mDisassociations;

public:
   Idea(Ontology*, const Many&);
   Idea(const Idea&) = delete;
   Idea(Idea&&) = delete;

   auto GetOntology() const -> Ontology*;
   void Associate(Verb&);
   void Equal(Verb&) const;
   void Teardown();

   bool operator > (const Idea&) const noexcept;
   bool operator < (const Idea&) const noexcept;

   NOD() auto GetAssociations()    const noexcept -> const Ideas&;
   NOD() auto GetDisassociations() const noexcept -> const Ideas&;

   bool HasAssociation     (const Idea*) const;
   bool HasDisassociation  (const Idea*) const;

   void Associate          (Idea*);
   void Disassociate       (Idea*);

   explicit operator Text() const;

private:
   template<bool ASSOCIATE>
   bool LinkIdea(Idea*);
   template<bool ASSOCIATE>
   void AssociateInner(Verb&);
   auto AdvancedCompare(const Idea*, IdeaSet&) const -> const Idea*;
   Text Self() const;
   void Link(Idea*, Ideas&);
};
