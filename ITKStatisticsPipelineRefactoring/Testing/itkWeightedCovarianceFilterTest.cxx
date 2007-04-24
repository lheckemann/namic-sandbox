/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWeightedCovarianceFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/26 15:55:14 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkWeightedCovarianceFilter.h"
#include "itkListSample.h"
#include "itkFixedArray.h"
#include "itkMeasurementVectorTraits.h"
#include "itkFunctionBase.h"

const unsigned int                  MeasurementVectorSize = 3;
const unsigned int                  numberOfMeasurementVectors = 3;
unsigned int                        counter = 0;

typedef itk::FixedArray< 
  float, MeasurementVectorSize >             MeasurementVectorType;

namespace itk {
namespace Statistics {
template < class TSample >
class MyWeightedCovarianceFilter : public WeightedCovarianceFilter< TSample >
{
 public:
  typedef MyWeightedCovarianceFilter           Self;
  typedef WeightedCovarianceFilter<TSample>     Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>             ConstPointer;
  typedef TSample                              SampleType;

  itkNewMacro(Self);

  //method to invoke MakeOutput with index value different
  //from one or zero. This is to check if an exception will be
  // thrown
  void CreateInvalidOutput()
    {
    unsigned int index=3;
    Superclass::MakeOutput( index );
    }
};
}
}


class WeightedCovarianceTestFunction :
  public itk::FunctionBase< MeasurementVectorType, double >
{
public:
  /** Standard class typedefs. */
  typedef WeightedCovarianceTestFunction Self;
  typedef itk::FunctionBase< MeasurementVectorType, double > Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  
  /** Standard macros. */
  itkTypeMacro(WeightedCovarianceTestFunction, FunctionBase);
  itkNewMacro(Self) ;

  /** Input type */
  typedef MeasurementVectorType InputType;

  /** Output type */
  typedef double OutputType;

  /**Evaluate at the specified input position */
  OutputType Evaluate( const InputType& input ) const 
  {
    MeasurementVectorType measurements ;
    // set the weight factor of the measurment 
    // vector with valuev[2, 2] to 0.5.
    return 1.0 ;
  }

protected:
  WeightedCovarianceTestFunction() {}
  ~WeightedCovarianceTestFunction() {}
} ; // end of class



