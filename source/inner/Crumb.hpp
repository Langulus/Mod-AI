///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Idea.hpp"


///                                                                           
///   Pattern crumb                                                           
///                                                                           
/// A pattern crumb is a distinguishable part of, or a whole pattern          
///                                                                           
struct Crumb {
protected:
   friend class Mind;

   // ID of the crumb                                                   
   Idea::Id mID = 0;
   // Start of pattern in raw memory                                    
   Offset mDataStart = 0;
   // End of pattern in raw memory                                      
   Offset mDataEnd = 0;
   // Usage and relevance ratings                                       
   Real mRating = 0;

   // The crumbs that precede this one (hypergraph if more than one)    
   Ideas mParents;
   // The crumbs that follow this one                                   
   Ideas mChildren;

   // The biggest pattern, starting from this crumb towards children    
   // This gives an approximation to how much stuff any crumb leads     
   // The shortest path is GetLength() of the crumb                     
   Count mLongestPath = 0;

   // Associations                                                      
   // Facilitates pattern connections, synonimity and equivalence       
   Ideas mAssociations;
   // Disassociations                                                   
   // Facilitates inhibitory connections and suppresses equivalence.    
   Ideas mDisassociations;

public:
   Crumb(const Idea::Id& id, const Idea::Id & parent, Real rating, Offset pstart, Offset pend);
   Crumb(const Crumb&) = default;
   Crumb(Crumb&&) noexcept = default;

   bool operator == (const Crumb& other) const noexcept {
      return mID == other.mID;
   }
   bool operator > (const Crumb& other) const noexcept {
      return mRating > other.mRating;
   }
   bool operator < (const Crumb& other) const noexcept {
      return mRating < other.mRating;
   }

   NOD() bool IsValid() const noexcept {
      return mDataEnd > mDataStart;
   }
   NOD() bool IsOrphan() const noexcept {
      return mParents.IsEmpty() or not mID;
   }
   NOD() bool IsStump() const noexcept {
      return mChildren.IsEmpty();
   }
   NOD() auto GetLength() const noexcept {
      return mDataEnd - mDataStart;
   }
   NOD() const auto& GetAssociations() const noexcept {
      return mAssociations;
   }

   void ResetParents          (Mind*);
   void ResetChildren         (Mind*);
   void RecalculateLongestPath(Mind*);

   void SetParents            (Mind*, const Ideas&);
   void SetChildren           (Mind*, const Ideas&);
   bool HasChild              (const Idea::Id&) const;
   bool HasParent             (const Idea::Id&) const;
   bool HasAssociation        (const Idea::Id&) const;
   bool HasDisassociation     (const Idea::Id&) const;

   Idea::Id Build(Mind*, const Bytes&, Offset progress, Offset end, bool& newlyBuilt);
   Idea::Id Seek (Mind*, const Bytes&, Offset progress, Offset end);

   void AddParent          (Mind*, const Idea::Id&);
   void RemoveParent       (Mind*, const Idea::Id&);
   void AddChild           (Mind*, const Idea::Id&);
   void RemoveChild        (Mind*, const Idea::Id&);
   void Associate          (const Idea::Id&);
   void Disassociate       (const Idea::Id&);

};

using Crumbs = TMany<Crumb>;