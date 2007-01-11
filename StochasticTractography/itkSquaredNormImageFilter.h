/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSquaredNormFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/03 15:07:51 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSquaredNormImageFilter_h
namespace Function {  
  
template< class TInput, class TOutput>
class SquaredNorm{
public:
  SquaredNorm() {}
  ~SquaredNorm() {}
  bool operator!=( const SquaredNorm & ) const{
    return false;
  }
  bool operator==( const SquaredNorm & other ) const{
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A ){
    return A.GetSquaredNorm();
  }
}; 
}

template <class TInputImage, class TOutputImage>
class ITK_EXPORT SquaredNormImageFilter:
public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Function::SquaredNorm< typename TInputImage::PixelType, 
                                          typename TOutputImage::PixelType>   >{
public:
  /** Standard class typedefs. */
  typedef SquaredNormImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Function::SquaredNorm< typename TInputImage::PixelType, 
                                                    typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

protected:
  SquaredNormImageFilter() {}
  virtual ~SquaredNormImageFilter() {}

private:
  SquaredNormImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
