/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToHistogramFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/08/24 15:16:46 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToHistogramFilter_h
#define __itkImageToHistogramFilter_h


#include "itkImageToListAdaptor.h"
#include "itkListSampleToHistogramGenerator.h"
#include "itkObject.h"


namespace itk {
namespace Statistics {

template <typename TAdaptor >
struct GetAdaptorMeasurementVectorLength
{
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TAdaptor::MeasurementVectorSize );
};

/** \class ImageToHistogramFilter
 *  \brief This class generates an histogram from an image.
 *
 *  The concept of Histogram in ITK is quite generic. It has been designed to
 *  manage multiple components data. This class facilitates the computation of
 *  an histogram from an image. Internally it creates a List that is feed into
 *  the ListSampleToHistogramGenerator.
 *
 */
  
 
template< class TImageType >
class ImageToHistogramFilter : public ProcessObject
{
public:
  /** Standard typedefs */
  typedef ImageToHistogramFilter        Self;
  typedef Object                        Superclass;
  typedef SmartPointer<Self>            Pointer;
  typedef SmartPointer<const Self>      ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToHistogramFilter, Object);

  /** standard New() method support */
  itkNewMacro(Self);

  typedef TImageType                                      ImageType;
  typedef ImageToListAdaptor< ImageType >                 AdaptorType;
  typedef typename AdaptorType::Pointer                   AdaptorPointer;
  typedef typename ImageType::PixelType                   PixelType;
  typedef typename PixelType::ValueType                   ValueType;
  typedef typename NumericTraits< ValueType >::RealType   ValueRealType;
  typedef DenseFrequencyContainer                         FrequencyContainerType;

  typedef ListSampleToHistogramGenerator< 
                         AdaptorType, 
                         ValueRealType,
                         FrequencyContainerType,
                         ::itk::Statistics::GetAdaptorMeasurementVectorLength< 
                                          AdaptorType >::MeasurementVectorLength
                           > GeneratorType;

  typedef typename GeneratorType::Pointer                   GeneratorPointer;

  typedef typename GeneratorType::HistogramType             HistogramType;
  typedef typename HistogramType::Pointer                   HistogramPointer;
  typedef typename HistogramType::ConstPointer              HistogramConstPointer;
  typedef typename HistogramType::SizeType                  SizeType;
  typedef typename HistogramType::MeasurementVectorType     MeasurementVectorType;

public:

  /** Connects the input image for which the histogram is going to be computed */
  void SetInput( const ImageType * );
  
  /** Return the output histogram.
   \warning This output is only valid after the Compute() method has been invoked 
   \sa Compute */
  const HistogramType * GetOutput() const;
  
  /** Set number of histogram bins */
  itkSetMacro( NumberOfBins, SizeType );
  itkGetMacro( NumberOfBins, SizeType );
 
  /** Set marginal scale value to be passed to the histogram generator */
  itkSetMacro( MarginalScale, double );
  itkGetMacro( MarginalScale, double );

  /** Set/Get the min and max bounds to the histogram */
  void SetHistogramMin(const MeasurementVectorType & histogramMin);
  void SetHistogramMax(const MeasurementVectorType & histogramMax);

  /** Set/Get whether the filter will compute the min and max values of the
   * histogram automatically. */
  itkSetMacro( AutoMinMax, bool );
  itkGetMacro( AutoMinMax, bool );

protected:
  ImageToHistogramFilter();
  virtual ~ImageToHistogramFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Triggers the Computation of the histogram */
  void GenerateData( void );


private:

  AdaptorPointer          m_ImageToListAdaptor;

  GeneratorPointer        m_HistogramGenerator;

  MeasurementVectorType   m_HistogramMin;
  MeasurementVectorType   m_HistogramMax;

  SizeType                m_NumberOfBins;

  double                  m_MarginalScale;

  bool                    m_AutoMinMax;

};


} // end of namespace Statistics 
} // end of namespace itk 

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToHistogramFilter.txx"
#endif

#endif
