#ifndef __itkFinslerTractographyImageFilter_h
#define __itkFinslerTractographyImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageDirectionalConstIteratorWithIndex.h"
#include "itkImageDirectionalIteratorWithIndex.h"
#include "itkVector.h"
#include "itkVectorContainer.h"

namespace itk
{

template <class TInputImage,class TOutputImage>
class ITK_EXPORT FinslerTractographyImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FinslerTractographyImageFilter    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( FinslerTractographyImageFilter, ImageToImageFilter );

  /** Type for input image. */
  typedef   TInputImage       InputImageType;
  typedef itk::ImageRegionConstIteratorWithIndex< 
            InputImageType >  InputIteratorType;
  typedef typename InputImageType::ConstPointer InputImagePointer;
  typedef typename InputImageType::PixelType  InputPixelType;
  typedef std::vector< InputPixelType >  InputPixelArrayType;

  /** Type for the arrival times */
  typedef   TOutputImage      OutputImageType;
  typedef itk::ImageRegionIteratorWithIndex< 
            OutputImageType > OutputIteratorType;
  typedef typename OutputImageType::Pointer OutputImagePointer;

  /** The dimension of the input and output images. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension);

  /** Pointer Type for the arrival vectors */
  typedef Image< itk::Vector< float, Dimension >,
                 itkGetStaticConstMacro(InputImageDimension)> VectorImageType;
  typedef itk::ImageRegionIteratorWithIndex< 
            VectorImageType > VectorIteratorType;
  typedef typename VectorImageType::Pointer VectorImagePointer;

  /** Set/Get methods for convergence parameter. */
  itkSetMacro( ConvergenceParameter, typename OutputImageType::PixelType );
  itkGetMacro( ConvergenceParameter, typename OutputImageType::PixelType );

  /** Set/Get methods for gradient diffusion vectors. */
  typedef vnl_vector_fixed< double, InputImageDimension >
    GradientDiffusionVectorType;

  /** Container to hold gradient directions of the 'n' DW measurements */
  typedef itk::VectorContainer< unsigned int,
               GradientDiffusionVectorType > GradientDiffusionVectorsContainerType;
  void SetGradientDiffusionVectors( GradientDiffusionVectorsContainerType * );

  /** Get arrival times */
  OutputImageType * GetArrivalTimes(void);

  /** Get arrival vectors */
  VectorImageType * GetArrivalVectors(void);

  /** Directional iterator */
  typedef itk::ImageDirectionalConstIteratorWithIndex< 
            InputImageType > InputDirConstIteratorType;
  typedef typename InputDirConstIteratorType::Iterator InputNeighborPixel;
  typedef itk::ImageDirectionalIteratorWithIndex< 
            OutputImageType > OutputDirIteratorType;
  typedef typename OutputDirIteratorType::Iterator  OutputNeighborPixel;
  typedef itk::ImageDirectionalIteratorWithIndex< 
            VectorImageType > VectorDirIteratorType;
  typedef typename VectorDirIteratorType::Iterator  VectorNeighborPixel;

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
  FinslerTractographyImageFilter();
  virtual ~FinslerTractographyImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Compute Arrival Times and Arrival Vectors */
  void GenerateData();  

  /** Prepare data. */
  void PrepareData();  

private:   
  FinslerTractographyImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename OutputImageType::PixelType    m_ConvergenceParameter;
  typename GradientDiffusionVectorsContainerType::Pointer
    m_GradientDiffusionVectorsContainer;

}; // end of FinslerTractographyImageFilter class

} //end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFinslerTractographyImageFilter.txx"
#endif

#endif
