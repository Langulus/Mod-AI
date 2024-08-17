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
///   Pattern                                                                 
///                                                                           
/// Tokenizes, analyzes, collects, resolves ideas, builds metapatterns        
///                                                                           
struct Pattern {
   LANGULUS_CONVERTS_TO(Text);

private:
   // Decoder                                                           
   Ontology& mOntology;
   // Patterns after collecting                                         
   TMany<Pattern*> mSubpatterns;
   // The original pattern                                              
   Many   mData;
   // The depth of the pattern                                          
   Offset mDepth;
   // Whether or not writing in source is allowed                       
   bool   mWritePermitted = false;
   // The serialized pattern segment [mStart; mStart + mCount)          
   Bytes  mDataSerialized;
   // The pattern's resulting idea                                      
   Idea*  mIdea;
   // The resolved idea                                                 
   Many   mResolved;
   // Number of times the pattern has been scanned                      
   Count  mScannedCount = 0;

public:
   Pattern(Ontology&, const Many&, bool writable, Offset depth = 0);
   ~Pattern() = default;

   Pattern& operator = (Moved<Pattern>&&);

   NOD() Offset Interpret(DMeta, Offset limit, Many&);
   NOD() Offset InnerInterpret(DMeta, Many&) const;

   void Compile(Temporal&);
   void InnerCompile(Temporal&) const;

   static bool DumpIdeaFractal(const Many&, Offset target, Offset depth);

   bool IsValid() const noexcept;
   void Resolve(Offset);
   void AssembleData(Offset, Offset, const Many&, Many&, IdeaSet&) const;

   NOD() auto Gather(DMeta, Many&) const -> Offset;
         auto Reduce() -> Idea*;
   NOD() auto FocusOn(const Many&) const -> Pattern;

   auto Collect() -> Count;
   void Build();
   void Push(Pattern&&);
   void ReplaceWith(Pattern&&);

   explicit operator Text() const;

private:
   auto InnerSeek() -> const Idea*;
   auto InnerBuild() -> const Idea*;
   bool InnerGather(
      const Many& input, Offset depth, Offset& limit, DMeta filter, Many& output
   ) const;

   void Serialize();
   Text Self() const;
};