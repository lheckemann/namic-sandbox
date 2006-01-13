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
#include "itkVectorContainer.h"
#include "itkProcessObject.h"


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
 
  //
  // TParametersTheta ==> struct {will be the inhomogeneity correction + transform parameters for registration}.
  //
  // TUnobserveredVariablesYPosteriorImageType ===>> WeightsImage
  //
  //
template < class TObservationsZImageType, 
           class TPriorPixelComponentType >
class ExpectationMaximizationImageClassification : 
   public ProcessObject
{

public:

  typedef ExpectationMaximizationImageClassification             Self;
  typedef ProcessObject                                          Superclass;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( ExpectationMaximizationImageClassification, ProcessObject );


public:

  typedef TObservationsZImageType                      InputImageType;

  itkStaticConstMacro( ImageDimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef VectorImage< float, ImageDimension >         WeightsImageType;

  typedef TPriorPixelComponentType                     PriorPixelComponentType;

  typedef VectorImage< PriorPixelComponentType, 
                       ImageDimension >                PriorsImageType;

  typedef typename PriorsImageType::PixelType          PriorsPixelType;

  typedef typename WeightsImageType::Pointer           WeightsImagePointer;
  typedef typename PriorsImageType::Pointer            PriorsImagePointer;

  typedef typename  InputImageType::PixelType          InputPixelType;
  typedef           InputPixelType                     MeasurementVectorType;

  typedef GaussianDensityFunction< MeasurementVectorType >  GaussianDensityFunctionType;

  typedef typename GaussianDensityFunctionType::ConstPointer  GaussianDensityFunctionPointer;

  typedef double   ProportionType;

 

  /**  Add a density function to the list of Density functions to use.  */
  void AddIntensityDistributionDensity( 
                  const GaussianDensityFunctionType * gaussian,
                  ProportionType proportion );



  /** Set the input image to be classified. This image may be a vector image
   * with multiple-components */
  void SetInput( const InputImageType * image );


  /** Set the image of priors. This is equivalent to an atlas.    */
  /*  The method assumes that the first class prior defines       */
  /*  the background.                                             */ 

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

  /** Compute the label map resulting from the Weights */
  void ComputeLabelMap();

  /** Initialize the allocation of the Weights image Initialize the corrected
   *  image and the weights. Throws exceptions is something goes wrong */
  void Initialize();

  /** This method compute the update of parameters to be estimated based on the
   *  weights and the list of samples (observations) */
  void ComputeMaximization();
 
  /** This method updates the weights (or posteriors) based on the current
   *  parameters and the collection of samples (observations). */
  void ComputeExpectation();

   /** This method evaluates whether the maximization has converged or not.
   *  Along with the number of iterations it defines the stopping criteria
   *  for the entire Expectation/Maximization method. */
  bool Converge() const;

   
private:

  ExpectationMaximizationImageClassification(const Self&) ; //purposely not implemented

  void operator=(const Self&) ; //purposely not implemented



  /** Container that holds the list of Gaussian distributions for each one of the Structures. */
  typedef VectorContainer< unsigned int, 
                           GaussianDensityFunctionPointer 
                                        >     IntensityDistributionContainerType;

  typedef typename IntensityDistributionContainerType::Pointer 
                                              IntensityDistributionContainerPointer;

  typedef VectorContainer< unsigned int,
                           ProportionType >   ProportionsContainerType;

  typedef typename ProportionsContainerType::Pointer  ProportionsContainerPointer;


  IntensityDistributionContainerPointer       m_ClassIntensityDistributions;
  

  WeightsImagePointer                         m_WeightsImage;


  ProportionsContainerPointer                 m_ClassProportions;

  // this is the resampled atlas.
  PriorsImagePointer                          m_ClassPriorImage;  


  typename InputImageType::ConstPointer       m_InputImage;

  
  unsigned long                               m_MaximumNumberOfIterations;


};

}

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExpectationMaximizationImageClassification.txx"
#endif


#endif

