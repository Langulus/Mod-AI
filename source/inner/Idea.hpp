///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Common.hpp"

struct Idea;
struct Ontology;
using Ideas = TMany<Idea*>;
using Rating = Real;


///                                                                           
///   Part of a Pattern                                                       
///                                                                           
struct Idea : Referenced {
protected:
   friend struct Ontology;

   // Usage and relevance ratings                                       
   Rating mRating = 0;
   // The data of the crumb                                             
   Bytes  mData;

   // The crumbs that precede this one (hypergraph if more than one)    
   Ideas mParents;
   // The crumbs that follow this one                                   
   Ideas mChildren;

   // Associations                                                      
   // Facilitates pattern connections, synonimity and equivalence       
   Ideas mAssociations;
   // Disassociations                                                   
   // Facilitates inhibitory connections and suppresses equivalence.    
   Ideas mDisassociations;

public:
   Idea(Idea*, Rating, const Bytes&);
   Idea(const Idea&) = default;
   Idea(Idea&&) noexcept = default;

   bool operator > (const Idea&) const noexcept;
   bool operator < (const Idea&) const noexcept;

   NOD() bool IsOrphan() const noexcept;
   NOD() bool IsStump() const noexcept;
   NOD() auto GetData() const noexcept -> const Bytes&;
   NOD() auto GetAssociations() const noexcept -> const Ideas&;

   void ResetParents       ();
   void ResetChildren      ();

   void SetParents         (const Ideas&);
   void SetChildren        (const Ideas&);
   bool HasChild           (const Idea*) const;
   bool HasParent          (const Idea*) const;
   bool HasAssociation     (const Idea*) const;
   bool HasDisassociation  (const Idea*) const;

   void AddParent          (Idea*);
   void RemoveParent       (Idea*);
   void AddChild           (Idea*);
   void RemoveChild        (Idea*);
   void Associate          (Idea*);
   void Disassociate       (Idea*);

   Idea* Build(Ontology&, const Bytes&, Offset progress, Offset end, bool& newlyBuilt);
   Idea* Seek (Ontology&, const Bytes&, Offset progress, Offset end);
};
