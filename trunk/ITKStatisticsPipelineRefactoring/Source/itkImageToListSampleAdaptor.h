/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToListSampleAdaptor.h,v $
  Language:  C++
  Date:      $Date: 2006/10/14 19:58:32 $
  Version:   $Revision: 1.37 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToListSampleAdaptor_h
#define __itkImageToListSampleAdaptor_h

#include <typeinfo>

#include "itkImage.h"
#include "itkPixelTraits.h"
#include "itkListSample.h"
#include "itkSmartPointer.h"
#include "itkImageRegionIterator.h"
#include "itkFixedArray.h"
#include "itkMacro.h"

namespace itk { 
namespace Statistics {

/** \class ImageToListSampleAdaptor
 *  \brief This class provides ListSampleBase interfaces to ITK Image
 *
 * After calling SetImage( const Image * ) method to plug in the image object,
 * users can use Sample interfaces to access Image data.
 * However, the resulting data are a list of measurement vectors. The type of
 * data is measurement vector. For example, if the pixel type of Image object 
 * is STL vector< float > and each pixel has two different types of 
 * measurements, intensity and gradient magnitude, this adaptor has
 * measurement vector of type ITK Point< float, 2>, and one element of the Point
 * is intensity and the other is gradient magnitude.
 *
 * There are two concepts of dimensions for this container. One is for Image 
 * object, and the other is for measurement vector dimension.
 * Only when using ITK Index to access data, the former concept is applicable
 * Otherwise, dimensions means dimensions of measurement vectors. 
 *
 * From the above example, there were two elements in a pixel and each pixel
 * provides [] operator for accessing its elements. However, in many cases,
 * The pixel might be a scalar value such as int or float. In this case,
 * The pixel doesn't support [] operator. To deal with this problem,
 * This class has two companion classes, ScalarAccessor and VectorAccessor.
 * If the pixel type is a scalar type, then you don't have change the third
 * template argument. If you have pixel type is vector one and supports
 * [] operator, then replace third argument with VectorAccessor
 *
 * \sa Sample, ListSampleBase
 */

template < class TImage,
           class TMeasurementVector = 
           ITK_TYPENAME TImage::PixelType >
class ITK_EXPORT ImageToListSampleAdaptor : 
    public ListSample< TMeasurementVector >
{
public:
  /** Standard class typedefs */
  typedef ImageToListSampleAdaptor               Self;
  typedef ListSample< TMeasurementVector >       Superclass;
  typedef SmartPointer< Self >                   Pointer;
  typedef SmartPointer<const Self>               ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToListSampleAdaptor, ListSample);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Image typedefs */
  typedef TImage                                          ImageType;
  typedef typename ImageType::Pointer                     ImagePointer;
  typedef typename ImageType::ConstPointer                ImageConstPointer;
  typedef typename ImageType::IndexType                   IndexType;
  typedef typename ImageType::PixelType                   PixelType;
  typedef typename ImageType::PixelContainerConstPointer  PixelContainerConstPointer;

  typedef typename ImageType::PixelContainer::ElementIdentifier 
  InstanceIdentifier;
  
  /** Image Iterator typedef support */
  typedef ImageRegionIterator< ImageType >               IteratorType; 
  typedef PixelTraits< typename TImage::PixelType >      PixelTraitsType;

  /** Superclass typedefs for Measurement vector, measurement, 
   * Instance Identifier, frequency, size, size element value */
  typedef TMeasurementVector                                   MeasurementVectorType; 
//  typedef typename PixelTraitsType::ValueType                MeasurementType;
  typedef typename MeasurementVectorTraitsTypes< 
             MeasurementVectorType >::ValueType                MeasurementType;
  typedef typename Superclass::FrequencyType                   FrequencyType;
  typedef typename Superclass::TotalFrequencyType              TotalFrequencyType;
  typedef typename Superclass::MeasurementVectorSizeType       MeasurementVectorSizeType;

  typedef MeasurementVectorType                       ValueType;

  /** Method to set the image */
  void SetImage(const TImage* image);

  /** Method to get the image */
  const TImage* GetImage() const;

  /** returns the number of measurement vectors in this container*/
  unsigned int Size() const;

  inline virtual const MeasurementVectorType & GetMeasurementVector(const InstanceIdentifier &id) const;

  inline FrequencyType GetFrequency(const InstanceIdentifier &id) const;

  TotalFrequencyType GetTotalFrequency() const;

protected:
  ImageToListSampleAdaptor();
  virtual ~ImageToListSampleAdaptor() {}
  void PrintSelf(std::ostream& os, Indent indent) const;  

private:
  ImageToListSampleAdaptor(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ImageConstPointer          m_Image;
}; // end of class ImageToListSampleAdaptor

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToListSampleAdaptor.txx"
#endif

#endif
