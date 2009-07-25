/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/BSplineDeformableRegistration.cxx $
  Language:  C++
  Date:      $Date: 2009-04-10 13:33:12 -0400 (Fri, 10 Apr 2009) $
  Version:   $Revision: 9136 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "BWHCPFIRegistrationCLP.h"

#include "CPFIBSplineRegistration.txx"


int main( int argc, char * argv[] )
{
  
  // Print out the arguments (need to add --echo to the argument list 
  // 
  std::vector<char *> myVargs;
  for (int vi=0; vi < argc; ++vi) myVargs.push_back(argv[vi]);
  myVargs.push_back(const_cast<char *>("--echo"));
  
  argc = myVargs.size();
  argv = &(myVargs[0]);

  PARSE_ARGS;

  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  try
    {
    itk::GetImageType (FixedImageFileName, pixelType, componentType);

    // This filter handles all types

    switch (componentType)
      {
      case itk::ImageIOBase::CHAR:
      case itk::ImageIOBase::UCHAR:
      case itk::ImageIOBase::USHORT:
      case itk::ImageIOBase::SHORT:
        if (Dimension == 3)
          {
          CPFIBSplineRegistration<short, 3> registration;
          return registration.DoIt(argc, argv);
          }
        else // Dimensions == 2
          {
          CPFIBSplineRegistration<short, 2> registration;
          return registration.DoIt(argc, argv);
          }
        break;

      case itk::ImageIOBase::ULONG:
      case itk::ImageIOBase::LONG:
      case itk::ImageIOBase::UINT:
      case itk::ImageIOBase::INT:
      case itk::ImageIOBase::DOUBLE:
      case itk::ImageIOBase::FLOAT:
        if (Dimension == 3)
          {
          CPFIBSplineRegistration<float, 3> registration;
          return registration.DoIt(argc, argv);
          }
        else // Dimensions == 2
          {
          CPFIBSplineRegistration<float, 2> registration;
          return registration.DoIt(argc, argv);
          }
        break;

      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }
  catch( itk::ExceptionObject &excep)
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
