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

#include <math.h>

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
void vtkDistractorModelingLogic::MoveSlider(vtkMRMLModelNode* Slider, vtkMRMLModelNode* Piston, vtkMRMLModelNode* Cylinder, double value, vtkMRMLLinearTransformNode* SliderTransformationNode, vtkMRMLLinearTransformNode* PistonTransformationNode, vtkMRMLLinearTransformNode* CylinderTransformationNode)
{
  //--------------------
  // Slider

  double SliderCenter[3];
  Slider->GetPolyData()->GetCenter(SliderCenter);

  vtkTransform* SliderTranslation = vtkTransform::New();
  SliderTranslation->Translate(SliderCenter[0]+47.70,SliderCenter[1],SliderCenter[2]-15);

  vtkTransform* SliderRotation = vtkTransform::New();
  SliderRotation->RotateY(value);

  vtkMatrix4x4* SliderTransformationMatrix = vtkMatrix4x4::New();
  SliderTransformationMatrix->Identity();

  vtkTransform* SliderInvertTranslation = vtkTransform::New();
  SliderInvertTranslation->Translate(-SliderCenter[0]-47.70, -SliderCenter[1], -SliderCenter[2]+15); // 5cm curvature, z = 15, so (x = sqrt(r^2 - z^2) with r in mm => x = -47.70)
  SliderInvertTranslation->PostMultiply();
  SliderInvertTranslation->Concatenate(SliderRotation);
  SliderInvertTranslation->PostMultiply();
  SliderInvertTranslation->Concatenate(SliderTranslation);
  SliderInvertTranslation->GetMatrix(SliderTransformationMatrix);

  SliderTransformationNode->SetAndObserveMatrixTransformToParent(SliderTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Slider->GetParentTransformNode() && Slider->GetParentTransformNode()!=SliderTransformationNode)
    {
    SliderTransformationNode->SetAndObserveTransformNodeID(Slider->GetParentTransformNode()->GetID());
    }

  //--------------------
  // Piston

  double PistonCenter[3] = {-42.7,0,-21};

  double newSliderCenterX = SliderTransformationMatrix->GetElement(0,0)*SliderCenter[0]+SliderTransformationMatrix->GetElement(0,1)*SliderCenter[1]+SliderTransformationMatrix->GetElement(0,2)*SliderCenter[2]+SliderTransformationMatrix->GetElement(0,3)*1;
  double newSliderCenterY = SliderTransformationMatrix->GetElement(2,0)*SliderCenter[0]+SliderTransformationMatrix->GetElement(2,1)*SliderCenter[1]+SliderTransformationMatrix->GetElement(2,2)*SliderCenter[2]+SliderTransformationMatrix->GetElement(2,3)*1;

  vtkTransform* PistonTranslation = vtkTransform::New();
  PistonTranslation->Translate(PistonCenter[0],PistonCenter[1],PistonCenter[2]);

  double dx = -newSliderCenterX+PistonCenter[0];
  double dy = -newSliderCenterY+PistonCenter[2];

  double PistonRotationAngle_rad = atan2(dx,dy);
  double PistonRotationAngle_deg = PistonRotationAngle_rad*180/M_PI;
  vtkTransform* PistonRotation = vtkTransform::New();
  PistonRotation->RotateY(PistonRotationAngle_deg);

  vtkMatrix4x4* PistonTransformationMatrix = vtkMatrix4x4::New();
  PistonTransformationMatrix->Identity();

  vtkTransform* PistonInvertTranslation = vtkTransform::New();
  PistonInvertTranslation->Translate(-PistonCenter[0],-PistonCenter[1],-PistonCenter[2]);
  PistonInvertTranslation->PostMultiply();
  PistonInvertTranslation->Concatenate(PistonRotation);
  PistonInvertTranslation->PostMultiply();
  PistonInvertTranslation->Concatenate(PistonTranslation);
  PistonInvertTranslation->GetMatrix(PistonTransformationMatrix);

  PistonTransformationNode->SetAndObserveMatrixTransformToParent(PistonTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Piston->GetParentTransformNode() && Piston->GetParentTransformNode()!=PistonTransformationNode)
    {
    PistonTransformationNode->SetAndObserveTransformNodeID(Piston->GetParentTransformNode()->GetID());
    }


  //--------------------
  // Cylinder

  double CylinderCenter[3];
  Cylinder->GetPolyData()->GetCenter(CylinderCenter);

  vtkTransform* CylinderTranslation = vtkTransform::New();
  CylinderTranslation->Translate(newSliderCenterX,0.5,newSliderCenterY);

  vtkTransform* CylinderRotation = vtkTransform::New();
  CylinderRotation->RotateY(PistonRotationAngle_deg);

  vtkMatrix4x4* CylinderTransformationMatrix = vtkMatrix4x4::New();
  CylinderTransformationMatrix->Identity();

  vtkTransform* CylinderInvertTranslation = vtkTransform::New();
  CylinderInvertTranslation->Translate(-newSliderCenterX,-0.5,-newSliderCenterY);
  CylinderInvertTranslation->PostMultiply();
  CylinderInvertTranslation->Concatenate(CylinderRotation);
  CylinderInvertTranslation->PostMultiply();
  CylinderInvertTranslation->Concatenate(CylinderTranslation);
  CylinderInvertTranslation->GetMatrix(CylinderTransformationMatrix);

  CylinderTransformationNode->SetAndObserveMatrixTransformToParent(CylinderTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Cylinder->GetParentTransformNode() && Cylinder->GetParentTransformNode()!=CylinderTransformationNode)
    {
    CylinderTransformationNode->SetAndObserveTransformNodeID(Cylinder->GetParentTransformNode()->GetID());
    }




  SliderTranslation->Delete();
  SliderRotation->Delete();
  SliderInvertTranslation->Delete();
  SliderTransformationMatrix->Delete();

  PistonTranslation->Delete();
  PistonRotation->Delete();
  PistonInvertTranslation->Delete();
  PistonTransformationMatrix->Delete();

  CylinderTranslation->Delete();
  CylinderRotation->Delete();
  CylinderInvertTranslation->Delete();
  CylinderTransformationMatrix->Delete();
}






