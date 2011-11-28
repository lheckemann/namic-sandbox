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

  // Initialize Distractor values here
  // TODO: Move to Distractor selection when will be available

  this->Distractor1.RotationCenter = vtkMatrix4x4::New();
  this->Distractor1.RotationCenter->SetElement(0,3,47.70);
  this->Distractor1.RotationCenter->SetElement(1,3,0.0);
  this->Distractor1.RotationCenter->SetElement(2,3,-15.0);

  this->Distractor1.RailAnchor = vtkMatrix4x4::New();
  this->Distractor1.RailAnchor->SetElement(0,3,-42.7);
  this->Distractor1.RailAnchor->SetElement(1,3,0.0);
  this->Distractor1.RailAnchor->SetElement(2,3,-21.0);

  this->Distractor1.SliderAnchor = vtkMatrix4x4::New();
  this->Distractor1.SliderAnchor->SetElement(0,3,-46.0);
  this->Distractor1.SliderAnchor->SetElement(1,3,0.0);
  this->Distractor1.SliderAnchor->SetElement(2,3,-29.3);

  this->Distractor1.CylinderAnchor = vtkMatrix4x4::New();
  this->Distractor1.CylinderAnchor->SetElement(0,3,-46.0);
  this->Distractor1.CylinderAnchor->SetElement(1,3,0.0);
  this->Distractor1.CylinderAnchor->SetElement(2,3,-29.3);


}


//---------------------------------------------------------------------------
vtkDistractorModelingLogic::~vtkDistractorModelingLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }


  if(&this->Distractor1)
    {
    this->Distractor1.RotationCenter->Delete();
    this->Distractor1.RailAnchor->Delete();
    this->Distractor1.SliderAnchor->Delete();
    this->Distractor1.CylinderAnchor->Delete();
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

  SliderTranslation->Translate(SliderCenter[0] + this->Distractor1.RotationCenter->GetElement(0,3),
                               SliderCenter[1] + this->Distractor1.RotationCenter->GetElement(1,3),
                               SliderCenter[2] + this->Distractor1.RotationCenter->GetElement(2,3));
/*
  SliderTranslation->Translate(this->Distractor1.SliderAnchor->GetElement(0,3) + this->Distractor1.RotationCenter->GetElement(0,3),
  this->Distractor1.SliderAnchor->GetElement(1,3) + this->Distractor1.RotationCenter->GetElement(1,3),
  this->Distractor1.SliderAnchor->GetElement(2,3) + this->Distractor1.RotationCenter->GetElement(2,3));
*/


  vtkTransform* SliderRotation = vtkTransform::New();
  SliderRotation->RotateY(value);

  vtkMatrix4x4* SliderTransformationMatrix = vtkMatrix4x4::New();
  SliderTransformationMatrix->Identity();

  vtkTransform* SliderInvertTranslation = vtkTransform::New();

  SliderInvertTranslation->Translate(-SliderCenter[0] - this->Distractor1.RotationCenter->GetElement(0,3),
                                     -SliderCenter[1] - this->Distractor1.RotationCenter->GetElement(1,3),
                                     -SliderCenter[2] - this->Distractor1.RotationCenter->GetElement(2,3));


/*
  SliderInvertTranslation->Translate(-this->Distractor1.SliderAnchor->GetElement(0,3) - this->Distractor1.RotationCenter->GetElement(0,3),
  -this->Distractor1.SliderAnchor->GetElement(1,3) - this->Distractor1.RotationCenter->GetElement(1,3),
  -this->Distractor1.SliderAnchor->GetElement(2,3) - this->Distractor1.RotationCenter->GetElement(2,3));
*/

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

  double newSliderCenterX = SliderTransformationMatrix->GetElement(0,0)*this->Distractor1.SliderAnchor->GetElement(0,3)+SliderTransformationMatrix->GetElement(0,1)*this->Distractor1.SliderAnchor->GetElement(1,3)+SliderTransformationMatrix->GetElement(0,2)*this->Distractor1.SliderAnchor->GetElement(2,3)+SliderTransformationMatrix->GetElement(0,3)*1;
  double newSliderCenterY = SliderTransformationMatrix->GetElement(2,0)*this->Distractor1.SliderAnchor->GetElement(0,3)+SliderTransformationMatrix->GetElement(2,1)*this->Distractor1.SliderAnchor->GetElement(1,3)+SliderTransformationMatrix->GetElement(2,2)*this->Distractor1.SliderAnchor->GetElement(2,3)+SliderTransformationMatrix->GetElement(2,3)*1;

  vtkTransform* PistonTranslation = vtkTransform::New();
  PistonTranslation->Translate(PistonCenter[0],PistonCenter[1],PistonCenter[2]);

  double dx = -newSliderCenterX+PistonCenter[0];
  double dy = -newSliderCenterY+PistonCenter[2];

  double PistonRotationAngle_rad = atan2(dx,dy);
  double PistonRotationAngle_deg = (PistonRotationAngle_rad*180/M_PI);
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

  double tx = newSliderCenterX - this->Distractor1.CylinderAnchor->GetElement(0,3);
  double ty = -this->Distractor1.CylinderAnchor->GetElement(1,3);
  double tz = newSliderCenterY - this->Distractor1.CylinderAnchor->GetElement(2,3);

  vtkTransform* CylinderAnchorTranslation = vtkTransform::New();
  CylinderAnchorTranslation->Translate(newSliderCenterX,0.0,newSliderCenterY);

  vtkTransform* CylinderTranslation = vtkTransform::New();
  CylinderTranslation->Translate(-newSliderCenterX,0.0,-newSliderCenterY);

  vtkTransform* CylinderRotation = vtkTransform::New();
  CylinderRotation->RotateY(PistonRotationAngle_deg);

  vtkMatrix4x4* CylinderTransformationMatrix = vtkMatrix4x4::New();
  CylinderTransformationMatrix->Identity();

  vtkTransform* CylinderInvertTranslation = vtkTransform::New();
  CylinderInvertTranslation->Translate(tx,ty,tz);
  CylinderInvertTranslation->PreMultiply();
  CylinderInvertTranslation->Concatenate(CylinderTranslation);
  CylinderInvertTranslation->PostMultiply();
  CylinderInvertTranslation->Concatenate(CylinderRotation);
  CylinderInvertTranslation->PostMultiply();
  CylinderInvertTranslation->Concatenate(CylinderAnchorTranslation);
  CylinderInvertTranslation->GetMatrix(CylinderTransformationMatrix);

  CylinderTransformationNode->SetAndObserveMatrixTransformToParent(CylinderTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Cylinder->GetParentTransformNode() && Cylinder->GetParentTransformNode()!=CylinderTransformationNode)
    {
    CylinderTransformationNode->SetAndObserveTransformNodeID(Cylinder->GetParentTransformNode()->GetID());
    }



  //--------------------
  // Delete

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
  CylinderAnchorTranslation->Delete();
}






