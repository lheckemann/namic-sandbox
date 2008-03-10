/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: RegistrationExamples7.cxx,v $
  Language:  C++
  Date:      $Date: 2006/05/14 12:16:23 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// this file defines the RegistrationExamples for the test driver
// and all it expects is that you have a function called RegisterTests

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <iostream>
#include "itkTestMain.h" 


void RegisterTests()
{
  REGISTER_TEST(ImageRegistration13Test);
  REGISTER_TEST(ImageRegistration13TestNoPDFJacobian);
  REGISTER_TEST(ImageRegistration13TestNoCaching);
}

#undef main
#define main  ImageRegistration13Test
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate13
#include "ImageRegistration13.cxx"

#undef main
#define MattesNoPDFJacobian
#define main  ImageRegistration13TestNoPDFJacobian
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate13NoPDFJacobian
#include "ImageRegistration13.cxx"
#undef MattesNoPDFJacobian

#undef main
#define MattesNoCaching
#define main  ImageRegistration13TestNoCaching
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate13NoCaching
#include "ImageRegistration13.cxx"
#undef MattesNoCaching

