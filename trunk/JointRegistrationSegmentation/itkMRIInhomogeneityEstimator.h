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
#include "itkImageToImageFilter.h"

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
 *
 * This class would fit in an EM framework, where the goal would be to 
 * 1. E - Estimate the weights given an estimated bias field
 * 2. M - Estimate the bias field given estimates of the weights.. (this class would
 * do step 2).
 *
 * From http://people.csail.mit.edu/welg/papers/adaptive1996.pdf
 * 
 * Equation (16): 
 *  Bias field estimate = H . R
 * 
 * H = is a linear low pass filter.
 * R = mean residual
 * 
 */ 
 
template < class TInputImage, class TInhomogeneityPrecisionType = float >
class MRIInhomogeneityEstimator : 
  public ImageToImageFilter< TInputImage, TInputImage >
{
public:
  typedef MRIInhomogeneityEstimator                   Self;
  typedef ImageToImageFilter< TInputImage, 
                              TInputImage >           Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  itkNewMacro( Self );
  itkTypeMacro( MRIInhomogeneityEstimator, ImageToImageFilter );
  
  typedef TInputImage                                  InputImageType;
  typedef InputImageType                               OutputImageType;

  itkStaticConstMacro( ImageDimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef VectorImage< TInhomogeneityPrecisionType, ImageDimension >         MembershipImageType;

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
 
  typedef typename GaussianMembershipFunctionType::CovarianceType CovarianceMatrixType;

  /** The weights image is an image of floats of the same dimension and size 
   * and number of components as the input image. This is the image estimate
   * obtained from the E step */
  virtual void SetMembershipImage( const MembershipImageType * );
  const MembershipImageType * GetMembershipImage() const;

  /** Method to set/get the density functions. Here you can set a vector 
   * container of density functions. If no density functions are specified,
   * the filter will create ones for you. These default density functions
   * are gaussian density functions centered around the K-means of the 
   * input image.  */
  virtual void SetIntensityDistributions( StructureIntensityDistributionContainerType
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

  virtual void AllocateImageOfInverseCovariances();

protected:
  MRIInhomogeneityEstimator();
  ~MRIInhomogeneityEstimator();

private:
  MRIInhomogeneityEstimator(const Self&) ; //purposely not implemented
  void operator=(const Self&) ; //purposely not implemented

  StructureIntensityDistributionContainerPointer 
                   m_StructureIntensityDistributionContainer;
  unsigned int m_NumberOfClasses;

  typedef itk::Image< CovarianceMatrixType, 
                      ImageDimension > InverseCovarianceImageType;
                                                           
  typename InverseCovarianceImageType::Pointer  m_InverseCovarianceImage;

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMRIInhomogeneityEstimator.txx"
#endif

#endif

