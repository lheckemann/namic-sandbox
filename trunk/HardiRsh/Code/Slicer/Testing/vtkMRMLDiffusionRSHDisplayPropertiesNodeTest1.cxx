/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkMRMLDiffusionRSHDisplayPropertiesNode.h"
#include "vtkAbstractTransform.h"
#include "vtkSmartPointer.h"

#include <stdlib.h>
#include <iostream>

#include "slicerTestingMacros.h"

class vtkMRMLDiffusionRSHDisplayPropertiesNodeTestHelper1 : public vtkMRMLDiffusionRSHDisplayPropertiesNode
{
public:
  // Provide a concrete New.
  static vtkMRMLDiffusionRSHDisplayPropertiesNodeTestHelper1 *New(){return new vtkMRMLDiffusionRSHDisplayPropertiesNodeTestHelper1;};
  virtual vtkMRMLNode* CreateNodeInstance()
    {
    return new vtkMRMLDiffusionRSHDisplayPropertiesNodeTestHelper1;
    }
  virtual const char* GetNodeTagName()
    {
    return "vtkMRMLDiffusionRSHDisplayPropertiesNodeTestHelper1";
    }

  virtual bool CanApplyNonLinearTransforms() { return false; }
  virtual void ApplyTransform(vtkAbstractTransform* transform) { return; }

  virtual int ReadData(vtkMRMLNode *refNode) { return 0; }
  virtual int WriteData(vtkMRMLNode *refNode) { return 0; }
};
 
int vtkMRMLDiffusionRSHDisplayPropertiesNodeTest1(int , char * [] )
{
  vtkSmartPointer< vtkMRMLDiffusionRSHDisplayPropertiesNodeTestHelper1 > node1 = vtkSmartPointer< vtkMRMLDiffusionRSHDisplayPropertiesNodeTestHelper1 >::New();

  EXERCISE_BASIC_OBJECT_METHODS( node1 );

  return EXIT_SUCCESS;
}
