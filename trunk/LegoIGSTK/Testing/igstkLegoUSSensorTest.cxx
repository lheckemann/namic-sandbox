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

int main( int argc, char * argv [] )
{

  if( argc < 2 )
    {
    std::cerr << "Error: missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "port  " << std::endl;
    std::cerr << "port values can be: 1, 2, 3, 4" << std::endl;
    return EXIT_FAILURE;
    }

  NXT_USB nxtUSB;

  // Convert port argument from {1,2,3,4} to {0,1,2,3}
  const int port = atoi( argv[1] ) - 1; 

  int open = nxtUSB.OpenLegoUSB();

  if( open == 0 )
    {
    std::cerr << "Failed Opening" << std::endl;
    std::cerr << nxtUSB.GetStatus() << std::endl;
    std::cerr << std::endl << std::endl;
    std::cerr << std::endl << std::endl;
    std::cerr << "IMPORTANT!: Did you remember to run it as ROOT (or sudo)?";
    std::cerr << std::endl << std::endl;
    std::cerr << "IMPORTANT!: Did you turned the LEGO NXT controller ON ?";
    std::cerr << std::endl << std::endl;
    return EXIT_FAILURE;
    }

  nxtUSB.SetSensorUS( port );
  nxtUSB.SetUSContinuous( port );

  for(unsigned int k=0; k<100; k++)
    {
    int distance = nxtUSB.GetUSSensor(port);
    std::cout << distance << std::endl;
    }

  nxtUSB.SetUSOff( port );

  nxtUSB.CloseLegoUSB();

  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}
