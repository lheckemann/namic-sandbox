/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkTubeObject.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 21:24:05 $
  Version:   $Revision: 1.2 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTubeObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
TubeObject::TubeObject():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_TubeSpatialObject = TubeSpatialObjectType::New();
  this->RequestSetSpatialObject( m_TubeSpatialObject );
} 

/** Destructor */
TubeObject::~TubeObject()  
{
}


/** Add a point to the tube */
void TubeObject::AddPoint(PointType & point)
{
  m_TubeSpatialObject->GetPoints().push_back(point);
}

/** Return the number of points in the tube */
unsigned int TubeObject::GetNumberOfPoints(void) const
{
  return m_TubeSpatialObject->GetPoints().size();
}

/** Return a given point */
const TubeObject::PointType * TubeObject::GetPoint(unsigned int pointId) const
{
  if(pointId >= m_TubeSpatialObject->GetPoints().size())
    {
    return NULL;
    }

  PointListType &points = m_TubeSpatialObject->GetPoints();
  return &(points[pointId]);
}


/** Return the internal list of points */
const TubeObject::PointListType TubeObject::GetPoints() const
{
  return m_TubeSpatialObject->GetPoints();
}


/** Print object information */
void TubeObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_TubeSpatialObject )
  {
    os << indent << this->m_TubeSpatialObject << std::endl;
  }
}


} // end namespace igstk

