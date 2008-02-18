/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: BSplineDeformableTransformToKernelTransformInverseCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2007/11/16 20:29:51 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __BSplineDeformableTransformToKernelTransformInverseCalculator_txx
#define __BSplineDeformableTransformToKernelTransformInverseCalculator_txx

#include "BSplineDeformableTransformToKernelTransformInverseCalculator.h"
#include "itkElasticBodySplineKernelTransform.h"

namespace itk
{

template< class TImage >
BSplineDeformableTransformToKernelTransformInverseCalculator< TImage >
::BSplineDeformableTransformToKernelTransformInverseCalculator() 
{
  this->SetDirectTransform( DirectTransformType::New() );
  this->m_InverseTransform = itk::ElasticBodySplineKernelTransform< double, 3 >::New();
}

template< class TImage >
BSplineDeformableTransformToKernelTransformInverseCalculator< TImage >
::~BSplineDeformableTransformToKernelTransformInverseCalculator()
{
}


template< class TImage >
void
BSplineDeformableTransformToKernelTransformInverseCalculator< TImage >
::ComputeInverseTransform()
{
try
  {   
  typedef InverseTransformType::PointSetType      PointSetType;
  typedef PointSetType::Pointer                   PointSetPointer;
  typedef PointSetType::PointType                 PointType;
  typedef PointSetType::PointsContainer           PointsContainer;

  PointSetPointer   sourceLandMarks = PointSetType::New();
  PointSetPointer   targetLandMarks = PointSetType::New();

  PointsContainer::Pointer sourceLandMarkPoints = sourceLandMarks->GetPoints();
  PointsContainer::Pointer targetLandMarkPoints = targetLandMarks->GetPoints();

  typedef PointSetType::PointIdentifier  PointIdType;

  PointIdType landmarkId = itk::NumericTraits< PointIdType >::Zero;

  typedef DirectTransformType::ImageType      GridImageType;
  typedef DirectTransformType::ImagePointer   GridImagePointer;

  GridImagePointer gridImage = this->m_DirectTransform->GetCoefficientImage()[0];

  typedef itk::ImageRegionIterator< GridImageType > GridRegionIterator;

  GridRegionIterator gitr( gridImage, gridImage->GetBufferedRegion() );

  gitr.GoToBegin();

  while( !gitr.IsAtEnd() )
    {
    //
    // Target points are in the space of the FixedImage
    // They are the nodes of the BSpline grid
    //
    PointType targetPoint;

    gridImage->TransformIndexToPhysicalPoint( gitr.GetIndex(), targetPoint );

    targetLandMarkPoints->InsertElement( landmarkId, targetPoint );

    //
    // Source points are in the space of the MovingImage
    // They are the images of the targetPoints under the DirectTransform
    //
    PointType sourcePoint = this->m_DirectTransform->TransformPoint( targetPoint );

    sourceLandMarkPoints->InsertElement( landmarkId, sourcePoint );


    ++landmarkId;
    ++gitr;
    }

  m_InverseTransform->SetSourceLandmarks( sourceLandMarks );
  m_InverseTransform->SetTargetLandmarks( targetLandMarks );
  m_InverseTransform->ComputeWMatrix();

  }
catch(ExceptionObject &e)
  {
  this->PrintError(e) ;
  }
catch(...)
  {
  this->PrintUncaughtError() ;
  }
}

template< class TImage >
void
BSplineDeformableTransformToKernelTransformInverseCalculator< TImage >
::PrintUncaughtError()
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in BSplineDeformableTransformToKernelTransformInverseCalculator:" << std::endl;
  std::cout << "unknown exception caught !!!" << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

template< class TImage >
void
BSplineDeformableTransformToKernelTransformInverseCalculator< TImage >
::PrintError(ExceptionObject &e)
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in BSplineDeformableTransformToKernelTransformInverseCalculator:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

} // end namespace itk

#endif //__BSplineDeformableTransformToKernelTransformInverseCalculator_txx
