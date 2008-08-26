#ifndef __sbiaOdfReconImageFilter_h_
#define __sbiaOdfReconImageFilter_h_

#include "itkImageToImageFilter.h"
#include "sbiaOrientationDistributionFunction.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/algo/vnl_svd.h"
#include "itkVectorContainer.h"
#include "itkVectorImage.h"

namespace itk{

template< class TReferenceImagePixelType, 
          class TGradientImagePixelType=TReferenceImagePixelType,
          class TOdfPixelType=double,
    unsigned int NOrder=4 >
class ITK_EXPORT OdfReconImageFilter :
  public ImageToImageFilter< Image< TReferenceImagePixelType, 3 >, 
                             Image< OrientationDistributionFunction< TOdfPixelType, NOrder >, 3 > >
{

public:

  typedef OdfReconImageFilter Self;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;
  typedef ImageToImageFilter< Image< TReferenceImagePixelType, 3>, 
          Image< OrientationDistributionFunction< TOdfPixelType, NOrder >, 3 > >
                          Superclass;
  
   /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(OdfReconImageFilter, ImageToImageFilter);
 
  typedef TReferenceImagePixelType                 ReferencePixelType;

  typedef TGradientImagePixelType                  GradientPixelType;

  typedef OrientationDistributionFunction< TOdfPixelType, NOrder >   OdfPixelType;
  
  /** Reference image data,  This image is aquired in the absence 
   * of a diffusion sensitizing field gradient */
  typedef typename Superclass::InputImageType      ReferenceImageType;
  
  typedef Image< OrientationDistributionFunction< TOdfPixelType, NOrder >, 3 >
                     OdfImageType;
  
  typedef OdfImageType                          OutputImageType;

  typedef typename Superclass::OutputImageRegionType
                                                   OutputImageRegionType;

  /** Typedef defining one (of the many) gradient images.  */
  typedef Image< GradientPixelType, 3 >            GradientImageType;

  /** An alternative typedef defining one (of the many) gradient images. 
   * It will be assumed that the vectorImage has the same dimension as the 
   * Reference image and a vector length parameter of \c n (number of
   * gradient directions)*/
  typedef VectorImage< GradientPixelType, 3 >      GradientImagesType;

  /** Holds the tensor basis coefficients G_k */
//  typedef vnl_matrix_fixed< double, OdfPixelType::InternalDimension, OdfPixelType::InternalDimension >        RshBasisMatrixType;
  
  typedef vnl_matrix< double >                     RshBasisMatrixType;

  /** Holds each magnetic field gradient used to acquire one DWImage */
  typedef vnl_vector_fixed< double, 3 >            GradientDirectionType;

  /** Container to hold gradient directions of the 'n' DW measurements */
  typedef VectorContainer< unsigned int, 
          GradientDirectionType >                  GradientDirectionContainerType;
  

  /** Set method to add a gradient direction and its corresponding image. */
  void AddGradientImage( const GradientDirectionType &, const GradientImageType *image);

  /** Another set method to add a gradient directions and its corresponding
   * image. The image here is a VectorImage. The user is expected to pass the 
   * gradient directions in a container. The ith element of the container 
   * corresponds to the gradient direction of the ith component image the 
   * VectorImage.  For the baseline image, a vector of all zeros
   * should be set.*/
  void SetGradientImage( GradientDirectionContainerType *, 
                                             const GradientImagesType *image);
  
  /** Set method to set the reference image. */
  void SetReferenceImage( ReferenceImageType *referenceImage )
    {
    if( m_GradientImageTypeEnumeration == GradientIsInASingleImage)
      {
      itkExceptionMacro( << "Cannot call both methods:" 
      << "AddGradientImage and SetGradientImage. Please call only one of them.");
      }
  
    this->ProcessObject::SetNthInput( 0, referenceImage );

    m_GradientImageTypeEnumeration = GradientIsInManyImages;
    }
    
  /** Get reference image */
  virtual ReferenceImageType * GetReferenceImage() 
  { return ( static_cast< ReferenceImageType *>(this->ProcessObject::GetInput(0)) ); }

  /** Return the gradient direction. idx is 0 based */
  virtual GradientDirectionType GetGradientDirection( unsigned int idx) const
    {
    if( idx >= m_NumberOfGradientDirections )
      {
      itkExceptionMacro( << "Gradient direction " << idx << "does not exist" );
      }
    return m_GradientDirectionContainer->ElementAt( idx+1 );
    }

  /** Threshold on the reference image data. The output tensor will be a null
   * tensor for pixels in the reference image that have a value less than this
   * threshold. */
  itkSetMacro( Threshold, ReferencePixelType );
  itkGetMacro( Threshold, ReferencePixelType );

  /** Flag for whether or not we want normalized odfs 
   */
  itkSetMacro( Normalize, bool );
  itkBooleanMacro( Normalize );
  
#ifdef ITK_USE_CONCEPT_CHECKING
  /* Begin concept checking 
  itkConceptMacro(ReferenceEqualityComparableCheck,
    (Concept::EqualityComparable<ReferencePixelType>));
  itkConceptMacro(TensorEqualityComparableCheck,
    (Concept::EqualityComparable<TensorPixelType>));
  itkConceptMacro(GradientConvertibleToDoubleCheck,
    (Concept::Convertible<GradientPixelType, double>));
  itkConceptMacro(DoubleConvertibleToTensorCheck,
    (Concept::Convertible<double, TensorPixelType>));
  itkConceptMacro(GradientReferenceAdditiveOperatorsCheck,
    (Concept::AdditiveOperators<GradientPixelType, GradientPixelType,
                                ReferencePixelType>));
  itkConceptMacro(ReferenceOStreamWritableCheck,
    (Concept::OStreamWritable<ReferencePixelType>));
  itkConceptMacro(TensorOStreamWritableCheck,
    (Concept::OStreamWritable<TensorPixelType>));

   End concept checking */
#endif

protected:
  OdfReconImageFilter();
  ~OdfReconImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
 
  void ComputeRshBasis();

  void BeforeThreadedGenerateData();
  void ThreadedGenerateData( const 
      OutputImageRegionType &outputRegionForThread, int);
  
  /** enum to indicate if the gradient image is specified as a single multi-
   * component image or as several separate images */
  typedef enum
    {
    GradientIsInASingleImage = 1,
    GradientIsInManyImages,
    Else
    } GradientImageTypeEnumeration;
    
private:
  
  /* Real Spherical harmonic basis coeffs */
  RshBasisMatrixType                                m_RshBasis;

  /** container to hold gradient directions */
  GradientDirectionContainerType::Pointer           m_GradientDirectionContainer;

  /** Number of gradient measurements */
  unsigned int                                      m_NumberOfGradientDirections;
  
  /** Number of baseline images */
  unsigned int                                      m_NumberOfBaselineImages;

  /** Threshold on the reference image data */
  ReferencePixelType                                m_Threshold;
  
  /** Gradient image was specified in a single image or in multiple images */
  GradientImageTypeEnumeration                      m_GradientImageTypeEnumeration;
  
  bool                                              m_Normalize;
  
  itkStaticConstMacro(NumberOfCoeffients,unsigned int, OdfPixelType::InternalDimension);

};

}

#include "sbiaOdfReconImageFilter.txx"

#endif
