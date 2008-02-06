/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlObject.h"

namespace igtl {

Object::Object()
{
}

void Object::Delete() 
{
}

void Object::Print(std::ostream& os)
{
  //os << "  " << this->GetClassName() << " (" << this << ")\n";
  PrintSelf(os, 4);
}

void Object::PrintSelf(std::ostream& os, int indent)
{
}

// avoid dll boundary problems
#ifdef _WIN32
void* Object::operator new(size_t nSize)
{
  void* p=malloc(nSize);
  return p;
}

void Object::operator delete( void *p )
{
  free(p);
}
#endif 

}
