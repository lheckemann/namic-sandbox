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
#include "itkIndex.h"

namespace itk {
namespace Statistics {

template < class TMeasurement = float,
           unsigned int VMeasurementVectorSize = 1,
           class TFrequencyContainer = DenseFrequencyContainer2 >
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

  typedef FixedArray< TMeasurement, VMeasurementVectorSize > MeasurementVectorType;;

  /** Run-time type information (and related methods). */
  itkTypeMacro(Histogram, MeasurementHistogram);

  /** standard New() method support */
  itkNewMacro(Self);

  /** Index typedef support. An index is used to access pixel values. */
  typedef itk::Index< VMeasurementVectorSize >  IndexType;
  typedef typename IndexType::IndexValueType    IndexValueType;

  typedef typename Superclass::InstanceIdentifier        InstanceIdentifier;

  /** size array type */
  typedef itk::Size< VMeasurementVectorSize > SizeType;
  typedef typename SizeType::SizeValueType    SizeValueType;

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

protected:
  Histogram();
  virtual ~Histogram() {}

private:
  Histogram(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHistogram.txx"
#endif

#endif
