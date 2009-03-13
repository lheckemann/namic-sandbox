/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTriangleBasisSystem.h,v $
  Language:  C++
  Date:      $Date: 2008-10-17 13:35:26 $
  Version:   $Revision: 1.47 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTriangleBasisSystem_h
#define __itkTriangleBasisSystem_h

#include "itkMesh.h"
#include "itkPoint.h"

namespace itk
{


/** \class TriangleBasisSystem
 * \brief  Stores basis coefficients at a triangular cell.
 *
 * TriangleBasisSystem stores basis coefficients
 * within a triangle. Basis coefficients can be used thereafter
 * for interpolation and gradient computation within that triangle. 
 *
 * This class is templated over the input vector type and dimension of basis.
 *
 *
 *
 * \sa Cell
 *
 * \ingroup TriangleBasisSystems
 */
template <class TVector, unsigned int M >
class ITK_EXPORT TriangleBasisSystem : public Object
{
public:
  typedef TVector   VectorType;

  /** Standard class typedefs. */
  typedef TriangleBasisSystem             Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Standard part of every itk Object. */
  itkTypeMacro(TriangleBasisSystem, Object);


  void SetVector( unsigned int k, const VectorType & v );
  const VectorType & GetVector( unsigned int k ) const;
protected:
  TriangleBasisSystem();
  virtual ~TriangleBasisSystem() {};
private:
  VectorType m_Basis[M];

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTriangleBasisSystem.txx"
#endif

#endif
