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
#include "itkVectorImage.h"
#include "itkVectorContainer.h"

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
class MRIInhomogeneityEstimator : public Object
{

public:


  typedef TInputImage                                  InputImageType;

  typedef InputImageType                               OutputImageType;

  itkStaticConstMacro( ImageDimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef VectorImage< float, ImageDimension >         MembershipImageType;

  typedef typename MembershipImageType::Pointer        MembershipImagePointer;

  typedef typename  InputImageType::PixelType          MeasurementVectorType;

  typedef Statistics::GaussianDensityFunction< 
                           MeasurementVectorType >     GaussianMembershipFunctionType;

  typedef typename GaussianMembershipFunctionType::Pointer  GaussianMembershipFunctionPointer;

  typedef VectorContainer< 
             unsigned int, 
             GaussianMembershipFunctionPointer >      StructureIntensityDistributionType;


public:

  typedef MRIInhomogeneityEstimator                   Self;
  typedef Object                                      Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( MRIInhomogeneityEstimator, Object );


protected:

  MRIInhomogeneityEstimator();

  ~MRIInhomogeneityEstimator();



private:

  MRIInhomogeneityEstimator(const Self&) ; //purposely not implemented

  void operator=(const Self&) ; //purposely not implemented


};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMRIInhomogeneityEstimator.txx"
#endif



#endif

