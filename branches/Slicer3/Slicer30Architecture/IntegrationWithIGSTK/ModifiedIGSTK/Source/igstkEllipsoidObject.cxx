/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkEllipsoidObject.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 20:48:04 $
  Version:   $Revision: 1.5 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkEllipsoidObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
EllipsoidObject::EllipsoidObject():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_EllipseSpatialObject = EllipseSpatialObjectType::New();
  this->RequestSetSpatialObject( m_EllipseSpatialObject );
} 

/** Destructor */
EllipsoidObject::~EllipsoidObject()  
{
}

/** Set the radius along each axis */
void EllipsoidObject::SetRadius( double rx, double ry, double rz )
{
  EllipseSpatialObjectType::ArrayType radius;
  radius[0] = rx;
  radius[1] = ry;
  radius[2] = rz;
  m_EllipseSpatialObject->SetRadius( radius );
}


/** Set the radii along each axis */
void EllipsoidObject::SetRadius( const ArrayType & radii )
{
  m_EllipseSpatialObject->SetRadius( radii );
}


/** Get the radii along each axis */
const EllipsoidObject::ArrayType & 
EllipsoidObject::GetRadius() const
{
  return m_EllipseSpatialObject->GetRadius();
}



/** Print Self function */
void EllipsoidObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  if( m_EllipseSpatialObject )
  {
    os << indent << m_EllipseSpatialObject << std::endl; 
  }
}


} // end namespace igstk

