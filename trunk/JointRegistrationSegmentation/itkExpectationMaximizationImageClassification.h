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
#ifndef __itkExpectationMaximizationAlgorithm_h
#define __itkExpectationMaximizationAlgorithm_h

#include "itkGaussianDensityFunction.h"
#include "itkVectorImage.h"
#include "itkTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include <vector>


namespace itk {

namespace Statistics {

/** \brief  Generic implementation of the Expectation Maximization  Algorithm.
 *
 *  \class ExpectationMaximizationAlgorithm is the base class for the family of
 *  Expectation and Maximization algorithms. This class provides the
 *  functionalities that are common to all Expectation Maximization algorithms.
 *  Specific instances of the EM algorithm will be implemented as derived
 *  classes from this base class.
 *
 */ 
 
template < class TImageType, class TPriorPixelComponentType, class TCorrectionPrecisionType=float >
class ExpectationMaximizationImageClassification : 
   public Object
{

public:

  typedef ExpectationMaximizationImageClassification             Self;
  typedef Object                                                 Superclass;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( ExpectationMaximizationImageClassification, Object );


public:

  typedef TImageType                                   InputImageType;

  itkStaticConstMacro( ImageDimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef VectorImage< float, ImageDimension >         WeightsImageType;

  typedef TPriorPixelComponentType                     PriorPixelComponentType;

  typedef VectorImage< PriorPixelComponentType, 
                       ImageDimension >                PriorsImageType;

  typedef typename PriorsImageType::PixelType          PriorsPixelType;

  typedef TCorrectionPrecisionType                     CorrectedValueType;

  typedef VectorImage< CorrectedValueType, 
                       ImageDimension >                CorrectedImageType;

  typedef typename WeightsImageType::Pointer           WeightsImagePointer;
  typedef typename PriorsImageType::Pointer            PriorsImagePointer;

  typedef typename  InputImageType::PixelType          MeasurementVectorType;

  typedef GaussianDensityFunction< MeasurementVectorType >  GaussianDensityFunctionType;

  typedef typename GaussianDensityFunctionType::ConstPointer  GaussianDensityFunctionPointer;

  typedef double   ProportionType;

 

  /**  Add a density function to the list of Density functions to use.  */
  void AddIntensityDistributionDensity( 
                  const GaussianDensityFunctionType * gaussian,
                  ProportionType proportion );



  /** This method triggers the computation of the estimation */
  void Update();


  /** Set the input image to be classified. This image may be a vector image
   * with multiple-components */
  void SetInput( const InputImageType * image );


  /** Set the image of priors. This is equivalent to an atlas. */
  void SetClassPrior( const PriorsImageType * image );


  /** Set number of Iterations */
  itkSetMacro( MaximumNumberOfIterations, unsigned long );


protected:

  /** Constructor */
  ExpectationMaximizationImageClassification();

  /** Destructor */
  ~ExpectationMaximizationImageClassification();

  /** This method performs the actual computation of the classification */
  void GenerateData();

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

  ExpectationMaximizationImageClassification(const Self&) ; //purposely not implemented

  void operator=(const Self&) ; //purposely not implemented



  /** Container that holds the list of Gaussian distributions for each one of the Structures. */
  typedef std::vector< GaussianDensityFunctionPointer 
                                        >     IntensityDistributionContainerType;


  typedef std::vector< ProportionType >       ProportionsContainerType;


  IntensityDistributionContainerType          m_ClassIntensityDistributions;
  

  WeightsImagePointer                         m_WeightsImage;


  ProportionsContainerType                    m_ClassProportions;

  // this is the resampled atlas.
  PriorsImagePointer                          m_ClassPriorImage;  


  typename InputImageType::ConstPointer       m_InputImage;


  typename CorrectedImageType::Pointer       m_CorrectedImage;


  unsigned long                              m_MaximumNumberOfIterations;


  TransformPointer                           m_Transform;

  InterpolatorPointer                        m_Interpolator;

};

}

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExpectationMaximizationImageClassification.txx"
#endif


#endif

