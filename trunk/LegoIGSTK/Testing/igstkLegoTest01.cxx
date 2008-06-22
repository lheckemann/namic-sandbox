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

  // in and out ports
  const int IN_1 = 0;
  const int IN_2 = 1;
  const int IN_3 = 2;
  const int IN_4 = 3;
  const int OUT_A = 0;
  const int OUT_B = 1;
  const int OUT_C = 2;


  std::cout << "OpenLegoUSB()" << std::endl;
  int open = nxtUSB.OpenLegoUSB();
  if( open == 0 )
    {
    std::cerr << "Failed opening" << std::endl;
    std::cerr << nxtUSB.GetStatus() << std::endl;
    std::cerr << std::endl << std::endl;
    std::cerr << std::endl << std::endl;
    std::cerr << "IMPORTANT!: Did you remember to run it as ROOT (or sudo)?";
    std::cerr << std::endl << std::endl;
    std::cerr << "IMPORTANT!: Did you turned the LEGO NXT controller ON ?";
    std::cerr << std::endl << std::endl;
    return EXIT_FAILURE;
    }

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

  int powerA = 5; 
  int powerB = 5; 
  int powerC = 5; 

  int motorADistanceUp = 5;
  int motorBDistanceUp = 5;
  int motorCDistanceUp = 5;

  std::cout << "MoveMotor( OUT_A, powerA, motorADistanceUp)" << std::endl;
  nxtUSB.MoveMotor( OUT_A, powerA,  motorADistanceUp );
  nxtUSB.MoveMotor( OUT_A, powerA, -motorADistanceUp );

  std::cout << "MoveMotor( OUT_B, powerB, motorBDistanceUp)" << std::endl;
  nxtUSB.MoveMotor( OUT_B, powerB,  motorBDistanceUp );
  nxtUSB.MoveMotor( OUT_B, powerB, -motorBDistanceUp );

  std::cout << "MoveMotor( OUT_C, powerC, motorCDistanceUp)" << std::endl;
  nxtUSB.MoveMotor( OUT_C, powerC,  motorCDistanceUp );
  nxtUSB.MoveMotor( OUT_C, powerC, -motorCDistanceUp );

  std::cout << "CloseLegoUSB()" << std::endl;
  nxtUSB.CloseLegoUSB();

  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}
