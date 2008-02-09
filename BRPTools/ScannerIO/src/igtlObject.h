/*=========================================================================

  Program:   Open ITK Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlObject_h
#define __igtlObject_h

#include <iostream>
#include "igtlSmartPointer.h"
#include "igtlMacros.h"

namespace igtl
{

class Object
{
public:
  typedef Object              Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlNewMacro(Object);
  virtual void Delete();

#ifdef _WIN32
  // avoid dll boundary problems
  void* operator new( size_t tSize );
  void  operator delete( void* p );
#endif
  
  void Print(std::ostream& os);
  void PrintSelf(std::ostream& os, int intent);

protected:
  Object();
  Object(const Object&) {}
  virtual ~Object() {};
  void operator=(const Object&) {}

};


} // namespace igtl

#endif // __igtlObject_h
