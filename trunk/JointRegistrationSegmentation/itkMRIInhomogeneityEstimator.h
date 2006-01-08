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
#ifndef __itkMRIInhomogeneityEstimator_h
#define __itkMRIInhomogeneityEstimator_h

#include "itkGaussianDensityFunction.h"

namespace itk {

/** \brief Filter for computing the inhomogeneity field of a multicomponent MRI image.
 *
 *  \class MRIInhomogeneityEstimator this class computes the inhomogeneity field of 
 *    a multicomponents MRI image. An inhomogenity field is computed per channel of 
 *    the multicomponent image but by taking into account interactions between all
 *    the channels.
 *
 *    This class is templated over the multicomponent MRI input image. That is expected
 *    to be defined as an itk::VectorImage.
 *
 *    This class involves a Gaussian classfication of the structures, and therefore uses
 *    a collection of GaussianDensityFunctions. 
 *
 */ 
 
template < class TInputImage >
class itk::MRIInhomogeneityEstimator< TInputImage > : public Object
{

public:


  typedef TInputImage                                  InputImageType;

  typedef InputImageType                               OutputImageType;

  itkStaticConstMacro( ImageDimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension )

  typedef VectorImage< floats, ImageDimension >        MembershipImageType;

  typedef typename MembershipImageType::Pointer        MembershipImagePointer;

  typedef typename  InputImageType::PixelType          MeasurementVectorType;

  typedef Statistics::GaussianDensityFunction< 
                           MeasurementVectorType >     GaussianMembershipFunctionType;

  typedef typename GaussianMembershipFunctionType::Pointer  GaussianMembershipFunctionPointer;

  typedef VectorContainer< 
                   GaussianMembershipFunctionPointer >      StructureIntensityDistributionType;

};

}

#endif

