/*=========================================================================

  Program:   SpatialObject Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkTubeReader.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/19 23:02:01 $
  Version:   $Revision: 1.3 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTubeReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
TubeReader::TubeReader():m_StateMachine(this)
{ 
  m_Group = GroupObjectType::New();
} 

/** Destructor */
TubeReader::~TubeReader()  
{
}

/** Read the spatialobject file */
void TubeReader::AttemptReadObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::TubeReader::AttemptReadObject called...\n");
  Superclass::AttemptReadObjectProcessing();

  // Do the conversion
  GroupSpatialObjectType::Pointer m_GroupSpatialObject = m_SpatialObjectReader->GetGroup();
  GroupSpatialObjectType::ChildrenListType * children = m_GroupSpatialObject->GetChildren(99999);
  GroupSpatialObjectType::ChildrenListType::const_iterator it = children->begin();

  while(it != children->end())
    {
    if(!strcmp((*it)->GetTypeName(),"TubeSpatialObject"))
      {
      // Create a new TubeObject
      TubeType::Pointer tube = TubeType::New();
      
      const TubeSpatialObjectType * tubes = 
              dynamic_cast< TubeSpatialObjectType * >( it->GetPointer() );

      // Get the points from the tube spatialobject
      typedef TubeSpatialObjectType::PointListType    PointListType;
      typedef PointListType::const_iterator           PointIterator;

      PointIterator pointItr = tubes->GetPoints().begin();
      PointIterator pointEnd = tubes->GetPoints().end();

      while( pointItr != pointEnd )
        {
        TubeType::PointType point;
        point.SetPosition( pointItr->GetPosition() );
        point.SetRadius( pointItr->GetRadius() );
        tube->AddPoint( point );
        pointItr++;
        }

      // For the moment no hierarchy
      m_Group->RequestAddObject(tube);

      }
    else if(!strcmp((*it)->GetTypeName(),"VesselTubeSpatialObject"))
      {
      // Create a new TubeObject
      TubeType::Pointer tube = TubeType::New();
      
      // Get the points from the tube spatialobject
      typedef itk::VesselTubeSpatialObject<3> VesselTubeSpatialObjectType;
      typedef VesselTubeSpatialObjectType::PointListType    PointListType;
      typedef PointListType::const_iterator                 PointIterator;
        
      const VesselTubeSpatialObjectType * vessels =
              dynamic_cast<VesselTubeSpatialObjectType*>( it->GetPointer() );

      PointIterator  pointItr = vessels->GetPoints().begin();
      PointIterator  pointEnd = vessels->GetPoints().end();

      while( pointItr != pointEnd )
        {
        TubeType::PointType point;
        point.SetPosition( pointItr->GetPosition() );
        point.SetRadius( pointItr->GetRadius() );
        tube->AddPoint( point );
        ++pointItr;
        }

      // For the moment no hierarchy
      m_Group->RequestAddObject( tube );

      }
    it++;
    }
  delete children;
}

/** Return the output as a group */
const TubeReader::GroupObjectType *
TubeReader::GetOutput() const
{
  return m_Group;
}

/** Print Self function */
void TubeReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Group = " << m_Group.GetPointer() << std::endl;
}

} // end namespace igstk


