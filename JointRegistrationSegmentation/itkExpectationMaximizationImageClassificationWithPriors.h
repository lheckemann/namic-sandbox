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
#ifndef __itkExpectationMaximizationImageClassificationWithPriors_h
#define __itkExpectationMaximizationImageClassificationWithPriors_h

#include "itkExpectationMaximizationImageClassification.h"

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
 
template < class TObservationsZImageType, 
           class TParametersTheta = SimpleDataObjectDecorator< Array< double > >,
           class TPriorPixelComponentType=float >
class ExpectationMaximizationImageClassificationWithPriors : 
   public ExpectationMaximizationImageClassification< TObservationsZImageType,
                                                      TParametersTheta > 
{

public:

  typedef ExpectationMaximizationImageClassificationWithPriors   Self;

  typedef ExpectationMaximizationImageClassification< 
                                       TObservationsZImageType,
                                       TParametersTheta >        Superclass;
  
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( ExpectationMaximizationImageClassificationWithPriors, ExpectationMaximizationImageClassification );


public:

  typedef typename Superclass::InputImageType                  InputImageType;

  typedef typename Superclass::WeightsImageType                WeightsImageType;

  itkStaticConstMacro( ImageDimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension );


  typedef TPriorPixelComponentType                     PriorPixelComponentType;

  typedef VectorImage< PriorPixelComponentType, 
                       ImageDimension >                PriorsImageType;

  typedef typename PriorsImageType::PixelType          PriorsPixelType;

  typedef typename PriorsImageType::ConstPointer       PriorsImagePointer;

  typedef typename  InputImageType::PixelType          InputPixelType;
  typedef           InputPixelType                     MeasurementVectorType;

  typedef GaussianDensityFunction< MeasurementVectorType >  GaussianDensityFunctionType;

  typedef typename GaussianDensityFunctionType::ConstPointer  GaussianDensityFunctionPointer;

  typedef double   ProportionType;

 

  /**  Add a density function to the list of Density functions to use.  */
  void AddIntensityDistributionDensity( 
                  const GaussianDensityFunctionType * gaussian,
                  ProportionType proportion );


  /** Set the image of priors. This is equivalent to an atlas.    */
  /*  The method assumes that the first class prior defines       */
  /*  the background.                                             */ 
  void SetClassPriors( const PriorsImageType * image );


  /** Get the number of classes that are expected by the priors 
   */
  unsigned int GetNumberOfClasses() const;


protected:

  /** Constructor */
  ExpectationMaximizationImageClassificationWithPriors();

  /** Destructor */
  ~ExpectationMaximizationImageClassificationWithPriors();

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
  bool Converged() const;

   

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
  

  ProportionsContainerPointer                 m_ClassProportions;

  // this is the resampled atlas.
  PriorsImagePointer                          m_ClassPriorImage;  


private:

  ExpectationMaximizationImageClassificationWithPriors(const Self&) ; //purposely not implemented

  void operator=(const Self&) ; //purposely not implemented



};

}

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExpectationMaximizationImageClassificationWithPriors.txx"
#endif


#endif

