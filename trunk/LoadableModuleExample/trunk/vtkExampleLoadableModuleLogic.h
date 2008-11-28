/*=auto=========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkExampleLoadableModuleLogic.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkExampleLoadableModuleLogic_h
#define __vtkExampleLoadableModuleLogic_h

#include "vtkSlicerModuleLogic.h"
#include "vtkMRMLScene.h"

#include "vtkExampleLoadableModule.h"
#include "vtkMRMLExampleLoadableModuleNode.h"


class vtkITKGradientAnisotropicDiffusionImageFilter;

class VTK_EXAMPLELOADABLEMODULE_EXPORT vtkExampleLoadableModuleLogic : public vtkSlicerModuleLogic
{
  public:
  static vtkExampleLoadableModuleLogic *New();
  vtkTypeMacro(vtkExampleLoadableModuleLogic,vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  // TODO: do we need to observe MRML here?
  virtual void ProcessMrmlEvents ( vtkObject *caller, unsigned long event,
                                   void *callData ){};

  // Description: Get/Set MRML node storing parameter values
  vtkGetObjectMacro (ExampleLoadableModuleNode, vtkMRMLExampleLoadableModuleNode);
  void SetAndObserveExampleLoadableModuleNode(vtkMRMLExampleLoadableModuleNode *n) 
    {
    vtkSetAndObserveMRMLNodeMacro( this->ExampleLoadableModuleNode, n);
    }

  // The method that creates and runs VTK or ITK pipeline
  void Apply();
  
protected:
  vtkExampleLoadableModuleLogic();
  virtual ~vtkExampleLoadableModuleLogic();
  vtkExampleLoadableModuleLogic(const vtkExampleLoadableModuleLogic&);
  void operator=(const vtkExampleLoadableModuleLogic&);

  vtkMRMLExampleLoadableModuleNode* ExampleLoadableModuleNode;
  vtkITKGradientAnisotropicDiffusionImageFilter* GradientAnisotropicDiffusionImageFilter;

};

#endif

