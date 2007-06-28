/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkYAFFImageIOFactory.cxx,v $
  Language:  C++
  Date:      $Date: 2007/03/22 14:28:51 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkYAFFImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkYAFFImageIO.h"
#include "itkVersion.h"

  
namespace itk
{
YAFFImageIOFactory::YAFFImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "itkYAFFImageIO",
                         "YAFF Image IO",
                         1,
                         CreateObjectFunction<YAFFImageIO>::New());
}
  
YAFFImageIOFactory::~YAFFImageIOFactory()
{
}

const char* 
YAFFImageIOFactory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}

const char* 
YAFFImageIOFactory::GetDescription() const
{
  return "YAFF ImageIO Factory, allows the loading of YAFF images into insight";
}

//
// Entry point function for ITK to invoke, in order to create a new instance of
// a factory.
//
extern "C" 
#ifdef _WIN32
__declspec( dllexport ) 
#endif  
  itk::ObjectFactoryBase* itkLoad()
{
  std::cout << "Calling YAFFImageIO itkLoad()" << std::endl;
  return itk::YAFFImageIOFactory::FactoryNew();
}


} // end namespace itk
