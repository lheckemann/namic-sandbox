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

  if( argc < 4 )
    {
    std::cerr << "Error: missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "motor  power  distance" << std::endl;
    std::cerr << "motor values can be: A, B, C" << std::endl;
    std::cerr << "power values rage : -100 to 100" << std::endl;
    std::cerr << "distance values rage : 0 to 100" << std::endl;
    return EXIT_FAILURE;
    }

  NXT_USB nxtUSB;

  const int motor     = (int)( argv[1][0] - 'A' );
  const int power     = atoi( argv[2] );
  const int distance  = atoi( argv[3] );

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

  nxtUSB.MoveMotor( motor,  power, distance );

  nxtUSB.CloseLegoUSB();

  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}
