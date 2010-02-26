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

#include "AffineRegistrationMonitor.h"

#include "vtkMatrix4x4.h"

/** Constructor */
AffineRegistrationMonitor::AffineRegistrationMonitor()
{
  // Variable for the conversion of the ITK transform into VTK Matrix
  this->Matrix     = vtkSmartPointer<vtkMatrix4x4>::New();

  // ITK Objects, does not require to call Delete()
  this->StartObserver       = ObserverType::New();
  this->IterationObserver   = ObserverType::New();

  this->StartObserver->SetCallbackFunction( 
    this, & Self::StartVisualization );

  this->IterationObserver->SetCallbackFunction( 
    this, & Self::Update );
}


/** Destructor */
AffineRegistrationMonitor::~AffineRegistrationMonitor()
{
}


/** Set the objects to be observed: optimizer and transform */
void AffineRegistrationMonitor
::Observe( OptimizerType * optimizer, TransformType * transform )
{
  this->ObservedOptimizer = optimizer;
  this->ObservedTransform = transform;

  this->ObservedOptimizer->AddObserver( 
    itk::StartEvent(), this->StartObserver     );

  this->ObservedOptimizer->AddObserver( 
    itk::IterationEvent(), this->IterationObserver );
}


/** Update the Visualization */
void AffineRegistrationMonitor::UpdateDataBeforeRendering()
{
  TransformType::MatrixType matrix = 
    this->ObservedTransform->GetMatrix();
 
  TransformType::OffsetType offset = 
    this->ObservedTransform->GetOffset();

  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      this->Matrix->SetElement(i,j,
        matrix.GetVnlMatrix().get(i,j));   
      }

    this->Matrix->SetElement( i, 3, offset[i]);
    }

  this->SetMovingActorMatrix( this->Matrix );
}
