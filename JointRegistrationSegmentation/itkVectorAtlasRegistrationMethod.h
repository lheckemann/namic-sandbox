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
#ifndef __itkVectorAtlasRegistrationMethod_h
#define __itkVectorAtlasRegistrationMethod_h


#include "itkImageRegistrationMethod2.h"
#include "itkKullbackLeiblerDivergenceImageToImageMetric.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkAmoebaOptimizer.h"
#include "itkCenteredTransformGeometricInitializer.h"
#include "itkVectorImage.h"
#include "itkVersorRigid3DTransform.h"


namespace itk {


/** \brief  Specific implementation of the Expectation Maximization  Algorithm.
 *
 *  \class VectorAtlasRegistrationMethod is the base class for the family of
 *  Expectation and Maximization algorithms. This class provides the
 *  functionalities that are common to all Expectation Maximization algorithms.
 *  Specific instances of the EM algorithm will be implemented as derived
 *  classes from this base class.
 *
 */ 
 
template < class TFixedImage, class TMovingImage >
class VectorAtlasRegistrationMethod : public ProcessObject
{

public:

  typedef VectorAtlasRegistrationMethod             Self;
  typedef ProcessObject                             Superclass;
  typedef SmartPointer< Self >                      Pointer;
  typedef SmartPointer< const Self >                ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( VectorAtlasRegistrationMethod, ProcessObject );


public:

  typedef TFixedImage                               FixedImageType;
  typedef TMovingImage                              MovingImageType;
  typedef VersorRigid3DTransform< double >          TransformType;

  /** Set/Get the Fixed image. */
  void SetFixedImage( const FixedImageType * fixedImage );
  itkGetConstObjectMacro( FixedImage, FixedImageType ); 

  /** Set/Get the Moving image. */
  void SetMovingImage( const MovingImageType * movingImage );
  itkGetConstObjectMacro( MovingImage, MovingImageType );

  /** Get the transform. This is the result of the regisration only if invoked
   * after calling Update() */
  itkGetConstObjectMacro( Transform, TransformType );


protected:

  
  typedef AmoebaOptimizer                      OptimizerType;

  typedef NearestNeighborInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  typedef ImageRegistrationMethod2< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  typedef KullbackLeiblerDivergenceImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;


  /** Constructor */
  VectorAtlasRegistrationMethod();

  /** Destructor */
  ~VectorAtlasRegistrationMethod();

  /** Initialize the allocation of the Weights image Initialize the corrected
   *  image and the weights. Throws exceptions is something goes wrong */
  void Initialize();

  /** This method compute the update of parameters to be estimated based on the
   *  weights and the list of samples (observations) */
  void ComputeRegistration();


    
private:

  VectorAtlasRegistrationMethod(const Self&) ; //purposely not implemented

  void operator=(const Self&) ; //purposely not implemented


  typename MetricType::Pointer           m_Metric;
  typename OptimizerType::Pointer        m_Optimizer;
  typename InterpolatorType::Pointer     m_Interpolator;
  typename RegistrationType::Pointer     m_Registration;
  typename TransformType::Pointer        m_Transform;
  typename FixedImageType::Pointer       m_FixedImage;
  typename MovingImageType::Pointer      m_MovingImage;

};

}  // end of itk namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVectorAtlasRegistrationMethod.txx"
#endif


#endif

