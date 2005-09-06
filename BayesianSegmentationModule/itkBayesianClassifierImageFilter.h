/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBayesianClassifierImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/03/14 21:47:51 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBayesianClassifierImageFilter_h
#define __itkBayesianClassifierImageFilter_h

#include "itkVectorImage.h"
#include "itkImageToImageFilter.h"
#include "itkMaximumRatioDecisionRule.h"
#include "itkDensityFunction.h"

namespace itk
{
  
/** \class BayesianClassifierImageFilter
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
class ITK_EXPORT BayesianClassifierImageFilter :
    public
ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef BayesianClassifierImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage
  >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BayesianClassifierImageFilter, ImageToImageFilter);

  /** Image Type and Pixel type for the images representing the membership of a
   *  pixel to a particular class. This image has arrays as pixels, the number of 
   *  elements in the array is the same as the number of classes to be used.    */
  typedef itk::VectorImage< double, Dimension >      MembershipImageType;
  typedef typename MembershipImageType::PixelType    MembershipPixelType;
  typedef typename MembershipImageType::Pointer      MembershipImagePointer;

  /** Image Type and Pixel type for the images representing the Prior
   * probability of a pixel belonging to  a particular class. This image has
   * arrays as pixels, the number of elements in the array is the same as the
   * number of classes to be used.  */
  typedef itk::VectorImage< double, Dimension >      PriorImageType;
  typedef typename PriorImageType::PixelType         PriorPixelType;
  typedef typename PriorImageType::Pointer           PriorImagePointer;

  /** Image Type and Pixel type for the images representing the Posterior
   * probability of a pixel belonging to  a particular class. This image has
   * arrays as pixels, the number of elements in the array is the same as the
   * number of classes to be used.  */
  typedef itk::VectorImage< double, Dimension >      PosteriorImageType;
  typedef typename PosteriorImageType::PixelType     PosteriorPixelType;
  typedef typename PosteriorImageType::Pointer       PosteriorImagePointer;

  typedef itk::ImageRegionIterator< MembershipImageType > MembershipImageIteratorType;

  /** Pixel types. */
  typedef typename TInputImage::PixelType  InputPixelType;
  typedef typename TOutputImage::PixelType OutputPixelType;
  
  /** Input and Output image types */
  typedef typename Superclass::InputImageType   InputImageType;
  typedef typename Superclass::OutputImageType  OutputImageType;

  /** Type of the Measurement */
  typedef InputPixelType   MeasurementVectorType;

  /** Type of the density functions */
  typedef Statistics::DensityFunction< MeasurementVectorType 
                                              > MembershipFunctionType;
  typedef typename MembershipFunctionType::ConstPointer       
                                                MembershipFunctionConstPointer;

  /** Membership function container */
  typedef std::vector< MembershipFunctionConstPointer >       MembershipFunctionContainer;

  /** Decision rule to use for defining the label */
  typedef MaximumRatioDecisionRule                            DecisionRuleType;

  
  /** Add a membership function to the filter. */
  void AddMembershipFunction( const MembershipFunctionType * newFunction );


protected:
  BayesianClassifierImageFilter();
  virtual ~BayesianClassifierImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Here is where the classification is computed.*/
  virtual void GenerateData();

private:
  BayesianClassifierImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MembershipFunctionContainer     m_MembershipFunctions;  
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBayesianClassifierImageFilter.txx"
#endif

#endif
