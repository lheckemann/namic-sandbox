/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFITSImageIOFactory.cxx,v $
  Language:  C++
  Date:      $Date: 2004/01/08 00:39:06 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkFITSImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkFITSImageIO.h"
#include "itkVersion.h"

  
namespace itk
{

FITSImageIOFactory::FITSImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "itkFITSImageIO",
                         "FITS Image IO",
                         1,
                         CreateObjectFunction<FITSImageIO>::New());
}
  
FITSImageIOFactory::~FITSImageIOFactory()
{
}

const char* 
FITSImageIOFactory::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

const char* 
FITSImageIOFactory::GetDescription() const
{
  return "FITS ImageIO Factory, allows the loading of FITS images into Insight";
}

} // end namespace itk

