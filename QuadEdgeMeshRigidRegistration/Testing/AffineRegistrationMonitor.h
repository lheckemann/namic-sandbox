/*=========================================================================

  Program:   Surface Registration Program
  Module:    $RCSfile: AffineRegistrationMonitor.h,v $

  Copyright (c) Kitware Inc. 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __AffineRegistrationMonitor_h
#define __AffineRegistrationMonitor_h

#include "RegistrationMonitor.h"
#include "itkMatrixOffsetTransformBase.h"
#include "itkOptimizer.h"

#include "vtkMatrix4x4.h"


/** \class AffineRegistrationMonitor 
 *  This class provides a VTK visualization pipeline configured for monitoring
 *  the progress of a registration process. 
 */
class AffineRegistrationMonitor : public RegistrationMonitor
{
public:
  
  typedef AffineRegistrationMonitor  Self;
  typedef RegistrationMonitor        Superclass;

  typedef itk::Optimizer       OptimizerType;

  typedef itk::MatrixOffsetTransformBase< double, 3, 3 > TransformType;

  AffineRegistrationMonitor();
  ~AffineRegistrationMonitor();

  void Observe( OptimizerType * optimizer, TransformType * transform );

protected:

  // These methods will only be called by the Observer
  virtual void Update();
  virtual void StartVisualization();

private:
  
  vtkSmartPointer< vtkMatrix4x4 >        Matrix;

  typedef itk::SimpleMemberCommand< Self >  ObserverType;

  ObserverType::Pointer           IterationObserver;
  ObserverType::Pointer           StartObserver;

  OptimizerType::Pointer          ObservedOptimizer;
  TransformType::Pointer          ObservedTransform;
};

#endif
