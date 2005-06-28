/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDiadicProductOverTraceNTensorToImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:47 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for DiadicProductails.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkOuterProductImageFilter_h
#define __itkOuterProductImageFilter_h

#include "itkUnaryFunctorImageFilter.h"

#include "itkFixedArray.h"
#include "itkSymmetricSecondRankTensor.h"
//#include "vnl/vnl_math.h"

//#if defined(_MSC_VER) && (_MSC_VER >= 1300)
//#pragma function(exp)
//#endif

namespace itk
{

/** \class DiadicProductOverTraceNTensorToImageFilter
 * \brief Computes the outer product of a vector pixel. The
 *  output is a SymmetricSecondRankTensor.
 *
 *
 * \ingroup IntensityImageFilters  Multithreaded
 *
 */

namespace Function {
template< class TInputPixelType, unsigned int TArrayLength>
class DiadicProduct
{
public:
  DiadicProduct() {};
  ~DiadicProduct() {};

  inline SymmetricSecondRankTensor<typename TInputPixelType::ValueType,TArrayLength> operator()( const TInputPixelType & in )
  {
    SymmetricSecondRankTensor<typename TInputPixelType::ValueType,TArrayLength> out;
    unsigned int count=0;
//std::cout << in << std::endl;
    for (unsigned int k=0; k<TArrayLength; k++)
      {
      for (unsigned int l=k; l<TArrayLength; l++)
        {
        out[count++] = in[k]*in[l];
        }
      }
if (out[0]!=0)
{ std::cout << out << std::endl;
  std::cout << in << std::endl; }
    return out;
  }
private:
   //unsigned int ArrayLength;
};

}
template <class TInputImage, unsigned int TArrayLength=itk::GetImageDimension<TInputImage>::ImageDimension>
class ITK_EXPORT OuterProductImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,Image<SymmetricSecondRankTensor<typename TInputImage::PixelType::ValueType, TArrayLength>,itk::GetImageDimension<TInputImage>::ImageDimension>,
                        Function::DiadicProduct<typename TInputImage::PixelType, TArrayLength >   >
{
public:
  /** Standard class typedefs. */
  typedef OuterProductImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,Image<SymmetricSecondRankTensor<typename TInputImage::PixelType::ValueType,TArrayLength>, itk::GetImageDimension<TInputImage>::ImageDimension>,
                                  Function::DiadicProduct< typename TInputImage::PixelType, TArrayLength> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

protected:
  OuterProductImageFilter() {}
  virtual ~OuterProductImageFilter() {}

private:
  OuterProductImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end namespace itk


#endif
