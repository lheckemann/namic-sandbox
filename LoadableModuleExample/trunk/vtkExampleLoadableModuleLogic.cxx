/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkExampleLoadableModuleLogic.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkExampleLoadableModuleLogic.h"
#include "vtkITKGradientAnisotropicDiffusionImageFilter.h"
#include "vtkExampleLoadableModule.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"

vtkExampleLoadableModuleLogic* vtkExampleLoadableModuleLogic::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkExampleLoadableModuleLogic");
  if(ret)
    {
      return (vtkExampleLoadableModuleLogic*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkExampleLoadableModuleLogic;
}


//----------------------------------------------------------------------------
vtkExampleLoadableModuleLogic::vtkExampleLoadableModuleLogic()
{
  this->ExampleLoadableModuleNode = NULL;
}

//----------------------------------------------------------------------------
vtkExampleLoadableModuleLogic::~vtkExampleLoadableModuleLogic()
{
  vtkSetMRMLNodeMacro(this->ExampleLoadableModuleNode, NULL);
}

//----------------------------------------------------------------------------
void vtkExampleLoadableModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  
}

void vtkExampleLoadableModuleLogic::Apply()
{

  // check if MRML node is present 
  if (this->ExampleLoadableModuleNode == NULL)
    {
    vtkErrorMacro("No input ExampleLoadableModuleNode found");
    return;
    }
  
  // find input volume
    vtkMRMLScalarVolumeNode *inVolume = vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->ExampleLoadableModuleNode->GetInputVolumeRef()));
  if (inVolume == NULL)
    {
    vtkErrorMacro("No input volume found");
    return;
    }
  
  // find output volume
  vtkMRMLScalarVolumeNode *outVolume =  vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->ExampleLoadableModuleNode->GetOutputVolumeRef()));
  if (outVolume == NULL)
    {
    vtkErrorMacro("No output volume found with id= " << this->ExampleLoadableModuleNode->GetOutputVolumeRef());
    return;
    }

  // copy RASToIJK matrix, and other attributes from input to output
  std::string name (outVolume->GetName());
  std::string id (outVolume->GetID());

  outVolume->CopyOrientation(inVolume);
  outVolume->SetAndObserveTransformNodeID(inVolume->GetTransformNodeID());

  outVolume->SetName(name.c_str());
  //outVolume->SetID(id.c_str());

  // create filter
  //vtkITKGradientAnisotropicDiffusionImageFilter* filter = vtkITKGradientAnisotropicDiffusionImageFilter::New();
  this->GradientAnisotropicDiffusionImageFilter = vtkITKGradientAnisotropicDiffusionImageFilter::New();

  // set filter input and parameters
  this->GradientAnisotropicDiffusionImageFilter->SetInput(inVolume->GetImageData());

  this->GradientAnisotropicDiffusionImageFilter->SetConductanceParameter(this->ExampleLoadableModuleNode->GetConductance());
  this->GradientAnisotropicDiffusionImageFilter->SetNumberOfIterations(this->ExampleLoadableModuleNode->GetNumberOfIterations());
  this->GradientAnisotropicDiffusionImageFilter->SetTimeStep(this->ExampleLoadableModuleNode->GetTimeStep()); 

  // run the filter
  this->GradientAnisotropicDiffusionImageFilter->Update();

  // set ouput of the filter to VolumeNode's ImageData
  // TODO FIX the bug of the image is deallocated unless we do DeepCopy
  vtkImageData* image = vtkImageData::New(); 
  image->DeepCopy( this->GradientAnisotropicDiffusionImageFilter->GetOutput() );
  outVolume->SetAndObserveImageData(image);
  image->Delete();
  outVolume->SetModifiedSinceRead(1);

  //outVolume->SetImageData(this->GradientAnisotropicDiffusionImageFilter->GetOutput());

  // delete the filter
  this->GradientAnisotropicDiffusionImageFilter->Delete();
}
