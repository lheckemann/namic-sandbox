#ifndef __itkFastSweepingImageFilter_h
#define __itkFastSweepingImageFilter_h

#include <itkImageToImageFilter.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>

namespace itk
{

template <class TInputImage,class TOutputImage>
class ITK_EXPORT FastSweepingImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FastSweepingImageFilter    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( FastSweepingImageFilter, ImageToImageFilter );

  /** Type for input image. */
  typedef   TInputImage       InputImageType;
  typedef itk::ImageRegionConstIteratorWithIndex< 
            InputImageType > InputIteratorType;

  /** Type for the arrival times */
  typedef   TOutputImage      OutputImageType;
  typedef itk::ImageRegionIteratorWithIndex< 
            OutputImageType > OutputIteratorType;

  /** Type for the region of the input image. */
  typedef typename InputImageType::RegionType   RegionType;

  /** Type for the index of the input image. */
  typedef typename RegionType::IndexType  IndexType;

  /** Type for the size of the input image. */
  typedef typename RegionType::SizeType   SizeType;
  
  /** The dimension of the input and output images. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Pointer Type for the arrival vectors */
  typedef Image< float,
                 itkGetStaticConstMacro(InputImageDimension)> VectorImageType;
  typedef itk::ImageRegionIteratorWithIndex< 
            VectorImageType > VectorIteratorType;

  /** Pointer Type for input image. */
  typedef typename InputImageType::ConstPointer InputImagePointer;

  /** Pointer Type for the output image. */
  typedef typename OutputImageType::Pointer OutputImagePointer;

  /** Pointer Type for the vector distance image. */
  typedef typename VectorImageType::Pointer VectorImagePointer;

  /** Get arrival times */
  OutputImageType * GetArrivalTimes(void);

  /** Get arrival vectors */
  VectorImageType * GetArrivalVectors(void);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(SameDimensionCheck,
    (Concept::SameDimension<InputImageDimension, OutputImageDimension>));
  itkConceptMacro(UnsignedIntConvertibleToOutputCheck,
    (Concept::Convertible<unsigned int, typename TOutputImage::PixelType>));
  itkConceptMacro(IntConvertibleToOutputCheck,
    (Concept::Convertible<int, typename TOutputImage::PixelType>));
  itkConceptMacro(DoubleConvertibleToOutputCheck,
    (Concept::Convertible<double, typename TOutputImage::PixelType>));
  itkConceptMacro(InputConvertibleToOutputCheck,
    (Concept::Convertible<typename TInputImage::PixelType,
                          typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  FastSweepingImageFilter();
  virtual ~FastSweepingImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Compute Danielsson distance map and Voronoi Map. */
  void GenerateData();  

  /** Prepare data. */
  void PrepareData();  

private:   
  FastSweepingImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of FastSweepingImageFilter class

} //end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFastSweepingImageFilter.txx"
#endif

#endif
