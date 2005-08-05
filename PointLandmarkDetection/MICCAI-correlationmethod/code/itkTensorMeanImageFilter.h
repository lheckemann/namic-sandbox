/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTensorMeanImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:51 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTensorMeanImageFilter_h
#define __itkTensorMeanImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkNumericTraits.h"
#include "itkSymmetricSecondRankTensor.h"

namespace itk
{
/** \class TensorMeanImageFilter
 * \brief Applies an averaging filter to an image
 *
 * Computes an image where a given pixel is the mean value of the
 * the pixels in a neighborhood about the corresponding input pixel.
 *
 * A mean filter is one of the family of linear filters.  
 *
 * \sa Image
 * \sa Neighborhood
 * \sa NeighborhoodOperator
 * \sa NeighborhoodIterator
 * 
 * \ingroup IntensityImageFilters
 */
template < class TValueType, unsigned int VLength, unsigned int VDimension >
class ITK_EXPORT TensorMeanImageFilter :
    public ImageToImageFilter< Image<SymmetricSecondRankTensor<TValueType, VLength>, VDimension>, Image<SymmetricSecondRankTensor<TValueType, VLength>, VDimension> >
{
public:
  /** Extract dimension from input and output image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      VDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      VDimension);

  /** Convenient typedefs for simplifying declarations. */
  typedef Image<SymmetricSecondRankTensor<TValueType, VLength>, VDimension> InputImageType;
  typedef Image<SymmetricSecondRankTensor<TValueType, VLength>, VDimension> OutputImageType;

  /** Standard class typedefs. */
  typedef TensorMeanImageFilter Self;
  typedef ImageToImageFilter< InputImageType, OutputImageType> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TensorMeanImageFilter, ImageToImageFilter);
  
  /** Image typedef support. */
  typedef typename InputImageType::PixelType InputPixelType;
  typedef typename OutputImageType::PixelType OutputPixelType;
  typedef typename NumericTraits<InputPixelType>::RealType InputRealType;
  typedef typename InputImageType::PixelType::ValueType InputValueType;
  typedef typename OutputImageType::PixelType::ValueType OutputValueType;
  typedef typename NumericTraits<TValueType>::RealType InputValueRealType;
  
  typedef typename InputImageType::RegionType InputImageRegionType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;

  typedef typename InputImageType::SizeType InputSizeType;

  /** Set the radius of the neighborhood used to compute the mean. */
  itkSetMacro(Radius, InputSizeType);

  /** Get the radius of the neighborhood used to compute the mean */
  itkGetConstReferenceMacro(Radius, InputSizeType);
  
  /** TensorMeanImageFilter needs a larger input requested region than
   * the output requested region.  As such, TensorMeanImageFilter needs
   * to provide an implementation for GenerateInputRequestedRegion()
   * in order to inform the pipeline execution model.
   *
   * \sa ImageToImageFilter::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion() throw(InvalidRequestedRegionError);

protected:
  TensorMeanImageFilter();
  virtual ~TensorMeanImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** TensorMeanImageFilter can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData()
   * routine which is called for each processing thread. The output
   * image data is allocated automatically by the superclass prior to
   * calling ThreadedGenerateData().  ThreadedGenerateData can only
   * write to the portion of the output image specified by the
   * parameter "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData() */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );

private:
  TensorMeanImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  InputSizeType m_Radius;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTensorMeanImageFilter.txx"
#endif

#endif
