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
#include <Flow/Verbs/Compare.hpp>

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
   LANGULUS_VERBS(Verbs::Compare, Verbs::Associate);

protected:
   friend struct Ontology;

   // Usage and relevance ratings                                       
   Rating mRating = 0;

   // The crumbs that precede this one (hypergraph if more than one)    
   //Ideas mParents;
   // The crumbs that follow this one                                   
   //Ideas mChildren;

   // Associations                                                      
   // Facilitates pattern connections, synonimity and equivalence       
   Ideas mAssociations;
   // Disassociations                                                   
   // Facilitates inhibitory connections and suppresses equivalence     
   Ideas mDisassociations;

public:
   Idea(Ontology*, const Neat&/*, Idea*, Rating, const Bytes&*/);
   Idea(const Idea&) = delete;
   Idea(Idea&&) = delete;

   void Associate(Verb&);
   void Compare(Verb&) const;

   bool operator > (const Idea&) const noexcept;
   bool operator < (const Idea&) const noexcept;

   //NOD() bool IsOrphan() const noexcept;
   //NOD() bool IsStump() const noexcept;
   //NOD() auto GetData() const noexcept -> const Bytes&;
   NOD() auto GetAssociations() const noexcept -> const Ideas&;

   void ResetParents       ();
   void ResetChildren      ();

   //void SetParents         (const Ideas&);
   //void SetChildren        (const Ideas&);
   //bool HasChild           (const Idea*) const;
   //bool HasParent          (const Idea*) const;
   bool HasAssociation     (const Idea*) const;
   bool HasDisassociation  (const Idea*) const;

   //void AddParent          (Idea*);
   //void RemoveParent       (Idea*);
   //void AddChild           (Idea*);
   //void RemoveChild        (Idea*);
   void Associate          (Idea*);
   void Disassociate       (Idea*);

   /*Idea* Build(Ontology&, const Bytes&, Offset progress, Offset end, bool& newlyBuilt);
   Idea* Seek (Ontology&, const Bytes&, Offset progress, Offset end);*/

   explicit operator Text() const;

private:
   Text Self() const;
};
