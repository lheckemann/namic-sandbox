/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkMRImageSpatialObjectTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/25 02:16:49 $
  Version:   $Revision: 1.1 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkMRImageSpatialObject.h"


int igstkMRImageSpatialObjectTest( int , char* [] )
{

  typedef igstk::MRImageSpatialObject         ImageSpatialObjectType;

  /* Instantiate one MR image */
  ImageSpatialObjectType::Pointer mrImage =  ImageSpatialObjectType::New();

  mrImage->Print( std::cout );

  return EXIT_SUCCESS;
}

