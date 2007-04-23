/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/04/11 13:52:36 $
  Version:   $Revision: 1.2 $

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
#include "itkListSample.h"
#include "itkSampleToSubsampleFilter.h"

namespace itk {
namespace Statistics {
namespace itkSampleToSubsampleFilter1Namespace {

template<class TSample >
class SubsamplerTester : public SampleToSubsampleFilter< TSample >
{
public:
  /** Standard class typedefs. */
  typedef SubsamplerTester                    Self;
  typedef SampleToSubsampleFilter<TSample>    Superclass;
  typedef SmartPointer< Self >                Pointer;
  typedef SmartPointer< const Self >          ConstPointer;

  /** Standard macros */ 
  itkTypeMacro(SubsamplerTester,SampleToSubsampleFilter);
  itkNewMacro(Self);

protected:
  SubsamplerTester() {}
  virtual ~SubsamplerTester() {}
  void PrintSelf(std::ostream& os, Indent indent) const
   {
   this->Superclass::PrintSelf(os,indent);  
   }
  
  void GenerateData()
    {
    }

private:
  SubsamplerTester(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end of itkSampleToSubsampleFilter1Namespace
} // end of Standard namespace
} // end of itk namespace


int itkSampleToSubsampleFilterTest1(int, char* [] )
{

  const unsigned int MeasurementVectorSize = 17;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >  MeasurementVectorType;

  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;

  typedef itk::Statistics::itkSampleToSubsampleFilter1Namespace::SubsamplerTester< SampleType > FilterType;


  SampleType::Pointer sample = SampleType::New();

  FilterType::Pointer filter = FilterType::New();

  //
  // Exercise the Print() method
  //
  filter->Print( std::cout );


  filter->SetInput( sample );

  filter->Update();

  std::cout << "Test Passed !" << std::endl;
  return EXIT_SUCCESS;
}
