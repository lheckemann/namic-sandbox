#ifndef __itkTubeSpatialObjectToDistanceFilter_h
#define __itkTubeSpatialObjectToDistanceFilter_h

#include "itkImageSource.h"

namespace itk {

// Group should be something with a std:: type iter
template<class TInputTubeSpatialObjectGroup, class TOutputImage>
class ITK_EXPORT TubeSpatialObjectToDistanceFilter : public ImageSource<TOutputImage>
{
public:
  typedef TubeSpatialObjectToDistanceFilter Self;
  typedef ImageSource<TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(TubeSpatialObjectToDistanceFilter,Superclass);

  /** Some convenient typedefs. */
  typedef TOutputImage                         OutputImageType;
  typedef typename OutputImageType::Pointer    OutputImagePointer;
  typedef typename OutputImageType::ValueType  ValueType;

  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;


  typedef TInputTubeSpatialObjectGroup InputTubeSpatialObjectGroupType;

  virtual void SetInput(const InputTubeSpatialObjectGroupType *pointset);
  virtual void SetInput(unsigned int, const InputTubeSpatialObjectGroupType *pointset);

  virtual const InputTubeSpatialObjectGroupType* GetInput();
  virtual const InputTubeSpatialObjectGroupType* GetInput(unsigned int idx);

protected:
  TubeSpatialObjectToDistanceFilter();
  ~TubeSpatialObjectToDistanceFilter() {};

  virtual void GenerateOutputInformation() {}; // do nothing
  virtual void GenerateData();

  virtual void PrintSelf(std::ostream& os, Indent indent) const;

private:
  TubeSpatialObjectToDistanceFilter(const Self&); // purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTubeSpatialObjectToDistanceFilter.txx"
#endif

#endif
