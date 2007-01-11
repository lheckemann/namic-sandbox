/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGeometricSumFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/03 15:07:51 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGeometricSumFilter_h
namespace Function {  
template< class TInput, class TOutput>
class GeometricSum{
public:
  GeometricSum() {}
  ~GeometricSum() {}
  bool operator!=( const GeometricSum & ) const{
    return false;
  }
  bool operator==( const GeometricSum & other ) const{
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A ){
      TOutput sum=1;
    for(int i=0; i < A.Size(); i++){
        sum*=A[i];
      }
      return sum;
  }
}; 
}

template <class TInputImage, class TOutputImage>
class ITK_EXPORT GeometricSumImageFilter:
public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Function::GeometricSum< typename TInputImage::PixelType, 
                                            typename TOutputImage::PixelType>   >{
public:
  /** Standard class typedefs. */
  typedef GeometricSumImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Function::GeometricSum< typename TInputImage::PixelType, 
                                                    typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

protected:
  GeometricSumImageFilter() {}
  virtual ~GeometricSumImageFilter() {}

private:
  GeometricSumImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
