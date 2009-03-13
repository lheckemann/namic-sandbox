
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTriangleBasisSystem.txx,v $
  Language:  C++
  Date:      $Date:  $
  Version:   $Revision:  $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTriangleBasisSystem_txx
#define __itkTriangleBasisSystem_txx

#include "itkTriangleBasisSystem.h"

namespace itk
{

/**
 * Constructor
 */
template <class TVector, unsigned int M>
TriangleBasisSystem<TVector, M >
::TriangleBasisSystem()
{
  itkDebugMacro("Constructor");
  
}

template <class TVector, unsigned int M >
void
TriangleBasisSystem<TVector, M >
::SetVector( unsigned int k, const VectorType & v )
{
  if (k>=M) 
    {
    itkExceptionMacro(<<"TriangleBasisSystem  SetVector index k is too high.");
    }
  m_Basis[k]= v;
}

template <class TVector, unsigned int M >
const TVector & 
TriangleBasisSystem<TVector, M >
::GetVector( unsigned int k ) const
{
  if (k>=M) 
    {
    itkExceptionMacro(<<"TriangleBasisSystem  GetVector index k is too high.");
    }
  const VectorType & result= m_Basis[k]; 
  return (result);
}

} // end namespace itk

#endif






