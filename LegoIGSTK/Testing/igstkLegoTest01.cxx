/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadEdgeMeshTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2008/01/15 13:43:14 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>
#include "NXT_USB.h"

int main( int , char* [] )
{
  NXT_USB nxtUSB;

  std::cout << "OpenLegoUSB()" << std::endl;
  nxtUSB.OpenLegoUSB();

  std::cout << "GetDeviceFilename() = " 
            << nxtUSB.GetDeviceFilename()
            << std::endl;

  std::cout << "GetIDVendor() = " << nxtUSB.GetIDVendor() << std::endl;
  std::cout << "GetIDProduct() = " << nxtUSB.GetIDProduct() << std::endl;

  std::cout << "GetStatus() = " 
            << nxtUSB.GetStatus()
            << std::endl;

  std::cout << "PlayTone(440,2)" << std::endl;
  nxtUSB.PlayTone(440,2);

  std::cout << "PlayTone(880,2)" << std::endl;
  nxtUSB.PlayTone(880,2);

  std::cout << "CloseLegoUSB()" << std::endl;
  nxtUSB.CloseLegoUSB();

  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}
