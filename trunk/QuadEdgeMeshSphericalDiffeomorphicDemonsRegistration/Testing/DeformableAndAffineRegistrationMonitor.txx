/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTestingMacros.h,v $
  Language:  C++
  Date:      $Date: 2009-05-09 17:40:20 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "DeformableAndAffineRegistrationMonitor.h"

#include "itkMacro.h"
#include "vtkPoints.h"

/** Constructor */
template <class TPointSet>
DeformableAndAffineRegistrationMonitor<TPointSet>
::DeformableAndAffineRegistrationMonitor()
{
  // Variable for the conversion of the ITK transform into VTK Matrix
  this->Matrix     = vtkSmartPointer<vtkMatrix4x4>::New();

  this->RegistrationMode = AFFINE;
}

/** Destructor */
template <class TPointSet>
DeformableAndAffineRegistrationMonitor<TPointSet>
::~DeformableAndAffineRegistrationMonitor()
{
}


/** Set the objects to be observed: optimizer and transform */
template <class TPointSet>
void DeformableAndAffineRegistrationMonitor<TPointSet>
::ObserveData( const PointSetType * destinationPointSet )
{
  this->ObservedPointSet = destinationPointSet;
  this->RegistrationMode = DEFORMABLE;
}


/** Set the objects to be observed: optimizer and transform */
template <class TPointSet>
void DeformableAndAffineRegistrationMonitor<TPointSet>
::ObserveData( const TransformType * transform )
{
  this->ObservedTransform = transform;
  this->RegistrationMode = AFFINE;
}


/** Update the Visualization */
template <class TPointSet>
void DeformableAndAffineRegistrationMonitor<TPointSet>
::UpdateDataBeforeRendering()
{
  switch( this->RegistrationMode )
    {
    case AFFINE:
      {
      this->AffineUpdateDataBeforeRendering();
      break;
      }
    case DEFORMABLE:
      {
      this->DeformableUpdateDataBeforeRendering();
      break;
      }
    }
}


/** Update the Visualization */
template <class TPointSet>
void DeformableAndAffineRegistrationMonitor<TPointSet>
::DeformableUpdateDataBeforeRendering()
{
  typedef typename PointSetType::PointsContainer    PointsContainer;
  typedef typename PointsContainer::ConstIterator   PointsIterator;
  typedef typename PointSetType::PointType          PointType;

  const PointsContainer * displacedPoints = this->ObservedPointSet->GetPoints();

  if( !displacedPoints )
    {
    std::cerr << "displacedPoints = NULL" << std::endl;
    return;
    }

  vtkPoints * vtkpoints = this->GetFixedSurfacePoints();

  if( !vtkpoints )
    {
    std::cerr << "vtkpoints = NULL" << std::endl;
    return;
    }

  unsigned int numberOfPoints = 
    static_cast< unsigned int>( vtkpoints->GetNumberOfPoints() );

  if( numberOfPoints != displacedPoints->Size() )
    {
    std::cerr << "vtkPolyData numberOfPoints  = " << numberOfPoints << std::endl;
    std::cerr << "number of deformable points = " << displacedPoints->Size() << std::endl;
    itkGenericExceptionMacro("Fixed Mesh and Deformed Points have different size");
    } 

  PointsIterator pointItr = displacedPoints->Begin();
  PointsIterator pointEnd = displacedPoints->End();

  vtkFloatingPointType displacedPoint[3];

  int pointId = 0;

  while( pointItr != pointEnd )
    {
    const PointType & observedDisplacedPoint = pointItr.Value();

    displacedPoint[0] = observedDisplacedPoint[0];
    displacedPoint[1] = observedDisplacedPoint[1];
    displacedPoint[2] = observedDisplacedPoint[2];

    vtkpoints->SetPoint( pointId, displacedPoint );
    
    ++pointItr;
    ++pointId;
    }

  // Reset the Actor transform given that the surface has been resampled.
  this->Matrix->Identity();
  this->SetFixedActorMatrix( this->Matrix );

  this->MarkFixedSurfaceAsModified();
}


/** Update the Visualization */
template <class TPointSet>
void DeformableAndAffineRegistrationMonitor<TPointSet>
::AffineUpdateDataBeforeRendering()
{
  TransformType::MatrixType matrix = 
    this->ObservedTransform->GetMatrix();
 
  vnl_matrix_fixed<double,3,3> inverseMatrix = matrix.GetVnlMatrix();

  TransformType::OffsetType offset = 
    this->ObservedTransform->GetOffset();

  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      this->Matrix->SetElement(i,j, inverseMatrix.get(i,j));   
      }

    this->Matrix->SetElement( i, 3, -offset[i]);
    }

  this->SetFixedActorMatrix( this->Matrix );
}
