/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToListAdaptor.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 15:23:50 $
  Version:   $Revision: 1.39 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToListAdaptor_h
#define __itkImageToListAdaptor_h

#include "itkImageToListSampleAdaptor.h"

namespace itk { 
namespace Statistics {

/** \class ImageToListAdaptor
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
           class TDummy = 
           ITK_TYPENAME TImage::PixelType >
class ITK_EXPORT ImageToListAdaptor : 
    public ImageToListSampleAdaptor<TImage>
{
public:
  /** Standard class typedefs */
  typedef ImageToListAdaptor                   Self;
  typedef ImageToListSampleAdaptor<TImage>     Superclass;
  typedef SmartPointer< Self >                 Pointer;
  typedef SmartPointer<const Self>             ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToListAdaptor, ImageToListSampleAdaptor);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
protected:
  ImageToListAdaptor(){};

private:
  ImageToListAdaptor(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
}; // end of class ImageToListAdaptor
} // end of namespace Statistics
} // end of namespace itk

#endif
