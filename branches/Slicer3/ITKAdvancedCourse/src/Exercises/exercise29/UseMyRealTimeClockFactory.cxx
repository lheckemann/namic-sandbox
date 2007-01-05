/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: UseMyRealTimeClockFactory.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/02 23:24:31 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkRealTimeClock.h"
#include <iostream>

int main()
{

  typedef  itk::RealTimeClock   ClockType;

  ClockType::Pointer myClock = ClockType::New();

  myClock->Print( std::cout );

  return 0;
}


