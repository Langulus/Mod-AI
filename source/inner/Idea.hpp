///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Common.hpp"


///                                                                           
///   Idea                                                                    
///                                                                           
/// A codepoint inside a specific mind                                        
///                                                                           
struct Idea {
   using Id = Offset;

protected:
   // The idea's owner                                                  
   Ref<const Mind> mMind;
   // The codepoint                                                     
   Id mId = 0;

public:
   constexpr Idea() noexcept = default;
   Idea(Id id, const Mind* mind) noexcept
      : mMind {mind}, mId {id} {}
};


///                                                                           
///   Idea                                                                    
///                                                                           
/// Some ideas compete, and this gives weight to them                         
///                                                                           
struct WeightedIdea : Idea {
protected:
   // The idea's weight                                                 
   Real mWeight = 0;

public:
   constexpr WeightedIdea() noexcept = default;
   WeightedIdea(const Idea& idea, Real weight = 1) noexcept
      : Idea {idea}, mWeight {weight} {}
};


/// Useful containers                                                         
constexpr Idea BadIdea = Idea {};
using Ids = TMany<Idea::Id>;
//using Ideas = TMany<Idea>;
using	WeightedIdeas = TMany<WeightedIdea>;