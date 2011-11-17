/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkDistractorModelingLogic.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMatrix4x4.h"

vtkCxxRevisionMacro(vtkDistractorModelingLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkDistractorModelingLogic);

//---------------------------------------------------------------------------
vtkDistractorModelingLogic::vtkDistractorModelingLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkDistractorModelingLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkDistractorModelingLogic::~vtkDistractorModelingLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkDistractorModelingLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::DataCallback(vtkObject *caller,
                                              unsigned long eid, void *clientData, void *callData)
{
  vtkDistractorModelingLogic *self = reinterpret_cast<vtkDistractorModelingLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkDistractorModelingLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::MoveSlider(vtkMRMLModelNode* Slider, double value, vtkMRMLLinearTransformNode* SliderTransformationNode)
{
  double SliderCenter[3];
  Slider->GetPolyData()->GetCenter(SliderCenter);

  vtkTransform* translation = vtkTransform::New();
  translation->Translate(SliderCenter[0]+47.70,SliderCenter[1],SliderCenter[2]-15);

  vtkTransform* rotation = vtkTransform::New();
  rotation->RotateY(value);

  vtkMatrix4x4* transformationMatrix = vtkMatrix4x4::New();
  transformationMatrix->Identity();

  vtkTransform* invertTranslation = vtkTransform::New();
  invertTranslation->Translate(-SliderCenter[0]-47.70, -SliderCenter[1], -SliderCenter[2]+15); // 5cm curvature, z = 15, so (x = sqrt(r^2 - z^2) with r in mm => x = -47.70)
  invertTranslation->PostMultiply();
  invertTranslation->Concatenate(rotation);
  invertTranslation->PostMultiply();
  invertTranslation->Concatenate(translation);
  invertTranslation->GetMatrix(transformationMatrix);

  SliderTransformationNode->SetAndObserveMatrixTransformToParent(transformationMatrix);

  translation->Delete();
  rotation->Delete();
  invertTranslation->Delete();
  transformationMatrix->Delete();
}






