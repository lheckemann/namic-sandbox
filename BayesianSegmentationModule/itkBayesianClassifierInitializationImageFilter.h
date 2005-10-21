/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBayesianClassifierInitializationImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/03/14 21:47:51 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBayesianClassifierInitializationImageFilter_h
#define __itkBayesianClassifierInitializationImageFilter_h

#include "itkVectorImage.h"
#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkScalarImageKmeansImageFilter.h"
#include "itkDensityFunction.h"
#include "itkMaximumDecisionRule.h"

namespace itk
{
  
/** \class BayesianClassifierInitializationImageFilter
 *
 * \brief This filter will perform Bayesian Classification on an image.
 *
 * [add detailed documentation]
 * 
 * This filter is templated over the input image type
 * and the output image type.
 * 
 * The filter expect both images to have the same number of dimensions.
 *
 * \author John Melonakos, Georgia Tech
 *
 * \ingroup IntensityImageFilters  Multithreaded
 */


template <class TInputImage, class TOutputImage>
class ITK_EXPORT BayesianClassifierInitializationImageFilter :
    public
ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef BayesianClassifierInitializationImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage
  >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BayesianClassifierInitializationImageFilter, ImageToImageFilter);

  /** Input and Output image types */
  typedef typename Superclass::InputImageType   InputImageType;
  typedef typename Superclass::OutputImageType  OutputImageType;
  
  /** Dimension of the input image */
  itkStaticConstMacro( Dimension, unsigned int, 
                       itk::GetImageDimension< InputImageType >::ImageDimension );

  /** Input and Output image iterators */
  typedef itk::ImageRegionConstIterator< InputImageType > InputImageIteratorType;
//  typedef itk::ImageRegionIterator< OutputImageType >     OutputImageIteratorType;

  /** Pixel types. */
  typedef typename TInputImage::PixelType  InputPixelType;
//  typedef typename TOutputImage::PixelType OutputPixelType;

  /** Image Type and Pixel type for the images representing the Prior
   * probability of a pixel belonging to  a particular class. This image has
   * arrays as pixels, the number of elements in the array is the same as the
   * number of classes to be used.  */
  typedef itk::VectorImage< double, Dimension >           PriorImageType;
  typedef typename PriorImageType::PixelType              PriorPixelType;
  typedef typename PriorImageType::Pointer                PriorImagePointer;
  typedef itk::ImageRegionIterator< PriorImageType >      PriorImageIteratorType;

  /** Image Type and Pixel type for the images representing the membership of a
   *  pixel to a particular class. This image has arrays as pixels, the number of 
   *  elements in the array is the same as the number of classes to be used.    */
  typedef VectorImage< double, Dimension >                MembershipImageType;
  typedef typename MembershipImageType::PixelType         MembershipPixelType;
  typedef typename MembershipImageType::Pointer           MembershipImagePointer;
  typedef itk::ImageRegionIterator< MembershipImageType > MembershipImageIteratorType;

  /** Types for the Kmeans Classification **/
  typedef itk::ScalarImageKmeansImageFilter< InputImageType > KMeansFilterType;
  typedef typename KMeansFilterType::OutputImageType          KMeansImageType;
  typedef itk::ImageRegionConstIterator< InputImageType >     ConstInputIteratorType;
  typedef itk::ImageRegionConstIterator< KMeansImageType >    ConstKMeansIteratorType;
  typedef itk::Array< double >                                CovarianceArrayType;

  /** Type of the Measurement */
  typedef itk::Vector< double, 1 >                        MeasurementVectorType;

  /** Type of the density functions */
  typedef Statistics::GaussianDensityFunction< MeasurementVectorType >
                                                          MembershipFunctionType;
  typedef typename MembershipFunctionType::Pointer        MembershipFunctionPointer;
  typedef itk::VectorContainer< unsigned short, MembershipFunctionType::MeanType* > 
                                                          MeanEstimatorsContainerType;
  typedef itk::VectorContainer< unsigned short, MembershipFunctionType::CovarianceType* > 
                                                          CovarianceEstimatorsContainerType;

  /** Membership function container */
  typedef std::vector< MembershipFunctionPointer >        MembershipFunctionContainer;

protected:
  BayesianClassifierInitializationImageFilter();
  virtual ~BayesianClassifierInitializationImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Here is where the prior and membership probability vector images are created.*/
  virtual void GenerateData();

private:
  BayesianClassifierInitializationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBayesianClassifierInitializationImageFilter.txx"
#endif

#endif
