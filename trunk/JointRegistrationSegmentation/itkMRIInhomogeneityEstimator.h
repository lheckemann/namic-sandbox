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
 * This class is templated over the multicomponent MRI input image. That is expected
 * to be defined as an itk::VectorImage.
 *
 * This class involves a Gaussian classfication of the structures, and therefore uses
 * a collection of GaussianDensityFunctions. The model assumes that in a bias-free 
 * MRI image, the statistical distribution of each tissue type can be represented as
 * a gaussian. Given this assumption, an EM algorithm is used to estimate the bias
 * field, the tissue class and noise.
 */ 
 
template < class TInputImage >
class MRIInhomogeneityEstimator : public ImageToImageFilter 
{
public:
  typedef MRIInhomogeneityEstimator                   Self;
  typedef ImageToImageFilter                          Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  itkNewMacro( Self );
  itkTypeMacro( MRIInhomogeneityEstimator, ImageToImageFilter );
  
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
             GaussianMembershipFunctionPointer >      StructureIntensityDistributionContainerType;
  typedef typename StructureIntensityDistributionContainerType::Pointer         
                                    StructureIntensityDistributionContainerPointer;
 
  /** Method to set/get the density functions. Here you can set a vector 
   * container of density functions. If no density functions are specified,
   * the filter will create ones for you. These default density functions
   * are gaussian density functions centered around the K-means of the 
   * input image.  */
  virtual void SetIntensityDistributions( StructureIntensityDistributionType 
                                                * densityFunctionContainer );
  itkGetObjectMacro( StructureIntensityDistributionContainer, 
                StructureIntensityDistributionContainerPointer );

  /** Get methods for the number of classes. The number of classes is the same
   * as the number of components in the VectorImage or the number of gaussian
   * distributions specified. (They must be equal, of course) */
  itkGetMacro( NumberOfClasses, unsigned int );

  /** Initialize the gaussian functions. This will be called only if the membership 
   * function hasn't already been set. This method initializes intensity distribution functions
   * using gaussian density functions centered around the means computed using 
   * Kmeans.
   */
  virtual void InitializeStructureIntensityDistributions();

  virtual void GenerateData();

protected:
  MRIInhomogeneityEstimator();
  ~MRIInhomogeneityEstimator();

private:
  MRIInhomogeneityEstimator(const Self&) ; //purposely not implemented
  void operator=(const Self&) ; //purposely not implemented

  typename StructureIntensityDistributionContainerPointer 
                   m_StructureIntensityDistributionContainer;
  unsigned int m_NumberOfClasses;

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMRIInhomogeneityEstimator.txx"
#endif

#endif

