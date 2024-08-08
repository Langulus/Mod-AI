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
private:
   // Decoder                                                           
   Ontology& mOntology;
   // Patterns after collecting                                         
   TMany<Pattern*> mSubpatterns;
   // The original pattern                                              
   Many mData;
   // The depth of the pattern                                          
   Offset mDepth;
   // Whether or not writing in source is allowed                       
   bool mWritePermitted = false;
   // The serialized pattern segment [mStart; mStart + mCount)          
   Bytes mDataSerialized;
   // The pattern's resulting idea                                      
   const Idea* mIdea;
   // The resolved idea                                                 
   Many  mResolved;
   // Number of times the pattern has been scanned                      
   Count mScannedCount = 0;

public:
   Pattern(Ontology&, const Many&, bool writable, Offset depth = 0);
   ~Pattern() = default;

   NOD() Offset Interpret(DMeta, Offset limit, Many&);
   NOD() Offset InnerInterpret(DMeta, Many&) const;

   void Compile(Temporal&);
   void InnerCompile(Temporal&) const;

   static bool DumpIdeaFractal(const Many&, Offset target, Offset depth);

   bool IsValid() const noexcept;
   void Resolve(Offset);
   using IdeaMask = TMap<IdeaID, pcptr>;
   void AssembleData(Offset, Offset, const Many&, Many&, IdeaMask&) const;

   NOD() auto Gather(DMeta, Many&) const -> Offset;
   NOD() auto Reduce() -> const Idea*;
   NOD() auto FocusOn(const Many&) const -> Pattern;

   auto Collect() -> Count;
   void Build();
   void Push(Pattern&&);
   void ReplaceWith(Pattern&&);

private:
   auto InnerSeek () -> const Idea*;
   auto InnerBuild() -> const Idea*;
   void Serialize ();
};