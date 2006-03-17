/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: Collection.cxx,v $
Date:      $Date: 2006/03/03 22:26:40 $
Version:   $Revision: 1.11 $

=========================================================================auto=*/

#ifndef __mrmlCollection_h
#define __mrmlCollection_h

#include "mrmlObject.h"
#include "mrmlObjectFactory.h"
#include <list>
#include <string>

namespace mrml
{
class CollectionInternals;
class MRMLCommon_EXPORT Collection : public Object
{
public:
  typedef Collection Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  // Description:
  mrmlTypeMacro(Self, Superclass);
  mrmlNewMacro(Self);

  typedef Object* Item;
  void AddItem(Item elem);
  void RemoveItem (unsigned long i);
  void RemoveAllItems();

  // Not garantee to be o(log(n)) in all std::list implementation
  unsigned long GetNumberOfItems();
  bool IsEmpty();
  Object* GetItemAsObject (unsigned long i);

protected:
  Collection();
  ~Collection();

  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  Collection(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  CollectionInternals *Internal;
};
} // end namespace mrml
#endif
