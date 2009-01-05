/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPointLocator.h,v $
  Language:  C++
  Date:      $Date: 2003-09-10 14:29:23 $
  Version:   $Revision: 1.23 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPointLocator2_h
#define __itkPointLocator2_h

#include "itkObject.h"

namespace itk
{

/** \class PointLocator2
 * \brief Accelerate geometric searches for points.
 *
 * This class accelerates the search for the closest point to a user-provided
 * point, by using constructing a Kd-Tree structure for the PointSet.
 *
 */
template < class TPointSet >
class ITK_EXPORT PointLocator2 : public Object
{
public:
  /** Standard class typedefs. */
  typedef PointLocator2               Self;
  typedef Object                      Superclass;
  typedef SmartPointer<Self>          Pointer;
  typedef SmartPointer<const Self>    ConstPointer;
    
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Standard part of every itk Object. */
  itkTypeMacro(PointLocator2, Object);

  /** Typedefs related to the PointSet type */
  typedef TPointSet                             PointSetType;
  typedef typename PointSetType::ConstPointer   PointSetConstPointer;

  /** Connect the PointSet as input */
  itkSetConstObjectMacro( PointSet, PointSetType );
  itkGetConstObjectMacro( PointSet, PointSetType );

protected:
  PointLocator2();
  ~PointLocator2();
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

private:
  PointLocator2(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PointSetConstPointer     m_PointSet;
};

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPointLocator2.txx"
#endif
  
#endif


