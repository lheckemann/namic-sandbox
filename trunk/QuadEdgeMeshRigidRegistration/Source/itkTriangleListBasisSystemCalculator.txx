/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTriangleListBasisSystemCalculator.txx,v $
  Language:  C++
  Date:      $Date:  $
  Version:   $Revision:  $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTriangleListBasisSystemCalculator_txx
#define __itkTriangleListBasisSystemCalculator_txx

#include "itkTriangleCell.h"
#include "itkTriangleBasisSystem.h"
#include "itkTriangleListBasisSystemCalculator.h"
//#include "itkPointLocator2.h"

namespace itk
{

/**
 * Constructor
 */
template <class TMesh, class TBasisSystem >
TriangleListBasisSystemCalculator<TMesh, TBasisSystem>
::TriangleListBasisSystemCalculator()
{
  itkDebugMacro("Constructor");
  this->m_InputMesh= NULL; 
  this->m_List= NULL; 
}

/**
 * Destructor
 */
template <class TMesh, class TBasisSystem >
TriangleListBasisSystemCalculator<TMesh, TBasisSystem>
::~TriangleListBasisSystemCalculator()
{
  itkDebugMacro("Destructor");
}


template <class TMesh, class TBasisSystem >
void
TriangleListBasisSystemCalculator<TMesh, TBasisSystem>
::Calculate( void ) 
{

  if( this->m_InputMesh.IsNull() ) 
    {
    itkExceptionMacro(<<"TriangleListBasisSystemCalculator CalculateTriangle  m_InputMesh is NULL.");
    }

  //const unsigned int SurfaceDimension = 2; 

  CellsContainerIterator cellIterator = this->m_InputMesh->GetCells()->Begin();
  CellsContainerIterator cellEnd = this->m_InputMesh->GetCells()->End();

  TBasisSystem triangleBasisSystem;
  //triangleBasisSystem= TBasisSystem::New();

  unsigned int cellIndex=0; 
  while( cellIterator != cellEnd && cellIterator != cellEnd )
    {

    this->CalculateTriangle( cellIndex, triangleBasisSystem);
    m_List->push_back( triangleBasisSystem ); 

    ++cellIterator;
    ++cellIndex;
    }
}

template <class TMesh, class TBasisSystem >
const VectorContainer<TMesh, TBasisSystem> *
TriangleListBasisSystemCalculator<TMesh, TBasisSystem>
::GetBasisSystemList(void) const
{

   if( this->m_List.IsNull() ) 
    {
    itkExceptionMacro(<<"TriangleListBasisSystemCalculator GetBasisSystemList  m_List is NULL.");
    } 

    return m_List; 
}

} // end namespace itk

#endif
