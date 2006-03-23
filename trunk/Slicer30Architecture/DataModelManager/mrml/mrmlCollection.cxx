/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: Collection.cxx,v $
Date:      $Date: 2006/03/03 22:26:40 $
Version:   $Revision: 1.11 $

=========================================================================auto=*/
#include "mrmlCollection.h"
#include <iterator>

namespace mrml
{

class CollectionInternals
{
public:
  CollectionInternals() {};
  ~CollectionInternals() {};

  typedef std::list<Object::Pointer> ObjectsList;
  ObjectsList Collection;
};

//------------------------------------------------------------------------------
Collection::Collection()
{
  this->Internal = new CollectionInternals;
}
//------------------------------------------------------------------------------
Collection::~Collection()
{
  delete this->Internal;
}

//------------------------------------------------------------------------------
unsigned long Collection::GetNumberOfItems()
{
  return this->Internal->Collection.size();
}

//------------------------------------------------------------------------------
bool Collection::IsEmpty()
{
  return this->Internal->Collection.empty();
}

//------------------------------------------------------------------------------
void Collection::AddItem(Item i)
{
  this->Internal->Collection.push_back(i);
}

//------------------------------------------------------------------------------
void Collection::Clear() //RemoveAllItems()
{
  this->Internal->Collection.clear();
}

//------------------------------------------------------------------------------
void Collection::RemoveItem (unsigned long i)
{
  unsigned long size = this->Internal->Collection.size();
  if( i >= size )
    return;
  CollectionInternals::ObjectsList::iterator it = this->Internal->Collection.begin();
  CollectionInternals::ObjectsList::iterator end = this->Internal->Collection.end();
  unsigned long c = 0;
  for( ; c < i; ++it, ++c);
  this->Internal->Collection.erase(it);
  assert( this->Internal->Collection.size() == size-1 );
}

//------------------------------------------------------------------------------
unsigned long Collection::IsItemPresent(Item item)
{
  unsigned long r;
  CollectionInternals::ObjectsList::iterator it = this->Internal->Collection.begin();
  CollectionInternals::ObjectsList::iterator end = this->Internal->Collection.end();
  while( it != end && *it != item)
    {
    ++it;
    ++r;
    }
  if( it != end )
    return r;

  return (unsigned long)-1;
}

//------------------------------------------------------------------------------
void Collection::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os,indent);
//  std::copy(
//    this->Internal->Collection.begin(),
//    this->Internal->Collection.end(),
//    std::ostream_iterator<Item>(os << indent, "\n"));
  CollectionInternals::ObjectsList::iterator it = this->Internal->Collection.begin();
  CollectionInternals::ObjectsList::iterator end = this->Internal->Collection.end();
  for( ; it != end; ++it )
    {
    (*it)->Print(os, indent);
    }
}

} // end namespace mrml
