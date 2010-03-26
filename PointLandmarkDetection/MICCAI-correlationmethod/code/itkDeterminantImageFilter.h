/*=========================================================================

author: Bryn Lloyd, Computational Radiology Laborotory (CRL), Brigham and Womans
  date: 06/30/2005

=========================================================================*/
#ifndef __itkDeterminantImageFilter_h
#define __itkDeterminantImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkSymmetricSecondRankTensor.h"
//#include "vnl/vnl_math.h"

//#if defined(_MSC_VER) && (_MSC_VER >= 1300)
//#pragma function(exp)
//#endif

namespace itk
{

/** \class DetOverTraceNTensorToImageFilter
 * \brief Computes the det/trace^N from the symmetric second rank tensor
 *  pixel-wise, where N is the image Dimension.
 *
 * 
 * \ingroup IntensityImageFilters  Multithreaded
 *
 */
  /** The input SymmetricSecondRankTensor follows the following convention:

      2D: 0 1      3D: 0 1 2
          1 2          1 3 4
                       2 4 5
      Determinant:
      2D: 0*2-1*1  3D: 0*(3*5-4*4) - 1(1*5-2*4) + 2(1*4-2*3)

      Trace:
      2D: 0 + 2   3D: 0 + 3 + 5
  */

namespace Function {  
template< class TValueType>
class Det
{
public:
  Det() {};
  ~Det() {};
  //The 2D function
  inline TValueType operator()( const SymmetricSecondRankTensor<TValueType,2> & A )
  {
    TValueType  determinant = A[0]*A[2]-A[1]*A[1];
    return determinant;
  }

  // The 3D function
  inline TValueType operator()( const SymmetricSecondRankTensor<TValueType,3> & A )
  {
    TValueType  determinant = A[0]*(A[3]*A[5]-A[4]*A[4]) - A[1]*(A[1]*A[5]-A[2]*A[4]) + A[2]*(A[1]*A[4]-A[2]*A[3]);
    return determinant;
  }
private:
   const static TValueType eps = 0.0000001;
};

}
template <class TInputImage, class TOutputImage>
class ITK_EXPORT DeterminantImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage,
                        Function::Det<typename TInputImage::PixelType::ValueType>   >
{
public:
  /** Standard class typedefs. */
  typedef DeterminantImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage,
                                  Function::Det< typename TInputImage::PixelType::ValueType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
protected:
  DeterminantImageFilter() {}
  virtual ~DeterminantImageFilter() {}

private:
  DeterminantImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end namespace itk


#endif