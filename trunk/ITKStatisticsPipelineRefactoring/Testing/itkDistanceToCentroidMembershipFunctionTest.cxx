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
#include "itkDistanceToCentroidMembershipFunction.h"
#include "itkObjectFactory.h"
#include "itkEuclideanDistanceMetric.h"

namespace itk {
namespace Statistics {
namespace DistanceToCentroidMembershipFunctionTest {

template <class TMeasurementVector>
class MyDistanceToCentroidMembershipFunction : public DistanceToCentroidMembershipFunction< TMeasurementVector >
{
public:
  /** Standard class typedef. */
  typedef MyDistanceToCentroidMembershipFunction  Self;
  typedef DistanceToCentroidMembershipFunction< TMeasurementVector > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Standard macros */
  itkTypeMacro(MyDistanceToCentroidMembershipFunction, DistanceToCentroidMembershipFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self) ;
 
  /** Evaluate membership score */
  double Evaluate(const TMeasurementVector &x) const
    {
    double score;
    score = 1;
    return score;
    } 
};

}
}
}
int itkDistanceToCentroidMembershipFunctionTest(int, char* [] )
{

  const unsigned int MeasurementVectorSize = 17;

  typedef itk::FixedArray< 
    float, MeasurementVectorSize >  MeasurementVectorType;

  typedef itk::Statistics::DistanceToCentroidMembershipFunctionTest::MyDistanceToCentroidMembershipFunction< 
    MeasurementVectorType >   DistanceToCentroidMembershipFunctionType;

  DistanceToCentroidMembershipFunctionType::Pointer function = DistanceToCentroidMembershipFunctionType::New();

  std::cout << function->GetNameOfClass() << std::endl;
  std::cout << function->DistanceToCentroidMembershipFunctionType::Superclass::GetNameOfClass() << std::endl;

  typedef itk::Statistics::EuclideanDistanceMetric< MeasurementVectorType >  DistanceMetricType;

  DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New();

  function->SetDistanceMetric( distanceMetric );

  function->Print(std::cout);

  function->SetMeasurementVectorSize( MeasurementVectorSize ); // for code coverage

  if( function->GetMeasurementVectorSize() != MeasurementVectorSize )
    {
    std::cerr << "GetMeasurementVectorSize() Failed !" << std::endl;
    return EXIT_FAILURE;
    }

  //Test if an exception will be thrown if we try to resize the measurement vector
  //size
  try
    {
    function->SetMeasurementVectorSize( MeasurementVectorSize + 1 ); 
    std::cerr << "Exception should have been thrown since we are trying to resize\
                  non-resizeable measurement vector type " << std::endl;
    return EXIT_FAILURE; 
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Caughted expected exception: " << excp << std::endl;
    }

  return EXIT_SUCCESS;
}
