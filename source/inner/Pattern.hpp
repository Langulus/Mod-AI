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
   Ref<Mind*> mSource;
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
   Idea::Id mIdea;
   // The resolved idea                                                 
   Many mResolved;
   // Number of times the pattern has been scanned                      
   Count mScannedCount = 0;

public:
   Pattern() = delete;
   Pattern(Mind*, const Many&, bool writable, Offset depth = 0);
   ~Pattern() = default;

   NOD() pcptr Interpret(DMeta, pcptr limit, Any&);
   NOD() pcptr InnerInterpret(DMeta, Any&) const;
   NOD() static bool IntegrateInterpretation(Any&, const Any&);

   void Compile(pcptr limit, Temporal&);
   void InnerCompile(Temporal&) const;

   void Interpret(Verb&);

   NOD() explicit operator GASM() const;
   NOD() explicit operator Debug() const;

   static bool DumpIdeaFractal(const Block&, pcptr target, pcptr depth);

   inline bool IsValid() const noexcept {
      return mIdea or not mSubpatterns.IsEmpty();
   }

   void Resolve(pcptr);
   using IdeaMask = TMap<IdeaID, pcptr>;
   void AssembleData(pcptr, pcptr, const Any&, Any&, IdeaMask&) const;
   NOD() pcptr Gather(DMeta, Any&) const;

   Idea::Id Reduce();
   NOD() Pattern FocusOn(const Many&) const;

   auto Collect() -> Count;
   void Build();
   void Push(Pattern&&);
   void ReplaceWith(Pattern&&);

private:
   auto InnerSeek()  -> Idea::Id;
   auto InnerBuild() -> Idea::Id;
   void Serialize();
};