/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSample.h,v $
  Language:  C++
  Date:      $Date: 2006/02/21 18:53:18 $
  Version:   $Revision: 1.26 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSample_h
#define __itkSample_h

#include "itkMacro.h"
#include "itkPoint.h"
#include "itkSize.h"
#include "itkDataObject.h"
#include "itkFixedArray.h"
#include "itkMeasurementVectorTraits.h"
#include "itkNumericTraits.h"
#include <vector> // for the size_type declaration


namespace itk { 
namespace Statistics {

/** \class Sample 
 *  \brief A collection of measurements for statistical analysis
 *
 * Sample represents a set of measurements for statistical
 * analysis. Sample is templated over a measurement vector. The
 * measurement vector encapsulates the set of values associated with a
 * single measurement.  For instance, a measurement vector may contain
 * an image intensity of a pixel and the gradient magnitude at that pixel.
 * 
 * Data within a sample can be accessed via an
 * InstanceIdentifier. InstanceIdentifiers have different forms and
 * meanings depending on the type of sample.  For ListSamples, the
 * InstanceIdentifier is an index into the corresponding list. In this
 * case, the InstanceIndentifier corresponds to a particular
 * measurement stored in the Sample. For Histograms, an
 * InstanceIdentifier corresponds to a particular bin in the
 * N-dimensional histogram. In other words, the InstanceIdentifier in
 * a histogram does not correspond to a specific measurement used to
 * build the histogram but to the "bin" in which a number of original
 * measurements were "accumulated".
 *
 * <b>Recent API changes:</b>
 * The static const macro to get the length of a measurement vector,
 * 'MeasurementVectorSize'  has been removed to allow the length of a measurement
 * vector to be specified at run time. Please use the function 
 * GetMeasurementVectorSize() instead.
 *
 */

template < class TMeasurementVector >
class ITK_EXPORT Sample : public DataObject
{
public:
  /** Standard class typedefs */
  typedef Sample                        Self;  
  typedef DataObject                    Superclass;
  typedef SmartPointer< Self >          Pointer;
  typedef SmartPointer<const Self >     ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(Sample, DataObject);

  /** MeasurementVector typedef support */ 
  typedef TMeasurementVector MeasurementVectorType;

  /** ValueType of a measurement (ValueType of a component of the
   * MeasurementVector */ 
  typedef typename MeasurementVectorTraitsTypes< 
    MeasurementVectorType >::ValueType   MeasurementType;

  /** Frequency value type */
  typedef MeasurementVectorTraits::FrequencyType       FrequencyType;
  
  /** Total frequency type */
  typedef NumericTraits<FrequencyType>::AccumulateType TotalFrequencyType;

  /** InstanceIdentifier typedef. This identifier is a unique
   * sequential id for each measurement vector in a Sample subclass. */ 
  typedef MeasurementVectorTraits::InstanceIdentifier  InstanceIdentifier;

  /** Typedef for the length of each measurement vector */
  typedef unsigned int  MeasurementVectorSizeType;

  /** Get the size of the sample (number of measurements) */
  virtual InstanceIdentifier Size() const = 0;

  /** Get the measurement associated with a particular
   * InstanceIdentifier. */
  virtual const MeasurementVectorType & 
    GetMeasurementVector( InstanceIdentifier id) const = 0;

  /** Get the frequency of a measurement specified by instance
   * identifier. */
  virtual FrequencyType GetFrequency( InstanceIdentifier id ) const = 0;

  /** Get the total frequency of the sample. */
  virtual TotalFrequencyType GetTotalFrequency() const = 0;

  
  /** Set method for the length of the measurement vector */
  virtual void SetMeasurementVectorSize( MeasurementVectorSizeType s )
    {
    // Test whether the vector type is resizable or not
    MeasurementVectorType m;
    if( MeasurementVectorTraits::IsResizable( m ) )
      {
      // then this is a resizable vector type
      //
      // if the new size is the same as the previou size, just return
      if( s == this->m_MeasurementVectorSize )
        {
        return;
        }
      else
        {
        // If the new size is different from the current size, then
        // only change the measurement vector size if the container is empty.
        if( this->Size() )
          {
          itkExceptionMacro("Attempting to change the measurement \
          vector size of a non-empty Sample");
          }
        else
          {
          this->m_MeasurementVectorSize = s;
          this->Modified();
          }
        }
      }
    else
      {
      // If this is a non-resizable vector type
      MeasurementVectorType m3;
      MeasurementVectorSizeType defaultLength = MeasurementVectorTraits::GetLength( m3 );
      // and the new length is different from the default one, then throw an exception
      if( defaultLength != s )
        {
        itkExceptionMacro("Attempting to change the measurement \
                           vector size of a non-resizable vector type");
        }
      }
    }

  /** Get method for the length of the measurement vector */
  itkGetConstMacro( MeasurementVectorSize, MeasurementVectorSizeType );

  
protected:
  Sample()
    {
    m_MeasurementVectorSize = MeasurementVectorTraits::GetLength( 
                                        MeasurementVectorType() );
    }

  virtual ~Sample() {}

  void PrintSelf(std::ostream& os, Indent indent) const
    {
    Superclass::PrintSelf(os,indent);
    os << indent << "Length of measurement vectors in the sample: " << 
      m_MeasurementVectorSize << std::endl;
    }


  
private:
  Sample(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MeasurementVectorSizeType m_MeasurementVectorSize;
}; // end of class

} // end of namespace Statistics
} // end of namespace itk

#endif
