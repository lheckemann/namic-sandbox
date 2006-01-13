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
#ifndef __itkJointRegistrationSegmentationAlgorithm_h
#define __itkJointRegistrationSegmentationAlgorithm_h



#include "itkExpectationMaximizationImageClassification.h"
#include "itkTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"



namespace itk {

namespace Statistics {

/** \brief  Specific implementation of the Expectation Maximization  Algorithm.
 *
 *  \class JointRegistrationSegmentationAlgorithm is the base class for the family of
 *  Expectation and Maximization algorithms. This class provides the
 *  functionalities that are common to all Expectation Maximization algorithms.
 *  Specific instances of the EM algorithm will be implemented as derived
 *  classes from this base class.
 *
 */ 
 
template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType=float >
class JointRegistrationSegmentationAlgorithm : 
   public ExpectationMaximizationImageClassification< TImageType, 
                                                      TPriorPixelComponentType, 
                                                      TCorrectionPrecisionType >
{

public:

  typedef JointRegistrationSegmentationAlgorithm             Self;
  typedef ExpectationMaximizationImageClassification< 
                              TImageType, 
                              TPriorPixelComponentType, 
                              TCorrectionPrecisionType >         Superclass;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( JointRegistrationSegmentationAlgorithm, ExpectationMaximizationImageClassification );


public:

  typedef TImageType                                   InputImageType;

  itkStaticConstMacro( ImageDimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef typename Superclass::WeightsImageType        WeightsImageType;

  typedef typename Superclass::PriorPixelComponentType PriorPixelComponentType;
  typedef typename Superclass::PriorsImageType         PriorsImageType;
  typedef typename Superclass::PriorsPixelType         PriorsPixelType;
  typedef typename Superclass::CorrectedValueType      CorrectedValueType;
  typedef typename Superclass::LogImageType            LogImageType;
  typedef typename Superclass::LogPixelType            LogPixelType;
  typedef typename Superclass::WeightsImagePointer     WeightsImagePointer;
  typedef typename Superclass::PriorsImagePointer      PriorsImagePointer;
  typedef typename Superclass::InputPixelType          InputPixelType;
  typedef typename Superclass::MeasurementVectorType   MeasurementVectorType;
  typedef typename Superclass::GaussianDensityFunctionType GaussianDensityFunctionType;
  typedef typename Superclass::GaussianDensityFunctionPointer GaussianDensityFunctionPointer;

  typedef typename Superclass::ProportionType          ProportionType;


protected:

  /** Constructor */
  JointRegistrationSegmentationAlgorithm();

  /** Destructor */
  ~JointRegistrationSegmentationAlgorithm();

  /** Register the current weights image with the Class Priors image. */
  void ComputeRegistrationBetweenPriorsAndWeights();

  /** Compute the correction for the inhomogeneity field. */
  void ComputeInhomogeneityCorrection();

  /** Initialize the allocation of the Weights image Initialize the corrected
   *  image and the weights. Throws exceptions is something goes wrong */
  void Initialize();

  /** This method compute the update of parameters to be estimated based on the
   *  weights and the list of samples (observations) */
  void ComputeMaximization();
 
  /** This method updates the weights (or posteriors) based on the current
   *  parameters and the collection of samples (observations). */
  void ComputeExpectation();


  /** Base class of a generic transform */
  typedef   Transform<double, ImageDimension, ImageDimension >   TransformType;

  typedef   typename TransformType::Pointer       TransformPointer;


  typedef   NearestNeighborInterpolateImageFunction< 
                                            PriorsImageType,
                                            double >      InterpolatorType;

  typedef   typename  InterpolatorType::Pointer           InterpolatorPointer;

    
private:

  JointRegistrationSegmentationAlgorithm(const Self&) ; //purposely not implemented

  void operator=(const Self&) ; //purposely not implemented


  typename LogImageType::Pointer              m_LogInputImage;


  typename LogImageType::Pointer              m_CorrectedLogImage;


  TransformPointer                            m_Transform;

  
  InterpolatorPointer                         m_Interpolator;

};

}

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkJointRegistrationSegmentationAlgorithm.txx"
#endif


#endif

