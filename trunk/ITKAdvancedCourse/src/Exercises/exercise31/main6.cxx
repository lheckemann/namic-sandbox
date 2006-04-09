/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: main6.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/18 21:57:55 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "itkMultiThreader.h"
#include "itkSimpleFastMutexLock.h"
#include "itkMutexLock.h"
#include "itkRealTimeClock.h"
#include <iostream>


static unsigned long Counter = 0;
static itk::MutexLock::Pointer mutex;

int main()
{

  mutex = itk::MutexLock::New();

  itk::RealTimeClock::Pointer  clock  =  itk::RealTimeClock::New();

  const double T0 = clock->GetTimestamp();

  const unsigned long N =400000L;

  
    for(unsigned long i=0; i<N; i++) 
      {
      Counter += i;
      }

  const double T1 = clock->GetTimestamp();
  Counter = 0;

    for(unsigned long i=0; i<N; i++) 
      {
      mutex->Lock();
      Counter += i;
      mutex->Unlock();
      }

   const double T2 = clock->GetTimestamp();

   const double Time0 = (T1 - T0);
   const double Mean0 = Time0 / N;

   std::cout << "Time for " << N << " loop iteration  = " << Time0 << " seconds " << std::endl;
   std::cout << "Mean time of loop iteration " << Mean0 << " seconds " << std::endl;
   
   std::cout << std::endl;

   const double Time = (T2 - T1);
   const double Mean = Time / N;

   std::cout << "Time for " << N << " Lock/Unlock()s  = " << Time << " seconds " << std::endl;
   std::cout << "Mean time of Lock/Unlock = " << Mean << " seconds " << std::endl;

   std::cout << std::endl;
   std::cout << "Ratio = " << ( Mean / Mean0 ) << std::endl;

   return 0;
}



