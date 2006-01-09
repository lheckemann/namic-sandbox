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
   public ExpectationMaximizationMixtureModelEstimator< 
                    Sample< typename TImageType::PixelType > >
{

public:

  typedef ExpectationMaximizationImageClassification             Self;
  typedef ExpectationMaximizationMixtureModelEstimator< 
                  Sample< typename TImageType::PixelType > >     Superclass;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( ExpectationMaximizationImageClassification, ExpectationMaximizationMixtureModelEstimator );


public:

  typedef TImageType                                   InputImageType;

  itkStaticConstMacro( ImageDimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef VectorImage< float, ImageDimension >         MembershipImageType;

  typedef typename MembershipImageType::Pointer        MembershipImagePointer;

  typedef typename  InputImageType::PixelType          MeasurementVectorType;

  typedef Statistics::GaussianDensityFunction< 
                           MeasurementVectorType >     GaussianMembershipFunctionType;

  typedef typename GaussianMembershipFunctionType::Pointer  GaussianMembershipFunctionPointer;



protected:

  ExpectationMaximizationImageClassification();

  ~ExpectationMaximizationImageClassification();



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

