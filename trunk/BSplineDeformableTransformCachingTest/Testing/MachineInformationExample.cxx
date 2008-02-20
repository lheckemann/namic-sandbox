/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: DeformableRegistration8.cxx,v $
  Language:  C++
  Date:      $Date: 2007/11/22 00:30:16 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "MachineInformation.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
  if (argc < 2 )
    {
    MachineInformation::DescribeMachine( std::cout );
    }
  else
    {
    std::string filename = argv[1];
    std::ofstream output( filename.c_str() );
    MachineInformation::DescribeMachineLatex( output );
    }

  return 0;
}
