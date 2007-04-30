/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkPointSetToListSampleAdaptor.h,v $
Language:  C++
Date:      $Date: 2005/09/30 17:40:35 $
Version:   $Revision: 1.15 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPointSetToListSampleAdaptor_h
#define __itkPointSetToListSampleAdaptor_h

#include <typeinfo>

#include "itkPointSet.h"
#include "itkListSample.h"
#include "itkSmartPointer.h"

namespace itk { 
namespace Statistics {

/** \class PointSetToListSampleAdaptor
 *  \brief This class provides ListSampleBase interfaces to ITK PointSet
 *
 * After calling SetPointSet(PointSet*) method to plug-in 
 * the PointSet object, users can use Sample interfaces to access 
 * PointSet data. This adaptor assumes that the PointsContainer is
 * actual storage for measurment vectors. In other words, PointSet's
 * points are equal to measurement vectors. This class totally ignores
 * PointsDataContainer.
 * 
 * \sa Sample, ListSampleBase, DefaultStaticMeshTraits, PointSet
 */

template < class TPointSet >
class ITK_EXPORT PointSetToListSampleAdaptor :
    public ListSample< typename TPointSet::PointType >
{
public:
  /** Standard class typedefs */
  typedef PointSetToListSampleAdaptor                           Self;
  typedef ListSample< typename TPointSet::PointType >           Superclass;
  typedef SmartPointer< Self >                                  Pointer;
  typedef SmartPointer<const Self>                              ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(PointSetToListSampleAdaptor, ListSample);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** the number of components in a measurement vector */
  itkStaticConstMacro(MeasurementVectorSize, unsigned int,
                      TPointSet::PointDimension);

  /** PointSet typedefs */
  typedef TPointSet                                       PointSetType;
  typedef typename TPointSet::Pointer                     PointSetPointer;
  typedef typename TPointSet::ConstPointer                PointSetConstPointer;
  typedef typename TPointSet::PointIdentifier             InstanceIdentifier;
  typedef typename TPointSet::PointsContainerPointer      PointsContainerPointer;
  typedef typename TPointSet::PointsContainerConstPointer PointsContainerConstPointer;
  typedef typename TPointSet::PointsContainerIterator     PointsContainerIterator;
  typedef typename TPointSet::PointType                   PointType;


  /** Superclass typedefs for Measurement vector, measurement, 
   * Instance Identifier, frequency, size, size element value */
  typedef typename Superclass::MeasurementType            MeasurementType;
  typedef typename Superclass::MeasurementVectorType      MeasurementVectorType;
  typedef MeasurementVectorType                           ValueType;
  typedef typename Superclass::FrequencyType              FrequencyType;
  typedef typename Superclass::TotalFrequencyType         TotalFrequencyType;
  typedef typename Superclass::MeasurementVectorSizeType  MeasurementVectorSizeType;

  /** Method to set the point set */
  void SetPointSet(const TPointSet* pointSet);

  /** Method to get the point set */
  const TPointSet* GetPointSet();

  /** returns the number of measurement vectors in this container*/
  unsigned int Size() const;

  /** returns the measurement vector that is specified by the instance
   * identifier argument. */
  const MeasurementVectorType & GetMeasurementVector(const InstanceIdentifier &id) const;

  /** returns 1 as other subclasses of ListSampleBase does */
  FrequencyType GetFrequency(const InstanceIdentifier &id) const;

  /** returns the size of this container */
  TotalFrequencyType GetTotalFrequency() const;

protected:
  PointSetToListSampleAdaptor(); 
  
  virtual ~PointSetToListSampleAdaptor() {}
  void PrintSelf(std::ostream& os, Indent indent) const;  

private:
  PointSetToListSampleAdaptor(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** the PointSet data source pointer */
  PointSetConstPointer                     m_PointSet;

  /** the points container which will be actually used for storing
   * measurement vectors */
  PointsContainerConstPointer              m_PointsContainer;

  /** temporary points for conversions */
  mutable PointType                        m_TempPoint;

}; // end of class PointSetToListSampleAdaptor

} // end of namespace Statistics
} // end of namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPointSetToListSampleAdaptor.txx"
#endif

#endif
