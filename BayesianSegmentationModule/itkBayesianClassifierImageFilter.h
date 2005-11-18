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
#include "itkMaximumDecisionRule.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

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


template < class TInputVectorImage, class TLabelsType=unsigned char, 
           class TPosteriorsPrecisionType=double, class TPriorsPrecisionType=double >
class ITK_EXPORT BayesianClassifierImageFilter :
    public ImageToImageFilter< 
              TInputVectorImage, Image< TLabelsType, 
              ::itk::GetImageDimension< TInputVectorImage >::ImageDimension > >
{
public:
  /** Standard class typedefs. */
  typedef BayesianClassifierImageFilter                    Self;
  typedef ImageToImageFilter< 
              TInputVectorImage,
              Image< TLabelsType, 
                           ::itk::GetImageDimension< 
                                 TInputVectorImage >::ImageDimension > > Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( BayesianClassifierImageFilter, ImageToImageFilter );

  /** Input and Output image types */
  typedef typename Superclass::InputImageType        InputImageType;
  typedef typename Superclass::OutputImageType       OutputImageType;
  typedef typename InputImageType::ConstPointer      InputImagePointer;
  typedef typename OutputImageType::Pointer          OutputImagePointer;
  typedef typename InputImageType::RegionType        ImageRegionType;
  
  /** Dimension of the input image */
  itkStaticConstMacro( Dimension, unsigned int, 
                       ::itk::GetImageDimension< InputImageType >::ImageDimension );

  /** Input and Output image iterators */
  typedef ImageRegionConstIterator< InputImageType > InputImageIteratorType;
  typedef ImageRegionIterator< OutputImageType >     OutputImageIteratorType;

  /** Pixel types. */
  typedef typename InputImageType::PixelType              InputPixelType;
  typedef typename OutputImageType::PixelType             OutputPixelType;

  /** Image Type and Pixel type for the images representing the Prior
   * probability of a pixel belonging to  a particular class. This image has
   * arrays as pixels, the number of elements in the array is the same as the
   * number of classes to be used.  */
  typedef VectorImage< TPriorsPrecisionType, 
                             Dimension >                  PriorsImageType;
  typedef typename PriorsImageType::PixelType             PriorsPixelType;
  typedef typename PriorsImageType::Pointer               PriorsImagePointer;
  typedef ImageRegionConstIterator< PriorsImageType >     PriorsImageIteratorType;

  /** Image Type and Pixel type for the images representing the membership of a
   *  pixel to a particular class. This image has arrays as pixels, the number of 
   *  elements in the array is the same as the number of classes to be used.    */
  typedef TInputVectorImage                               MembershipImageType;
  typedef typename MembershipImageType::PixelType         MembershipPixelType;
  typedef typename MembershipImageType::Pointer           MembershipImagePointer;
  typedef ImageRegionConstIterator< MembershipImageType > MembershipImageIteratorType;

  /** Image Type and Pixel type for the images representing the Posterior
   * probability of a pixel belonging to  a particular class. This image has
   * arrays as pixels, the number of elements in the array is the same as the
   * number of classes to be used.  */
  typedef VectorImage< TPosteriorsPrecisionType, 
                             Dimension >                  PosteriorsImageType;
  typedef typename PosteriorsImageType::PixelType         PosteriorsPixelType;
  typedef typename PosteriorsImageType::Pointer           PosteriorsImagePointer;
  typedef ImageRegionIterator< PosteriorsImageType >      PosteriorsImageIteratorType;

  /** Decision rule to use for defining the label */
  typedef MaximumDecisionRule                             DecisionRuleType;
  typedef DecisionRuleType::Pointer                       DecisionRulePointer;

  /** An image from a single component of the Posterior */
  typedef itk::Image< TPosteriorsPrecisionType, Dimension > 
                                                          ExtractedComponentImageType;
  
  /** Optional Smoothing filter that will be applied to the Posteriors */
  typedef ImageToImageFilter< 
                   ExtractedComponentImageType, 
                   ExtractedComponentImageType  >         SmoothingFilterType;
  typedef typename SmoothingFilterType::Pointer           SmoothingFilterPointer;

  /** Set/ Get macros for the smoothing filter that may optionally be applied
   * to the posterior image */
  void SetSmoothingFilter( SmoothingFilterType * );
  itkGetMacro( SmoothingFilter, SmoothingFilterPointer );

  /** Number of iterations to apply the smoothing filter */
  itkSetMacro( NumberOfSmoothingIterations, unsigned int );
  itkGetMacro( NumberOfSmoothingIterations, unsigned int );
  
protected:

  BayesianClassifierImageFilter();
  virtual ~BayesianClassifierImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Here is where the classification is computed.*/
  virtual void GenerateData();

  /** Allocate Memory for the Output.*/
  virtual void AllocateOutputs();


  /** Methods for computing the labeled map for all combinations of conditions */
  virtual void ComputeBayesRule();
  virtual void NormalizeAndSmoothPosteriors();
  virtual void ClassifyBasedOnPosteriors();
  PosteriorsImageType *GetPosteriorImage();

private:

  BayesianClassifierImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Boolean flag indicating that the user defined the Priors optional input */
  bool m_UserProvidedPriors;

  /** Boolean flag indicating that the user provided a Smoothing filter */
  bool m_UserProvidedSmoothingFilter;

  /** Pointer to optional Smoothing filter */
  SmoothingFilterPointer  m_SmoothingFilter;

  /** Number of iterations to apply the smoothing filter */
  unsigned int m_NumberOfSmoothingIterations;

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBayesianClassifierImageFilter.txx"
#endif

#endif
