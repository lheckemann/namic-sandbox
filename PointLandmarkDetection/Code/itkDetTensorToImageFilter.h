/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDetOverTraceNTensorToImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:47 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDetTensorToImageFilter_h
#define __itkDetTensorToImageFilter_h

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
class ITK_EXPORT DetTensorToImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage,
                        Function::Det<typename TInputImage::PixelType::ValueType>   >
{
public:
  /** Standard class typedefs. */
  typedef DetTensorToImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage,
                                  Function::Det< typename TInputImage::PixelType::ValueType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
protected:
  DetTensorToImageFilter() {}
  virtual ~DetTensorToImageFilter() {}

private:
  DetTensorToImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end namespace itk


#endif
