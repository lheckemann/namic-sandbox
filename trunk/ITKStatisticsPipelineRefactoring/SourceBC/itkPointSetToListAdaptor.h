/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkPointSetToListAdaptor.h,v $
Language:  C++
Date:      $Date: 2009-03-04 15:24:04 $
Version:   $Revision: 1.16 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPointSetToListAdaptor_h
#define __itkPointSetToListAdaptor_h

#include "itkPointSetToListSampleAdaptor.h"

namespace itk { 
namespace Statistics {

/** \class PointSetToListAdaptor
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
class ITK_EXPORT PointSetToListAdaptor :
    public PointSetToListSampleAdaptor<TPointSet>
{
public:
  /** Standard class typedefs */
  typedef PointSetToListAdaptor                           Self;
  typedef PointSetToListSampleAdaptor<TPointSet >         Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(PointSetToListAdaptor, PointSetToListSampleAdaptor);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
protected:
  PointSetToListAdaptor() {}
  virtual ~PointSetToListAdaptor() {}

private:
  PointSetToListAdaptor(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end of namespace Statistics
} // end of namespace itk

#endif
