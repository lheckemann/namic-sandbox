/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFastAnisotropicChamferDistanceImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/24 13:39:24 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFastAnisotropicChamferDistanceImageFilter_h
#define __itkFastAnisotropicChamferDistanceImageFilter_h

#include <itkImageToImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNarrowBand.h>

namespace itk
{

/** \class FastAnisotropicChamferDistanceImageFilter
*   \brief This class compute the signed (positive and negative) city-block distance in a narrow band
* 
*   \par OVERVIEW  
*   This filter computes a Signed AnisotropicChamfer Distance Map of the input image
*   specialy designed to work within the Level Set framework,
*   in the Narrow Band Reinitialization (generally applied after
*   IsoContourDistanceImageFilter ).
*   It can however be used for other purposes.
*
*   The input is assumed to contain voxels with values higher than
*   the Maximal Computed Distance, 
*   or values between -1 and 1 (or -min_spacing and min_spacing if UseImageSpacing
*   is On) for voxels close to the 0-isosurface from which we compute the distance.
*
*   This filter is N-dimensional.
*
* \ingroup ImageFeatureExtraction 
*
*/

template < class TInputImage, class TOutputImage >
class ITK_EXPORT FastAnisotropicChamferDistanceImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FastAnisotropicChamferDistanceImageFilter    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Method for creation through the object factory */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro( FastAnisotropicChamferDistanceImageFilter, ImageToImageFilter );
  
  /** Type for input image. */
  typedef   TInputImage       InputImageType;
  
  /** Type for input image. */
  typedef   TOutputImage      OutputImageType;
  
  /** Type for the region of the input image. */
  typedef typename InputImageType::RegionType   RegionType;
  
  /** Type for the region of the input image. */
  typedef typename InputImageType::PixelType   PixelType;
  
  /** Type for the index of the input image. */
  typedef typename RegionType::IndexType  IndexType;
  
  /** Type for the index of the input image. */
  typedef typename InputImageType::OffsetType  OffsetType;
 
  /** Type for the spacing of the input image. */
  typedef typename InputImageType::SpacingType  SpacingType;
 
  /** Type for the size of the input image. */
  typedef typename RegionType::SizeType   SizeType;
  
  /** The dimension of the input and output images. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      InputImageType::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      OutputImageType::ImageDimension);
  
  /** Pointer Type for input image. */
  typedef typename InputImageType::ConstPointer InputImagePointer;
  
  /** NarrowBand container */
  typedef BandNode<IndexType,PixelType> BandNodeType;
  typedef NarrowBand<BandNodeType> NarrowBandType;
  typedef typename NarrowBandType::Pointer NarrowBandPointer;
 
  /** Maximal computed distance */
  itkSetMacro( MaximumDistance, float);
  itkGetMacro( MaximumDistance, float);

  /** Toggle use image spacing in distance computation. */
  itkSetMacro( UseImageSpacing, bool);
  itkGetMacro( UseImageSpacing, bool);
  itkBooleanMacro( UseImageSpacing );

  /** */
  void SetRegionToProcess( const RegionType &r)
  {
    if ( m_RegionToProcess != r)
      {
      m_RegionToProcess = r;
      this->Modified();
      }      
  }
  RegionType GetRegionToProcess() const
  {
    return m_RegionToProcess;
  }
  
  void SetNarrowBand( NarrowBandType * ptr)
  {  
    if ( m_NarrowBand != ptr )
      {
      m_NarrowBand = ptr;
      this->Modified();
      }
  }
  
  NarrowBandPointer GetNarrowBand() const
  { return m_NarrowBand; }
  
#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(SameDimensionCheck,
    (Concept::SameDimension<ImageDimension, OutputImageDimension>));
  itkConceptMacro(SameTypeCheck,
    (Concept::SameType<PixelType, typename TOutputImage::PixelType>));
  itkConceptMacro(FloatConvertibleToPixelTypeCheck,
    (Concept::Convertible<float, PixelType>));
  itkConceptMacro(PixelTypeConvertibleToFloatCheck,
    (Concept::Convertible<PixelType, float>));
  itkConceptMacro(PixelTypeGreaterThanFloatCheck,
    (Concept::GreaterThanComparable<PixelType, float>));
  itkConceptMacro(PixelTypeLessThanFloatCheck,
    (Concept::LessThanComparable<PixelType, float>));
  itkConceptMacro(PixelTypeFloatAdditiveOperatorsCheck,
    (Concept::AdditiveOperators<PixelType, float, float>));
  itkConceptMacro(FloatGreaterThanPixelTypeCheck,
    (Concept::GreaterThanComparable<float, PixelType>));
  itkConceptMacro(FloatLessThanPixelTypeCheck,
    (Concept::LessThanComparable<float, PixelType>));
  /** End concept checking */
#endif

protected:
  FastAnisotropicChamferDistanceImageFilter();
  virtual ~FastAnisotropicChamferDistanceImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Compute a Signed AnisotropicChamfer Distance Map up to the specified maximal distance 
      in n dimensions */
  void GenerateDataND();  

  /** Compute a Signed AnisotropicChamfer Distance Map up to the specified maximal distance */
  void GenerateData();  
  
private:   
  FastAnisotropicChamferDistanceImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  float  m_MaximumDistance;
  
  /** coefficients of the AnisotropicChamfer distance for each kind of neighbor. */
  float  m_Weights[ImageDimension];
  
  NarrowBandPointer m_NarrowBand;
  
  /** Region in the image to process.  */
  typename InputImageType::RegionType m_RegionToProcess;
 
  bool m_UseImageSpacing; 
  
}; // end of FastAnisotropicChamferDistanceImageFilter class

} //end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFastAnisotropicChamferDistanceImageFilter.txx"
#endif

#endif
