/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogram.h,v $
  Language:  C++
  Date:      $Date: 2005/09/30 17:24:45 $
  Version:   $Revision: 1.49 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHistogram_h
#define __itkHistogram_h

#include <vector>

#include "itkMeasurementHistogram.h"
#include "itkDenseFrequencyContainer.h"
#include "itkIndex.h"

namespace itk {
namespace Statistics {
template<unsigned int VDimension=2>
class HistogramPrivateSize : public itk::Size<VDimension>
{
public:
  HistogramPrivateSize() {}
  HistogramPrivateSize(unsigned int dimension) {}
  unsigned int GetNumberOfElements() { return VDimension; }
};

template<unsigned int VDimension=2>
class HistogramPrivateIndex : public itk::Index<VDimension>
{
public:
  HistogramPrivateIndex() {}
  HistogramPrivateIndex(unsigned int dimension) {}
  unsigned int GetNumberOfElements() { return VDimension; }
};

template<typename TValueType = float, unsigned int VLength=1>
class HistogramPrivateMeasurementVector : public itk::FixedArray<TValueType, VLength>
{
public:
  HistogramPrivateMeasurementVector() {}
  HistogramPrivateMeasurementVector(unsigned int dimension) {}
};

template < class TMeasurement = float,
           unsigned int VMeasurementVectorSize = 1,
           class TFrequencyContainer = DenseFrequencyContainer >
class ITK_EXPORT Histogram
  : public MeasurementHistogram < TMeasurement, TFrequencyContainer >
{
public:
  /** Standard typedefs */
  typedef Histogram                    Self;
  typedef MeasurementHistogram < TMeasurement, TFrequencyContainer  >
                                       Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;

  typedef HistogramPrivateMeasurementVector< TMeasurement, VMeasurementVectorSize > MeasurementVectorType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(Histogram, MeasurementHistogram);

  /** standard New() method support */
  itkNewMacro(Self);

  /** frequency container typedef */
  typedef TFrequencyContainer                      FrequencyContainerType;
  typedef typename FrequencyContainerType::Pointer FrequencyContainerPointer;

  /** Frequency and TotalFrequency value type from superclass */
  typedef typename FrequencyContainerType::FrequencyType      FrequencyType;
  typedef typename FrequencyContainerType::TotalFrequencyType TotalFrequencyType;

  /** Index typedef support. An index is used to access pixel values. */
  typedef HistogramPrivateIndex< VMeasurementVectorSize >  IndexType;
  typedef typename IndexType::IndexValueType               IndexValueType;

  typedef typename Superclass::InstanceIdentifier        InstanceIdentifier;

  /** size array type */
  typedef HistogramPrivateSize< VMeasurementVectorSize > SizeType;
  typedef typename SizeType::SizeValueType               SizeValueType;

  /** Initialize the histogram, generating the offset table and
   * preparing the frequency container. Subclasses should call this
   * method in their Initialize() method. */
  void Initialize(const SizeType &size);

  /** Initialize the histogram using equal size bins. To assign bin's
   * min and max values along each dimension use SetBinMin() and
   * SetBinMax() functions. */
  void Initialize(const SizeType &size, MeasurementVectorType& lowerBound,
                  MeasurementVectorType& upperBound);

  /** Get the index of a measurement value from the histogram.
   * \deprecated Use GetIndex(const MeasurementVectorType &
   * measurement, IndexType & index ) const instead. */
  const IndexType & GetIndex(const MeasurementVectorType& measurement) const;

  /** Get the index of histogram corresponding to the specified
   *  measurement value. Returns true if index is valid and false if
   *  the measurement is outside the histogram */
  bool GetIndex(const MeasurementVectorType & measurement,
                IndexType & index ) const;
  
  /** Get the index that is uniquely labelled by an instance identifier
   * The corresponding id is the offset of the index 
   * This method uses ImageBase::ComputeIndex() method */
  const IndexType & GetIndex(const InstanceIdentifier &id) const;

  /** Get the instance identifier of the bin that is indexed by the 
   * index. The corresponding instance identifier is the offset of the index 
   * This method uses ImageBase::ComputeIndex() method */
  InstanceIdentifier GetInstanceIdentifier(const IndexType &index) const;

  /** Increase the frequency of an instance identifier.
   * Frequency is increased by the specified value. Returns false if
   * the bin is out of bounds. */
  bool IncreaseFrequency(const InstanceIdentifier &id,
                         const FrequencyType value);

  /** Increase the frequency of an index.  Frequency is
   * increased by the specified value. Returns false if the bin is out
   * of bounds. */
  bool IncreaseFrequency(const IndexType &index, 
                         const FrequencyType value);
  
  /** Increase the frequency of a measurement.  Frequency is
   * increased by the specified value. Returns false if the
   * measurement is outside the bounds of the histogram. */
  bool IncreaseFrequency(const MeasurementVectorType &measurement, 
                         const FrequencyType value);
  
  /** Get the measurement of an instance identifier. This is the
   * centroid of the bin.
   */
  const MeasurementVectorType & GetMeasurementVector(const InstanceIdentifier &id) const;
  
  /** Get the measurement of an index. This is the centroid of the bin. */
  const MeasurementVectorType & GetMeasurementVector(const IndexType &index) const;
  
  /** Returns true if the given index is out of bound meaning one of index
   * is not between [0, last index] */
  bool IsIndexOutOfBounds(const IndexType &index) const;

  /** Get the size (N-dimensional) of the histogram  */
  SizeType GetSize() const
    { 
    SizeType mySize;
    for (unsigned int i = 0; i < VMeasurementVectorSize; i++)
      {
      mySize[i] = Superclass::GetSize()[i];
      }
    return mySize;
    }

  /** Get the size of histogram along a specified dimension */
  SizeValueType GetSize(const unsigned int dimension) const
    {
    return Superclass::GetSize(dimension); 
    }

public:

  /** \class Histogram::ConstIterator class that walks through the elements of
   * the histogram */
  class ConstIterator
    {
    public:

    friend class Histogram;

    ConstIterator(const Self * histogram)
      {
      m_Id = 0;
      m_Histogram = histogram;
      }

    ConstIterator(const ConstIterator & it)
      {
      m_Id        = it.m_Id;
      m_Histogram = it.m_Histogram;
      }

    ConstIterator& operator=(const ConstIterator& it)
      {
      m_Id  = it.m_Id;
      m_Histogram = it.m_Histogram;
      return *this;
      }

    FrequencyType GetFrequency() const
      {
      return  m_Histogram->GetFrequency(m_Id);
      }

    InstanceIdentifier GetInstanceIdentifier() const
      {
      return m_Id;
      }

    const MeasurementVectorType & GetMeasurementVector() const
      {
      return m_Histogram->GetMeasurementVector(m_Id);
      }

    ConstIterator& operator++()
      {
      ++m_Id;
      return *this;
      }

    bool operator!=(const ConstIterator& it)
      {
      return (m_Id != it.m_Id);
      }

    bool operator==(const ConstIterator& it)
      {
      return (m_Id == it.m_Id);
      }

  protected:
    // This method is purposely not implemented
    ConstIterator();

    ConstIterator(InstanceIdentifier id, const Self * histogram)
      : m_Id(id), m_Histogram(histogram)
      {}

    // ConstIterator pointing DenseFrequencyContainer
    InstanceIdentifier m_Id;

    // Pointer of DenseFrequencyContainer
    const Self* m_Histogram;
    }; // end of iterator class

  /** \class Histogram::Iterator */
  class Iterator : public ConstIterator
    {
    public:

    Iterator(Self * histogram):ConstIterator( histogram )
      {
      }

    Iterator(InstanceIdentifier id, Self * histogram)
        : ConstIterator( id, histogram )
      {}

    Iterator(const Iterator & it):ConstIterator(it)
      {
      }

    Iterator & operator=(const Iterator& it)
      {
      this->ConstIterator::operator=( it );
      return *this;
      }

    bool SetFrequency(const FrequencyType value)
      {
      Self * histogram = const_cast< Self * >( this->m_Histogram );
      return histogram->SetFrequency( this->m_Id, value );
      }

    protected:
    // To ensure const-correctness these method must not be in the public API.
    // The are purposly not implemented, since they should never be called.
    Iterator();
    Iterator(const Self * histogram);
    Iterator(InstanceIdentifier id, const Self * histogram);
    Iterator(const ConstIterator & it);
    ConstIterator& operator=(const ConstIterator& it);

    }; // end of iterator class


  Iterator  Begin()
    {
    Iterator iter(0, this);
    return iter;
    }

  Iterator  End()
    {
      return Iterator(Superclass::m_OffsetTable[this->GetMeasurementVectorSize()], this);
    }

  ConstIterator  Begin() const
    {
    ConstIterator iter(0, this);
    return iter;
    }

  ConstIterator End() const
    {
      return ConstIterator(Superclass::m_OffsetTable[this->GetMeasurementVectorSize()], this);
    }

protected:
  Histogram();
  virtual ~Histogram() {}

private:
  Histogram(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  mutable IndexType m_TempIndex;
  mutable MeasurementVectorType m_InternalMeasurementVector;
};

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHistogram.txx"
#endif

#endif
