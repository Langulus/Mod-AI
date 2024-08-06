///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Idea.hpp"

struct Crumb;
using Crumbs = TMany<const Crumb*>;
using Rating = Real;


///                                                                           
///   Part of a Pattern                                                       
///                                                                           
struct Crumb : Referenced {
protected:
   friend struct Mind;

   // Usage and relevance ratings                                       
   Rating mRating = 0;
   // The data of the crumb                                             
   Bytes  mData;

   // The crumbs that precede this one (hypergraph if more than one)    
   Crumbs mParents;
   // The crumbs that follow this one                                   
   Crumbs mChildren;

   // Associations                                                      
   // Facilitates pattern connections, synonimity and equivalence       
   Crumbs mAssociations;
   // Disassociations                                                   
   // Facilitates inhibitory connections and suppresses equivalence.    
   Crumbs mDisassociations;

public:
   Crumb(const Crumb*, Rating, const Bytes&);
   Crumb(const Crumb&) = default;
   Crumb(Crumb&&) noexcept = default;

   bool operator >  (const Crumb&) const noexcept;
   bool operator <  (const Crumb&) const noexcept;

   NOD() bool IsOrphan() const noexcept;
   NOD() bool IsStump() const noexcept;
   NOD() auto GetData() const noexcept -> const Bytes&;
   NOD() auto GetAssociations() const noexcept -> const Crumbs&;

   void ResetParents       ();
   void ResetChildren      ();

   void SetParents         (const Crumbs&);
   void SetChildren        (const Crumbs&);
   bool HasChild           (const Crumb*) const;
   bool HasParent          (const Crumb*) const;
   bool HasAssociation     (const Crumb*) const;
   bool HasDisassociation  (const Crumb*) const;

   void AddParent          (const Crumb*);
   void RemoveParent       (const Crumb*);
   void AddChild           (const Crumb*);
   void RemoveChild        (const Crumb*);
   void Associate          (const Crumb*);
   void Disassociate       (const Crumb*);

};