int itkWeightedCovarianceFilterTest(int, char* [] ) 
{
  std::cout << "WeightedCovarianceFilter test \n \n";

 typedef itk::Statistics::ListSample< 
    MeasurementVectorType >                    SampleType;

  typedef itk::Statistics::MyWeightedCovarianceFilter< SampleType > 
    FilterType;

  typedef FilterType::MatrixType                 CovarianceMatrixType; 

  FilterType::Pointer filter = FilterType::New() ;

  MeasurementVectorType               measure;
  
  SampleType::Pointer sample = SampleType::New();

  sample->SetMeasurementVectorSize( MeasurementVectorSize ); 

  measure[0] =  4.00;
  measure[1] =  2.00;
  measure[2] =  0.60;
  sample->PushBack( measure );

  measure[0] =  4.20;
  measure[1] =  2.10;
  measure[2] =  0.59;
  sample->PushBack( measure );

  measure[0] =  3.90;
  measure[1] =  2.00;
  measure[2] =  0.58;
  sample->PushBack( measure );

  measure[0] =  4.30;
  measure[1] =  2.10;
  measure[2] =  0.62;
  sample->PushBack( measure );

  measure[0] =  4.10;
  measure[1] =  2.20;
  measure[2] =  0.63;
  sample->PushBack( measure );

  std::cout << filter->GetNameOfClass() << std::endl;
  filter->Print(std::cout);

  //Invoke update before adding an input. An exception should be 
  //thrown.
  try
    {
    filter->Update();
    std::cerr << "Exception should have been thrown since \
                    Update() is invoked without setting an input" << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    return EXIT_FAILURE;
    }    

  if ( filter->GetInput() != NULL )
    {
    std::cerr << "GetInput() should return NULL if the input \
                     has not been set" << std::endl;
    return EXIT_FAILURE;
    }

  //test if exception is thrown if a derived class tries to create
  // an invalid output 
  try
    {
    filter->CreateInvalidOutput();
    std::cerr << "Exception should have been thrown: " << std::endl;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    return EXIT_FAILURE;
    }
  
  filter->ResetPipeline();

  // Run the filter with no weights 
  filter->SetInput( sample );

  try
    {
    filter->Update();
    std::cerr << "Failed to throw exception when weights were missing" << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cout << "Expected exception caught: " << excp << std::endl;
    }    

  //Specify weight
  typedef FilterType::WeightArrayType  WeightArrayType;
  WeightArrayType weightArray(sample->Size());
  weightArray.Fill(1.0);

  filter->SetWeights( weightArray );

  // run with the weights
  try
    {
    filter->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    return EXIT_FAILURE;
    }    


  MeasurementVectorType  mean = filter->GetMean();
  CovarianceMatrixType matrix = filter->GetCovarianceMatrix();

  std::cout << "Mean: "              << mean << std::endl;
  std::cout << "Covariance Matrix: " << matrix << std::endl;

  //Check the results

  double epsilon = 1e-2;

  float value3[3] = {4.10, 2.08, 0.604};

  MeasurementVectorType  meanExpected3( value3 ); 

  for ( unsigned int i = 0; i < MeasurementVectorSize; i++ )
    {
    if ( fabs( meanExpected3[i] - mean[i] ) > epsilon )
      {
      std::cerr << "The computed mean value is incorrrect" << std::endl;
      return EXIT_FAILURE;
      }
    }

 CovarianceMatrixType  matrixExpected( MeasurementVectorSize, MeasurementVectorSize );

 matrixExpected[0][0] = 0.025;
 matrixExpected[0][1] = 0.0075;
 matrixExpected[0][2] = 0.00175;

 matrixExpected[1][0] = 0.0075;
 matrixExpected[1][1] = 0.0070;
 matrixExpected[1][2] = 0.00135;

 matrixExpected[2][0] = 0.00175;
 matrixExpected[2][1] = 0.00135;
 matrixExpected[2][2] = 0.00043;

 for ( unsigned int i = 0; i < MeasurementVectorSize; i++ )
  {
  for ( unsigned int j = 0; j < MeasurementVectorSize; j++ )
    if ( fabs( matrixExpected[i][j] - matrix[i][j] ) > epsilon )
      {
      std::cerr << "Computed covariance matrix value is incorrrect" << std::endl;
      return EXIT_FAILURE;
      }
    }

  filter->SetWeights( weightArray );

  std::cout << "Weight array: " << filter->GetWeights() << std::endl;

  try
    {
    filter->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    return EXIT_FAILURE;
    }    
 
  std::cout << "Weight array: " << filter->GetWeights() << std::endl;

  mean = filter->GetMean();
  matrix = filter->GetCovarianceMatrix();

  std::cout << "Mean: "              << mean << std::endl;
  std::cout << "Covariance Matrix: " << matrix << std::endl;

  for ( unsigned int i = 0; i < MeasurementVectorSize; i++ )
    {
    if ( fabs( meanExpected3[i] - mean[i] ) > epsilon )
      {
      std::cerr << "The computed mean value is incorrrect" << std::endl;
      return EXIT_FAILURE;
      }
    }

  for ( unsigned int i = 0; i < MeasurementVectorSize; i++ )
   {
  for ( unsigned int j = 0; j < MeasurementVectorSize; j++ )
    if ( fabs( matrixExpected[i][j] - matrix[i][j] ) > epsilon )
      {
      std::cerr << "Computed covariance matrix value is incorrrect" << std::endl;
      return EXIT_FAILURE;
      }
    }


  //set  a constant 1.0 weight using a function
  WeightedCovarianceTestFunction::Pointer weightFunction = WeightedCovarianceTestFunction::New(); 
  filter->SetWeightFunction( weightFunction.GetPointer() );

  try
    {
    filter->Update();
    }
  catch ( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught: " << excp << std::endl;
    return EXIT_FAILURE;
    }    

  mean = filter->GetMean();
  matrix = filter->GetCovarianceMatrix();

  std::cout << "Mean: "              << mean << std::endl;
  std::cout << "Covariance Matrix: " << matrix << std::endl;

  for ( unsigned int i = 0; i < MeasurementVectorSize; i++ )
    {
    if ( fabs( meanExpected3[i] - mean[i] ) > epsilon )
      {
      std::cerr << "The computed mean value is incorrrect" << std::endl;
      return EXIT_FAILURE;
      }
    }

  for ( unsigned int i = 0; i < MeasurementVectorSize; i++ )
   {
  for ( unsigned int j = 0; j < MeasurementVectorSize; j++ )
    if ( fabs( matrixExpected[i][j] - matrix[i][j] ) > epsilon )
      {
      std::cerr << "Computed covariance matrix value is incorrrect" << std::endl;
      return EXIT_FAILURE;
      }
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}



