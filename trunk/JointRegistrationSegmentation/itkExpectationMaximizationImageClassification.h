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
#ifndef __itkExpectationMaximizationImageClassification_h
#define __itkExpectationMaximizationImageClassification_h

#include "itkGaussianDensityFunction.h"
#include "itkVectorImage.h"
#include "itkVectorContainer.h"
#include "itkExpectationMaximizationMethod.h"
#include "itkSimpleDataObjectDecorator.h"


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
template < class TObservationsZImageType >
class ExpectationMaximizationImageClassification : 
   public ExpectationMaximizationMethod< 
              TObservationsZImageType, 
              VectorImage< float, 
                           GetImageDimension< TObservationsZImageType >::ImageDimension >, 
              SimpleDataObjectDecorator< Array< double > > >
{

public:

  typedef ExpectationMaximizationImageClassification             Self;

  typedef ExpectationMaximizationMethod< 
              TObservationsZImageType, 
              VectorImage< float, 
                           GetImageDimension< TObservationsZImageType >::ImageDimension >, 
              SimpleDataObjectDecorator< Array< double > >
                                                            >    Superclass;
  
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( ExpectationMaximizationImageClassification, ProcessObject );


public:

  typedef TObservationsZImageType                       InputImageType;

  typedef typename Superclass::UnobservedVariablesPosteriorType  
                                                        WeightsImageType;

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

  /**
   * Perform the E-Step. The E-Step computes the posterior
   * probability of the unobserved variables using the observations
   * and the current estimated parameters.
   *
   * This will modify output 1.
   */
  virtual void ComputeExpectation();
  
  /**
   * Perform the M-Step. The M-Step computes the maximum likelhood
   * estimate of the parameters using the observations and the
   * estimated unobserved variables.
   *
   * This will modify output 0.
   */
  virtual void ComputeMaximization();

  /**
   * Convergence criteria
   */
  virtual bool Converged() const;
 
   
private:

  ExpectationMaximizationImageClassification(const Self&) ; //purposely not implemented

  void operator=(const Self&) ; //purposely not implemented

  
};

}

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExpectationMaximizationImageClassification.txx"
#endif


#endif

