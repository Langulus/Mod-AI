///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Common.hpp"
#include <Langulus/Anyness/TSet.hpp>
#include <Langulus/Verbs/Do.hpp>
#include <Langulus/Verbs/Associate.hpp>
#include <Langulus/Verbs/Equal.hpp>
#include <Langulus/Verbs/Interpret.hpp>

struct Idea;
struct Ontology;

using Ideas   = TMany<Idea*>;
using IdeaSet = TSet<const Idea*>;
using Rating  = Real;


///                                                                           
///   An idea                                                                 
///                                                                           
struct Idea : Referenced, ProducedFrom<Ontology> {
   LANGULUS(PRODUCER) Ontology;
   LANGULUS_CONVERTS_TO(Text);
   LANGULUS_VERBS(
      Verbs::Equal,
      Verbs::Associate,
      Verbs::Interpret
   );

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

   void Associate(Verb&);
   void Equal(Verb&) const;
   void Interpret(Verb&) const;

   void Teardown();
   auto GetOntology() const->Ontology*;
   explicit operator Text() const;

   /// Extract a specific type from all idea associations                     
   ///   @tparam T - the type to seek for                                     
   ///   @return the hierarchy of the selected type                           
   template<CT::Flat T>
   Many Extract() const {
      IdeaSet mask;
      return ExtractInner(MetaDataOf<T>(), mask);
   }

   bool operator > (const Idea&) const noexcept;
   bool operator < (const Idea&) const noexcept;

   auto GetAssociations()    const noexcept -> const Ideas&;
   auto GetDisassociations() const noexcept -> const Ideas&;

   bool HasAssociation     (const Idea*) const;
   bool HasDisassociation  (const Idea*) const;

   void Associate          (Idea*);
   void Disassociate       (Idea*);

private:
   template<bool ASSOCIATE>
   bool LinkIdea(Idea*);
   template<bool ASSOCIATE>
   void AssociateInner(Verb&);
   auto AdvancedCompare(const Idea*, IdeaSet&) const -> const Idea*;
   Many ExtractInner(DMeta, IdeaSet&) const;
   Many ExtractInnerInner(DMeta, const Many&) const;
   Text Self() const;
   void Link(Idea*, Ideas&);
};