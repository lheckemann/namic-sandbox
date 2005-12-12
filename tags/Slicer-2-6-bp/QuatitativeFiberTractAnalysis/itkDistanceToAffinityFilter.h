#ifndef __itkDistanceToAffinityFilter_h
#define __itkDistanceToAffinityFilter_h

#include "itkUnaryFunctorImageFilter.h"

namespace itk {

namespace Function {

template<class TInput, class TOutput>
class GaussianWeight
{
public:
  GaussianWeight() {
    m_Order = 2;
    m_Scale = .1;
  };
  ~GaussianWeight() {};
  inline TOutput operator()(const TInput & A)
  {
    return exp(-pow(double(A),double(m_Order))/m_Scale);
  }

  
  double GetScale() {return m_Scale;}
  unsigned int GetOrder() {return m_Order;}

  void SetScale(double scale) { m_Scale = scale;}
  void SetOrder(unsigned int order) {m_Order = order;}

private:
  unsigned int m_Order;
  double m_Scale;
}; // end GaussianWeight class

} // end function namespace

template<class TInputImage, class TOutputImage>
class ITK_EXPORT DistanceToAffinityFilter :
    public UnaryFunctorImageFilter<TInputImage,TOutputImage,
                                   Function::GaussianWeight<
  typename TInputImage::PixelType,
  typename TOutputImage::PixelType> >
{
public:
  typedef DistanceToAffinityFilter Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage,
                                   Function::GaussianWeight<
    typename TInputImage::PixelType,
    typename TOutputImage::PixelType> > SuperClass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  itkNewMacro(Self);

  void SetOrder(unsigned int order)
  {
    if(order == this->GetFunctor().GetOrder() )
      {
      return;
      }
    this->GetFunctor().SetOrder(order);
    this->Modified();
  }

  void SetScale(double scale)
  {
    if(scale == this->GetFunctor().GetScale() )
      {
      return;
      }
    this->GetFunctor().SetScale(scale);
    this->Modified();
  }
protected:
  DistanceToAffinityFilter() {}
  virtual ~DistanceToAffinityFilter() {}

private:
  DistanceToAffinityFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};


} // end namespace itk

#endif
