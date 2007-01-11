  Module:    $RCSfile: itkReplaceEntryVectorImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/03 15:07:51 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkReplaceEntryVectorImageFilter_h
namespace Function {  
template< class TInput, class TOutput >
class ReplaceEntry{
public:
  typedef VectorContainer< unsigned int >  IndexContainerType;
  typedef IndexContainer::Pointer  IndexContainerPointer;
  typedef VectorContainer< TOutput >  ValueContainerType;
  typedef ValueContainer::Pointer ValueContainerPointer;
  
  ReplaceEntry() {
      m_indices = IndexContainerType::New();
      m_values = ValueContainerType::New();
  }
  ~ReplaceEntry() {}
  bool operator!=( const ReplaceEntry & ) const{
    return false;
  }
  bool operator==( const ReplaceEntry & other ) const{
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A ){
    TOutput pixel = A;
    
    for(int i=0; i<m_indices.Size(); i++)
      pixel.SetElement( m_indices->GetElement(i), m_values->GetElement(i) );
      
      return pixel;
  }
  
  inline void AddEntryReplacement(unsigned int index, TOutput::ValueType value){
    m_indices->InsertElement( m_indices->Size(), index );
    m_values->InsertElement( m_values->Size(), value);
  }
protected:
  IndexContainerPointer m_indices;
  ValueContainerPointer m_values;
}; 
}

template <class TInputImage, class TOutputImage>
class ITK_EXPORT ReplaceEntryImageFilter:
public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Function::ReplaceEntry< typename TInputImage::PixelType, 
                                            typename TOutputImage::PixelType>   >{
public:
  /** Standard class typedefs. */
  typedef ReplaceEntryImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Function::ReplaceEntry< typename TInputImage::PixelType, 
                                                    typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  typedef TOutputImage OuputImageType;
  typedef OutputImage::PixelType PixelType;
  typedef PixelType::ValueType ValueType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  inline void AddEntryReplacement(unsigned int index, ValueType value){
    GetFunctor()->SetEntryReplacement(index, value);
  }
protected:
  ReplaceEntryImageFilter() {}
  virtual ~ReplaceEntryImageFilter() {}

private:
  ReplaceEntryImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
