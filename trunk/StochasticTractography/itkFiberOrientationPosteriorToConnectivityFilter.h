#ifndef __itkFiberOrientationPosteriorToConnectivityFilter_h__
#define __itkFiberOrientationPosteriorToConnectivityFilter_h__

#include "itkImageToImageFilter.h"

namespace itk{
template< class TFiberOrientationPosteriorImage, class TConnectivityImage >
class ITK_EXPORT FiberOrientationPosteriorToConnectivityFilter :
  public ImageToImageFilter< TFiberOrientationLikelihoodImage,
                    TConnectivityImage >{
public:
  typedef FiberOrientationPosteriorToConnectivityFilter Self;
  typedef ImageToImageFilter< TFiberOrientationPosteriorImage,
                      TConnectivityImage > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  itkNewMacro(Self);
  itkTypeMacro( FiberOrientationPosteriorToConnectivityFilter,
            ImageToImageFilter );
  
  typedef TFiberOrientationPosteriorImage FOLImageType;
  typedef FOLImageType::ConstPointer FOLImageConstPointer;
  typedef FOLImageType::PointType FOLImagePointType;
  typedef FOLImageType::IndexType FOLImageIndexType;
  typedef FOLImageType::PixelType FOLImagePixelType;
  typedef TConnectivityImage CImageType;
  typedef CImageType::Pointer  CImagePointer;
  
  typedef PolyLineParametricPath< 3 > PathType;
  typedef PathType::Pointer PathPointer;
  typedef PathType::ContinuousIndexType PathContinuousIndexType;
  
  typedef vnl_vector_fixed< double, 3 > FiberOrientationType;
  typedef VectorContainer< unsigned int, FiberDirectionType > FOContainerType;
  typedef FOContainer::Pointer FOContainerPointer;
  
  /** the number of tracts to generate **/
  itkSetMacro( TotalTracts, unsigned int);
  itkGetMacro( TotalTracts, unsigned int);
  
  /** the maximum length of fiber **/
  itkSetMacro( MaxTractLength, unsigned int );
  itkGetMacro( MaxTractLength, unsigned int );
  
  /** Set/Get the seed point **/
  itkSetMacro( SeedPoint, FOLImagePointType );
  itkGetMacro( SeedPoint, FOLImagePointType );
  
  /** Set/Get the list of directions to sample **/
  itkSetObjectMacro( SampleDirections, FiberOrientationContainerType );
  itkGetObjectMacro( SampleDirections, FiberOrientationContainerType );
  
  void GenerateData();
protected:
  FiberOrientationPosteriorToConnectivityFilter();
  virtual ~FiberOrientationPosteriorToConnectivityFilter();

  unsigned int m_MaxTractLength;
  unsigned int m_TotalFibers;
  FOLImagePointType m_SeedPoint;
  FOContainerPointer m_SampleDirections;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFiberOrientationPosteriorToConnectivityFilter.h"
#endif

#endif
