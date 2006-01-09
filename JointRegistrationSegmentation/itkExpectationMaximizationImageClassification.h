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

#include "itkGaussianMixtureModelComponent.h"
#include "itkSample.h"
#include "itkExpectationMaximizationMixtureModelEstimator.h"
#include "itkVectorImage.h"

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
 
template < class TImageType >
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

  typedef VectorImage< float, ImageDimension >         MembershipImageType;

  typedef typename MembershipImageType::Pointer        MembershipImagePointer;

  typedef typename  InputImageType::PixelType          MeasurementVectorType;

  typedef Sample< MeasurementVectorType >              SampleType;

  typedef GaussianDensityFunction< MeasurementVectorType >  GaussianDensityFunctionType;

  typedef typename GaussianDensityFunctionType::Pointer  GaussianDensityFunctionPointer;

 
  /**  Add a Gaussian density function to the list of Density functions to use.  */
  void AddGaussianComponent( const GaussianDensityFunctionType * gaussian );


  /** This method triggers the computation of the estimation */
  void Update();


protected:

  ExpectationMaximizationImageClassification();

  ~ExpectationMaximizationImageClassification();


  /** This method compute the update of parameters to be estimated based on the
    * weights and the list of samples (observations) */
  void ComputeMaximization();
 
  /** This method updates the weights (or posteriors) based on the current
   * parameters and the collection of samples (observations). */
  void ComputeExpectation();


  /** This method performs the actual computation of the classification */
  void GenerateData();

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

