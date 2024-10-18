///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Mind.hpp"
#include "AI.hpp"


/// Mind construction                                                         
///   @param producer - the producer                                          
///   @param descriptor - instructions for configuring the mind               
Mind::Mind(AI* producer, const Many& descriptor)
   : Resolvable   {this}
   , ProducedFrom {producer, descriptor} {
   VERBOSE_AI("Initializing...");
   Couple(descriptor);
   VERBOSE_AI("Initialized");
}

/// First stage destruction                                                   
void Mind::Teardown() {
   mSocieties.Reset();
   mHistory.Reset();
   mOntology.Teardown();
}

/// A mind records everything that happens around it                          
/// This happens through a dispatching Do verb                                
///   @param verb - the verb to log and dispatch                              
void Mind::Do(Verb& verb) {
   //TODO some verbs require sensing organs to register, filter those here
   // Record history                                                    
   if (not mHistory.ContainsKey(mLifetime))
      mHistory.Insert(mLifetime);
   mHistory[mLifetime] << verb;

   // Dispatch                                                          
   if (verb.template IsVerb<Verbs::Create>())
      Create(verb);
}

/// A mind can produce/reuse/destroy ideas                                    
///   @param verb - creation verb                                             
void Mind::Create(Verb& verb) {
   mOntology.Create(verb);
}

/// Mind update routine                                                       
///   @param deltaTime - time between updates                                 
///   @return false                                                           
bool Mind::Update(Time deltaTime) {
   //TODO don't increment time if passed out
   mLifetime += deltaTime;
   return false;
}

/// React on environmental change                                             
void Mind::Refresh() {

}