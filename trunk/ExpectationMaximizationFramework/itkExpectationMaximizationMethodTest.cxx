/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageRegistration9.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/31 14:14:16 $
  Version:   $Revision: 1.29 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkExpectationMaximizationMethod.h"
#include "itkVectorContainer.h"
#include "itkDataObjectDecorator.h"


namespace itk {
//
// Create a type of EM method
// 
template <typename TObservationsZ, typename TUnobservedVariablesYPosterior, typename TParametersTheta >
class ITK_EXPORT myExpectationMaximizationMethod : 
                 public ExpectationMaximizationMethod< 
                                               TObservationsZ, 
                                               TUnobservedVariablesYPosterior, 
                                               TParametersTheta > 
{
public:
  
  typedef myExpectationMaximizationMethod                        Self;

  typedef ExpectationMaximizationMethod< 
                               TObservationsZ, 
                               TUnobservedVariablesYPosterior, 
                               TParametersTheta >                Superclass;

  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( myExpectationMaximizationMethod, ExpectationMaximizationMethod );

  typedef typename Superclass::ObservationsType      ObservationsType;
  typedef typename Superclass::UnobservedVariablesPosteriorType  UnobservedVariablesPosteriorType; 
  typedef typename Superclass::ParametersType        ParametersType;



protected:

  virtual void ComputeExpectation() 
     {
     }

  virtual void ComputeMaximization() 
     {
     }

  virtual bool Converged() const
     {
     return false;
     }


};

}


int main( int argc, char *argv[] )
{

  if( argc < 2 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << "   " << std::endl;
    return 1;
    }

  // Attempt a simplified version of an EM problem
  //
  // 
  typedef itk::VectorContainer< unsigned int, double >  ObservedVariablesPayloadType;
  typedef itk::VectorContainer< unsigned int, double >  UnobservedVariablesPayloadType;
  typedef itk::VectorContainer< unsigned int, double >  ParametersThetaPayloadType;


  // 
  //  Wrappers for making this classes pass through the pipeline
  //
  typedef itk::DataObjectDecorator< ObservedVariablesPayloadType >     ObservedVariablesType;
  typedef itk::DataObjectDecorator< UnobservedVariablesPayloadType >   UnobservedVariablesType;
  typedef itk::DataObjectDecorator< ParametersThetaPayloadType >       ParametersThetaType;
  


  
  //
  //  Declare the Expectation Maximization Method 
  //
  typedef itk::myExpectationMaximizationMethod< 
                              ObservedVariablesType,
                              UnobservedVariablesType,
                              ParametersThetaType >   
                                    ExpectationMaximizationMethodType;

  //
  // Construct one
  //
  ExpectationMaximizationMethodType::Pointer  emMethod =
                           ExpectationMaximizationMethodType::New();


  // 
  // Construct the input data
  //
  ObservedVariablesType::Pointer observedVariables = ObservedVariablesType::New();
  

  emMethod->SetObservations( observedVariables );



  // 
  //  Trigger the execute of the method
  //
  try
    {
    emMethod->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excp << std::endl;
    return 1;
    }

    
  // 
  //  Retrieve the results
  //
  //ParametersThetaType::ConstPointer parameters = emMethod->GetParameters();
//  emMethod->GetParameters();
  
//  UnobservedVariablesType::ConstPointer unobservedVariables = 
//                                                 emMethod->GetUnobservedVariablesPosterior();

  return 0;

}


