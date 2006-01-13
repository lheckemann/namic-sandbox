/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageRegistrationMethod2.h,v $
  Language:  C++
  Date:      $Date: 2005/11/01 21:57:22 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVectorAtlasRegistrationMethod_txx
#define __itkVectorAtlasRegistrationMethod_txx

#include "itkVectorAtlasRegistrationMethod.h"

namespace itk {



template < class TFixedImageType, class TMovingImage >
VectorAtlasRegistrationMethod< TFixedImageType, TMovingImage >
::VectorAtlasRegistrationMethod()
{
   this->m_Metric        = MetricType::New();
   this->m_Optimizer     = OptimizerType::New();  
   this->m_Interpolator  = InterpolatorType::New();
   this->m_Transform     = TransformType::New();
   this->m_Registration  = RegistrationType::New();
}



template < class TFixedImageType, class TMovingImage >
VectorAtlasRegistrationMethod< TFixedImageType, TMovingImage >
::~VectorAtlasRegistrationMethod()
{
}


   

template < class TFixedImageType, class TMovingImage >
void
VectorAtlasRegistrationMethod< TFixedImageType, TMovingImage >
::GenerateData()
{
  this->Initialize();
  this->ComputeRegistration();
}

 
  

template < class TFixedImageType, class TMovingImage >
void
VectorAtlasRegistrationMethod< TFixedImageType, TMovingImage >
::Initialize()
{

  this->m_Registration->SetMetric(        this->m_Metric        );
  this->m_Registration->SetOptimizer(     this->m_Optimizer     );
  this->m_Registration->SetInterpolator(  this->m_Interpolator  );


  typedef itk::CenteredTransformGeometricInitializer< 
                                    TransformType, 
                                    FixedImageType, 
                                    MovingImageType 
                                                 >  TransformInitializerType;

  typename TransformInitializerType::Pointer initializer = 
                                          TransformInitializerType::New();

  this->m_Registration->SetFixedImage(  this->m_FixedImage  );
  this->m_Registration->SetMovingImage( this->m_MovingImage );


  this->m_Registration->SetFixedImageRegion( 
                               this->m_FixedImage->GetBufferedRegion() );


  initializer->SetTransform(   this->m_Transform );
  initializer->SetFixedImage(  this->m_FixedImage );
  initializer->SetMovingImage( this->m_MovingImage );

  initializer->InitializeTransform();

  this->m_Registration->SetTransform( this->m_Transform.GetPointer() );

  this->m_Registration->SetInitialTransformParameters( 
                                 this->m_Transform->GetParameters() );

  double translationScale = 1.0 / 10.0;

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( this->m_Transform->GetNumberOfParameters() );

  optimizerScales[0] =  1.0;
  optimizerScales[1] =  1.0;
  optimizerScales[2] =  1.0;
  optimizerScales[3] =  translationScale;
  optimizerScales[4] =  translationScale;
  optimizerScales[5] =  translationScale;

  this->m_Optimizer->SetScales( optimizerScales );

  //
  // Reasonable defaults
  //
  this->m_Optimizer->SetParametersConvergenceTolerance( 0.00001 ); 
  this->m_Optimizer->SetFunctionConvergenceTolerance( 0.0001 );
  this->m_Optimizer->SetMaximumNumberOfIterations( 1500 );

  this->m_Optimizer->MinimizeOn();

}



 

template < class TFixedImageType, class TMovingImage >
void
VectorAtlasRegistrationMethod< TFixedImageType, TMovingImage >
::ComputeRegistration()
{

  try 
    { 
    std::cout << "Starting registration " << std::endl;
    this->m_Registration->StartRegistration(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return;
    } 

  // THIS IS JUST FOR DEBUGGING
  OptimizerType::ParametersType finalParameters = 
                    this->m_Registration->GetLastTransformParameters();

  // set some output information,
  char results[1024];
  typedef TransformType::VersorType VersorType;
  
  VersorType versor = this->m_Transform->GetVersor();
  
  TransformType::OffsetType offset = this->m_Transform->GetOffset();
  
  typedef VersorType::VectorType   AxisType;

  AxisType axis = versor.GetAxis();

  sprintf(results,"Translation: %g %g %g\nRotation Axis %f %f %f %f\nOffset: %g %g %g", 
          finalParameters[3],
          finalParameters[4],
          finalParameters[5],
          axis[0],
          axis[1],
          axis[2],
          versor.GetAngle(),
          offset[0],
          offset[1],
          offset[2]
          );

  std::cout << results << std::endl;

}



} // end namespace itk


#endif

