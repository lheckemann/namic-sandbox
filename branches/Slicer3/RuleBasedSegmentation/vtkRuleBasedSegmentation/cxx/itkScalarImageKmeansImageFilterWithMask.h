/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageKmeansImageFilterWithMask.h,v $
  Language:  C++
  Date:      $Date: 2006/04/05 13:59:37 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkScalarImageKmeansImageFilterWithMask_h
#define __itkScalarImageKmeansImageFilterWithMask_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkNumericTraits.h"

#include "itkKdTree.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkWeightedCentroidKdTreeGenerator.h"

#include "itkMinimumDecisionRule.h"
#include "itkEuclideanDistance.h"
#include "itkSampleClassifier.h"

#include "itkImageToListGenerator.h"
#include "itkImageRegion.h"
#include "itkRegionOfInterestImageFilter.h"

#include <vector>

namespace itk
{
/** \class ScalarImageKmeansImageFilterWithMask
 * \brief Classifies the intensity values of a scalar image using the K-Means algorithm.
 *
 * Given an input image with scalar values, it uses the K-Means statistical
 * classifier in order to define labels for every pixel in the image. The
 * filter is templated over the type of the input image. The output image is
 * predefined as having the same dimension of the input image and pixel type
 * unsigned char, under the assumption that the classifier will generate less
 * than 256 classes. 
 *
 * You may want to look also at the RelabelImageFilter that may be used as a
 * postprocessing stage, in particular if you are interested in ordering the
 * labels by their relative size in number of pixels.
 *
 * \sa Image
 * \sa ImageKmeansModelEstimator
 * \sa KdTreeBasedKmeansEstimator, WeightedCentroidKdTreeGenerator, KdTree
 * \sa RelabelImageFilter
 * 
 * \ingroup ClassificationFilters 
 */
template <class TInputImage,
          class TMaskImage=TInputImage >
class ITK_EXPORT ScalarImageKmeansImageFilterWithMask :
    public ImageToImageFilter< TInputImage, Image<unsigned char,
                                                  ::itk::GetImageDimension<TInputImage>::ImageDimension> >
{
public:
  /** Extract dimension from input and output image. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Convenient typedefs for simplifying declarations. */
  typedef TInputImage InputImageType;
  typedef Image<unsigned char,
             ::itk::GetImageDimension<TInputImage>::ImageDimension> OutputImageType;

  /** Standard class typedefs. */
  typedef ScalarImageKmeansImageFilterWithMask Self;
  typedef ImageToImageFilter< InputImageType, OutputImageType> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ScalarImageKmeansImageFilterWithMask, ImageToImageFilter);
  
  /** Image typedef support. */
  typedef typename InputImageType::PixelType InputPixelType;
  typedef typename OutputImageType::PixelType OutputPixelType;

  /** Type used for representing the Mean values */
  typedef typename NumericTraits< InputPixelType >::RealType RealPixelType;
  
  /** Mask Image typedefs */
  typedef TMaskImage                           MaskImageType;
  typedef typename MaskImageType::PixelType    MaskPixelType;

  /** Get/Set MaskImage **/
  itkSetObjectMacro( MaskImage, MaskImageType );
  itkGetConstObjectMacro( MaskImage, MaskImageType );

  /** Get/Set MaskValue **/
  itkSetMacro( MaskValue, MaskPixelType );
  itkGetMacro( MaskValue, MaskPixelType );
  
  /** Create a List from the scalar image */
  typedef itk::Statistics::ImageToListGenerator< InputImageType, MaskImageType > ImageToListGeneratorType;
 
  /** Define the Measurement vector type from the AdaptorType */
  typedef typename ImageToListGeneratorType::MeasurementVectorType  MeasurementVectorType;

  /** Create the K-d tree structure */
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator< 
    typename ImageToListGeneratorType::ListSampleType > TreeGeneratorType;
  typedef typename TreeGeneratorType::KdTreeType TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;

  typedef typename EstimatorType::ParametersType ParametersType;

  typedef typename InputImageType::RegionType        ImageRegionType;
  
  typedef RegionOfInterestImageFilter< 
                                 InputImageType,
                                 InputImageType  > RegionOfInterestFilterType;

  /** Add a new class to the classification by specifying its initial mean. */
  void AddClassWithInitialMean( RealPixelType mean );

  /** Return the array of Means found after the classification */
  itkGetConstReferenceMacro( FinalMeans, ParametersType );

  /** Set/Get the UseNonContiguousLabels flag. When this is set to false the
   * labels are numbered contiguously, like in {0,1,3..N}. When the flag is set
   * to true, the labels are selected in order to span the dynamic range of the
   * output image. This last option is useful when the output image is intended
   * only for display. The default value is false. */
  itkSetMacro( UseNonContiguousLabels, bool );
  itkGetConstReferenceMacro( UseNonContiguousLabels, bool );
  itkBooleanMacro( UseNonContiguousLabels );

  /** Set Region method to constrain classfication to a certain region */
  void SetImageRegion( const ImageRegionType & region );
  
  /** Get the region over which the statistics will be computed */
  itkGetConstReferenceMacro( ImageRegion, ImageRegionType );
  
#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputHasNumericTraitsCheck,
                  (Concept::HasNumericTraits<InputPixelType>));
  /** End concept checking */
#endif

protected:
  ScalarImageKmeansImageFilterWithMask();
  virtual ~ScalarImageKmeansImageFilterWithMask() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** This method runs the statistical methods that identify the means of the
   * classes and the use the distances to those means in order to label the
   * image pixels.   *
   * \sa ImageToImageFilter::GenerateData() 
   **/
  void GenerateData();

private:
  ScalarImageKmeansImageFilterWithMask(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typedef std::vector< RealPixelType > MeansContainer;

  MeansContainer  m_InitialMeans;

  ParametersType  m_FinalMeans;

  bool m_UseNonContiguousLabels;

  ImageRegionType m_ImageRegion;

  bool m_ImageRegionDefined;
  
  typename MaskImageType::Pointer m_MaskImage;

  MaskPixelType m_MaskValue;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkScalarImageKmeansImageFilterWithMask.txx"
#endif

#endif