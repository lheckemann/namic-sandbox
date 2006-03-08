/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFastChamferDistanceImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:30 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFastChamferDistanceImageFilter_h
#define __itkFastChamferDistanceImageFilter_h

#include <itkImageToImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNarrowBand.h>

namespace itk
{

/** \class FastChamferDistanceImageFilter
*   \brief This class compute the signed (positive and negative) chamfer distance in a narrow band
* 
*   \par OVERVIEW  
*   This filter computes a Signed Chamfer Distance Map of the input image
*   specialy designed to work within the Level Set framework,
*   in the Narrow Band Reinitialization (generally applied after
*   IsoContourDistanceImageFilter ).
*   It can however be used for other purposes.
*
*   The input is assumed to contain voxels with values higher than
*   the Maximal Computed Distance, 
*   or values between -1 and 1 for voxels close to the 0-isosurface
*   from which we compute the distance.
*
*   This filter is N-dimensional.
*
*   \par REFERENCES
*   Fast and Accurate Redistancing for Level Set Methods
*  `Krissian K. and Westin C.F.',
*   EUROCAST NeuroImaging Workshop Las Palmas Spain,
*   Ninth International Conference on Computer Aided Systems Theory , pages 48-51, Feb 2003.
*
* \ingroup ImageFeatureExtraction 
*
*/

template < class TInputImage, class TOutputImage >
class ITK_EXPORT FastChamferDistanceImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FastChamferDistanceImageFilter    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Method for creation through the object factory */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro( FastChamferDistanceImageFilter, ImageToImageFilter );
  
  /** Type for input image. */
  typedef   TInputImage       InputImageType;
  
  /** Type for the region of the input image. */
  typedef typename InputImageType::RegionType   RegionType;
  
  /** Type for the region of the input image. */
  typedef typename InputImageType::PixelType   PixelType;
  
  /** Type for the index of the input image. */
  typedef typename RegionType::IndexType  IndexType;
  
  /** Type for the index of the input image. */
  typedef typename InputImageType::OffsetType  OffsetType;
  
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
  
  
  /** coefficients of the Chamfer distance for each kind of neighbor. */
  itkSetVectorMacro( Weights, float, ImageDimension);
  itkGetVectorMacro( Weights, const float, ImageDimension);
  
  
  /** Maximal computed distance */
  itkSetMacro( MaximumDistance, float);
  itkGetMacro( MaximumDistance, float);

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
  /** End concept checking */
#endif

protected:
  FastChamferDistanceImageFilter();
  virtual ~FastChamferDistanceImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Compute a Signed Chamfer Distance Map up to the specified maximal distance 
      in n dimensions */
  void GenerateDataND();  
  
  /** Compute a Signed Chamfer Distance Map up to the specified maximal distance */
  void GenerateData();  
  
private:   
  FastChamferDistanceImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  float  m_MaximumDistance;
  
  /** coefficients of the Chamfer distance for each kind of neighbor. */
  float  m_Weights[ImageDimension];
  
  NarrowBandPointer m_NarrowBand;
  
  /** Region in the image to process.  */
  typename InputImageType::RegionType m_RegionToProcess;
  
  
}; // end of FastChamferDistanceImageFilter class

} //end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFastChamferDistanceImageFilter.txx"
#endif

#endif
