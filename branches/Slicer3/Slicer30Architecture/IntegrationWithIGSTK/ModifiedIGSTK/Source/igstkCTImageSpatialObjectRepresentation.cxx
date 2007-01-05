/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkCTImageSpatialObjectRepresentation.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 20:48:04 $
  Version:   $Revision: 1.2 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCTImageSpatialObjectRepresentation.h"

namespace igstk
{ 

/** Constructor */
CTImageSpatialObjectRepresentation
::CTImageSpatialObjectRepresentation():m_StateMachine(this)
{

} 

/** Print Self function */
void CTImageSpatialObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
