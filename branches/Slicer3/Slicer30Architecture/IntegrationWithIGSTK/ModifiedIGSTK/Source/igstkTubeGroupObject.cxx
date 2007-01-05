/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkTubeGroupObject.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 21:24:05 $
  Version:   $Revision: 1.2 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTubeGroupObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
TubeGroupObject::TubeGroupObject():m_StateMachine(this)
{
}

/** Destructor */
TubeGroupObject::~TubeGroupObject()  
{
}

/** Return a pointer to a tube */
const TubeGroupObject::TubeObjectType * 
TubeGroupObject::GetTube(unsigned long id) const
{
  return dynamic_cast<const TubeObject*>(this->GetObject(id));
}

/** Print object information */
void TubeGroupObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk

