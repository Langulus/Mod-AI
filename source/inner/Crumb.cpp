///                                                                           
/// Langulus::Module::AI                                                      
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Crumb.hpp"
#include "../AI.hpp"
#include <Math/Number.hpp>

#define CRUMB_SEEK_VERBOSE(...)  //Logger::Verbose(__VA_ARGS__)
#define CRUMB_BUILD_VERBOSE(...) //Logger::Verbose(__VA_ARGS__)


/// Constructor                                                               
///   @param parent - the parent of the crumb                                 
///   @param rating - the crumb's rating                                      
///   @param data - the crumb's data                                          
Crumb::Crumb(const Crumb* parent, Rating rating, const Bytes& data)
   : mRating      {rating}
   , mData        {data} {
   if (parent) {
      LANGULUS_ASSUME(DevAssumes, not mParents.Contains(parent),
         "Parent is already linked");
      LANGULUS_ASSUME(DevAssumes, not parent->mChildren.Contains(this),
         "Child is already linked");
      mParents << parent;
      const_cast<Crumb*>(parent)->mChildren << this;
   }
}

/// Compare crumb ratings                                                     
///   @param other - the crumb to compare against                             
///   @return true if left crumb has the higher rating                        
bool Crumb::operator > (const Crumb& other) const noexcept {
   return mRating > other.mRating;
}

/// Compare crumb ratings                                                     
///   @param other - the crumb to compare against                             
///   @return true if right crumb has the higher rating                       
bool Crumb::operator < (const Crumb& other) const noexcept {
   return mRating < other.mRating;
}

/// Check if the crumb has parents                                            
///   @return true if crumb has parents                                       
bool Crumb::IsOrphan() const noexcept {
   return mParents.IsEmpty();
}

/// Check if the crumb has children                                           
///   @return true if crumb has children                                      
bool Crumb::IsStump() const noexcept {
   return mChildren.IsEmpty();
}

/// Get the bytesize of the crumb                                             
///   @return the size of the crumb in bytes                                  
auto Crumb::GetData() const noexcept -> const Bytes& {
   return mData;
}

/// Get the list of associations                                              
///   @return the contained associations                                      
auto Crumb::GetAssociations() const noexcept -> const Crumbs& {
   return mAssociations;
}

/// Check if crumb has a specific child                                       
///   @param crumb - the crumb to search for                                  
///   @return true if this crumb has the child                                
bool Crumb::HasChild(const Crumb* crumb) const {
   return mChildren.Contains(crumb);
}

/// Check if crumb has a specific parent                                      
///   @param crumb - the crumb to search for                                  
///   @return true if this crumb has the parent                               
bool Crumb::HasParent(const Crumb* crumb) const {
   return mParents.Contains(crumb);
}

/// Add a child crumb                                                         
///   @param crumb - the child crumb to add                                   
void Crumb::AddChild(const Crumb* crumb) {
   if (HasChild(crumb))
      return;
   const_cast<Crumb*>(crumb)->mParents << this;
   mChildren << crumb;
}

/// Remove child crumb                                                        
///   @param crumb - the child crumb to remove                                
void Crumb::RemoveChild(const Crumb* crumb) {
   mChildren.Remove(crumb);
   const_cast<Crumb*>(crumb)->mParents.Remove(this);
}

/// Remove parent                                                             
///   @param crumb - the parent crumb to remove                               
void Crumb::RemoveParent(const Crumb* crumb) {
   mParents.Remove(crumb);
   const_cast<Crumb*>(crumb)->mChildren.Remove(this);
}

/// Reset all parents                                                         
void Crumb::ResetParents() {
   for (auto& parent : mParents)
      const_cast<Crumb*>(parent)->mChildren.Remove(this);
   mParents.Clear();
}

/// Reset all children                                                        
void Crumb::ResetChildren() {
   for (auto& child : mChildren)
      const_cast<Crumb*>(child)->mParents.Remove(this);
   mChildren.Clear();
}

/// Set the crumb parent                                                      
///   @param parents - the parents to overwrite with                          
void Crumb::SetParents(const Crumbs& parents) {
   ResetParents();
   for (auto& p : parents)
      AddParent(p);
}

/// Add crumb parent                                                          
///   @param parent - the parent to add                                       
void Crumb::AddParent(const Crumb* parent) {
   if (HasParent(parent))
      return;

   const_cast<Crumb*>(parent)->mChildren << this;
   mParents << parent;
}

/// Set the children of the crumb                                             
///   @param children - the children to overwrite with                        
void Crumb::SetChildren(const Crumbs& children) {
   ResetChildren();
   for (auto& p : children)
      AddChild(p);
}

/// Check if crumb has a given association                                    
///   @param n - the idea to check if inside associations                     
///   @return true if the idea is inside list of associations                 
bool Crumb::HasAssociation(const Crumb* n) const {
   return mAssociations.Contains(n);
}

/// Check if crumb has a given disassociation                                 
///   @param n - the idea to check if inside disassociations                  
///   @return true if the idea is inside list of disassociations              
bool Crumb::HasDisassociation(const Crumb* n) const {
   return mDisassociations.Contains(n);
}

/// Associate this crumb with some data. Symmetic association                 
///   @param n - the idea to insert in associations                           
void Crumb::Associate(const Crumb* n) {
   if (HasAssociation(n))
      return;

   mAssociations << n;
   CRUMB_SEEK_VERBOSE("Decoder: ", Logger::Cyan, this, Logger::Gray,
                      " now synonym to ", Logger::Cyan, n);
}

/// Associate this crumb with some data. Symmetic association                 
///   @param n - the idea to insert in disassociations                        
void Crumb::Disassociate(const Crumb* n) {
   if (HasDisassociation(n))
      return;

   mDisassociations << n;
   CRUMB_SEEK_VERBOSE("Decoder: ", Logger::Cyan << this, Logger::Gray,
                      " now antonym to ", Logger::Cyan, n);
}