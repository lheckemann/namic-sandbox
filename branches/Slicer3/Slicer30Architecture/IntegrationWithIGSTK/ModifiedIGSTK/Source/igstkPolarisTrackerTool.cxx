/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkPolarisTrackerTool.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 20:48:04 $
  Version:   $Revision: 1.2 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkPolarisTrackerTool.h"

namespace igstk
{

/** Constructor (initializes Polaris-specific tool values) */
PolarisTrackerTool::PolarisTrackerTool():m_StateMachine(this)
{
}

/** Destructor */
PolarisTrackerTool::~PolarisTrackerTool()
{
}

/** Print Self function */
void PolarisTrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


}
