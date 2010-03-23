/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVoronoiPartitioningImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:40 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _itkVoronoiPartitioningImageFilter_h
#define _itkVoronoiPartitioningImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkVoronoiSegmentationImageFilterBase.h"
#include "itkImage.h"

namespace itk
{

/** \class VoronoiPartitioningImageFilter
 * 
 * Perform a partitioning of 2D images (single channel) by Voronoi Diagram.
 *
 * \ingroup HybridSegmentation
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT VoronoiPartitioningImageFilter:
    public VoronoiSegmentationImageFilterBase<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef VoronoiPartitioningImageFilter       Self;
  typedef VoronoiSegmentationImageFilterBase<TInputImage,TOutputImage> 
  Superclass;
  typedef SmartPointer <Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VoronoiPartitioningImageFilter,
               VoronoiSegmentationImageFilterBase);

  /** Convenient typedefs. */
  typedef typename Superclass::BinaryObjectImage BinaryObjectImage;
  typedef typename Superclass::IndexList IndexList;
  typedef typename Superclass::IndexType IndexType;
  typedef typename Superclass::RegionType RegionType;
  typedef typename Superclass::InputImageType InputImageType;
  typedef typename Superclass::OutputImageType OutputImageType;
  typedef typename Superclass::OutputPixelType OutputPixelType;
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointTypeDeque PointTypeDeque;
  typedef typename Superclass::PointIdIterator PointIdIterator;
  typedef typename Superclass::CellAutoPointer CellAutoPointer;
  typedef typename Superclass::EdgeIterator EdgeIterator;
  typedef typename Superclass::NeighborIdIterator NeighborIdIterator;

  /** Create the output results.  */
  virtual void MakeSegmentBoundary(void);
  virtual void MakeSegmentObject(void);
  
  /** Set/Get the threshold used to determine if a Voronoi region is
   * homogeneous. If the standard deviation of the intensities in the
   * Voronoi region is below this threshold, then the region is
   * considered homogeneous. */
  itkSetMacro(SigmaThreshold, double);
  itkGetMacro(SigmaThreshold, double);

  /** ImageDimension enumeration   */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension );

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(SameDimensionCheck,
    (Concept::SameDimension<InputImageDimension, OutputImageDimension>));
  itkConceptMacro(IntConvertibleToOutputCheck,
    (Concept::Convertible<int, OutputPixelType>));
  /** End concept checking */
#endif

protected:
  VoronoiPartitioningImageFilter();
  ~VoronoiPartitioningImageFilter();
  void PrintSelf(std::ostream &os, Indent indent) const;

  // Classify all the voronoi cells as interior , exterior or boundary.
  virtual void ClassifyDiagram(void);
  
  // Generate the seeds to be added by dividing the boundary cells.
  virtual void GenerateAddingSeeds(void);

  // Are the pixels specified in the index list homogeneous?
  virtual bool TestHomogeneity(IndexList &Plist);

  // Threshold for homogeneity criterion
  double m_SigmaThreshold;

private:
  VoronoiPartitioningImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

}//end namespace


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVoronoiPartitioningImageFilter.txx"
#endif

#endif



