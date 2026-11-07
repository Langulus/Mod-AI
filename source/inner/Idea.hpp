///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Export.hpp"
#include <Langulus/TSet.hpp>
#include <Langulus/Producible.hpp>
#include <Langulus/Verbs/Do.hpp>
#include <Langulus/Verbs/Associate.hpp>
#include <Langulus/Verbs/Equal.hpp>
#include <Langulus/Verbs/Interpret.hpp>

struct Idea;
struct Ontology;

using Ideas   = TMany<Idea*>;
using IdeaSet = TSet<Idea const*>;
using Rating  = Real;


///                                                                           
///   An idea                                                                 
struct Idea : Referenced, ProducedFrom<Ontology> {
   using CTTI_Producer = Ontology;
   using CTTI_Ability  = Types<
      Verbs::Equal,
      Verbs::Associate,
      Verbs::Interpret
   >;

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
   Idea(Ontology const*, Neat const&);
   Idea(Idea const&) = delete;
   Idea(Idea&&) = delete;

   void Equal(Verb&) const;
   void Associate(Verb&);
   void Interpret(Verb&) const;

   void Teardown();
   auto GetOntology() const -> Ontology*;
   explicit operator Text() const;

   /// Extract a specific type from all idea associations                     
   ///   @tparam T the type to seek for                                       
   ///   @return the hierarchy of the selected type                           
   template<CT::Flat T>
   Many Extract() const {
      IdeaSet mask;
      return ExtractInner(MetaDataOf<T>(), mask);
   }

   bool operator > (Idea const&) const noexcept;
   bool operator < (Idea const&) const noexcept;

   auto GetAssociations()    const noexcept -> Ideas const&;
   auto GetDisassociations() const noexcept -> Ideas const&;

   bool HasAssociation     (Idea const*) const;
   bool HasDisassociation  (Idea const*) const;

   void Associate          (Idea*);
   void Disassociate       (Idea*);

private:
   template<bool ASSOCIATE>
   bool LinkIdea(Idea*);
   template<bool ASSOCIATE>
   void AssociateInner(Verb&);
   auto AdvancedCompare(Idea const*, IdeaSet&) const -> Idea const*;
   Many ExtractInner(DMeta, IdeaSet&) const;
   Many ExtractInnerInner(DMeta, Neat const&) const;
   Text Self() const;
   void Link(Idea*, Ideas&);
};

LANGULUS_MORPHISM(Idea, Annies::Text);