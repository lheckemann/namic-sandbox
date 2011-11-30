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
  this->Distractor1.SliderAnchor->SetElement(0,3,-45.16);
  this->Distractor1.SliderAnchor->SetElement(1,3,0.0);
  this->Distractor1.SliderAnchor->SetElement(2,3,-28.48);

  this->Distractor1.CylinderAnchor = vtkMatrix4x4::New();
  this->Distractor1.CylinderAnchor->SetElement(0,3,this->Distractor1.SliderAnchor->GetElement(0,3));
  this->Distractor1.CylinderAnchor->SetElement(1,3,1.98);
  this->Distractor1.CylinderAnchor->SetElement(2,3,this->Distractor1.SliderAnchor->GetElement(2,3));

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


  vtkTransform* SliderRotation = vtkTransform::New();
  SliderRotation->RotateY(value);

  vtkMatrix4x4* SliderTransformationMatrix = vtkMatrix4x4::New();
  SliderTransformationMatrix->Identity();

  vtkTransform* SliderInvertTranslation = vtkTransform::New();
  SliderInvertTranslation->Translate(-SliderCenter[0] - this->Distractor1.RotationCenter->GetElement(0,3),
                                     -SliderCenter[1] - this->Distractor1.RotationCenter->GetElement(1,3),
                                     -SliderCenter[2] - this->Distractor1.RotationCenter->GetElement(2,3));

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

  double PistonAnchor[3] = {this->Distractor1.RailAnchor->GetElement(0,3),this->Distractor1.RailAnchor->GetElement(1,3),this->Distractor1.RailAnchor->GetElement(2,3)};

  double newSliderCenterX = SliderTransformationMatrix->GetElement(0,0)*this->Distractor1.SliderAnchor->GetElement(0,3)+SliderTransformationMatrix->GetElement(0,1)*this->Distractor1.SliderAnchor->GetElement(1,3)+SliderTransformationMatrix->GetElement(0,2)*this->Distractor1.SliderAnchor->GetElement(2,3)+SliderTransformationMatrix->GetElement(0,3)*1;
  double newSliderCenterY = SliderTransformationMatrix->GetElement(2,0)*this->Distractor1.SliderAnchor->GetElement(0,3)+SliderTransformationMatrix->GetElement(2,1)*this->Distractor1.SliderAnchor->GetElement(1,3)+SliderTransformationMatrix->GetElement(2,2)*this->Distractor1.SliderAnchor->GetElement(2,3)+SliderTransformationMatrix->GetElement(2,3)*1;

  vtkTransform* PistonTranslation = vtkTransform::New();
  PistonTranslation->Translate(PistonAnchor[0],PistonAnchor[1],PistonAnchor[2]);

  double gamma = atan2((newSliderCenterY-this->Distractor1.RailAnchor->GetElement(2,3)),(newSliderCenterX-this->Distractor1.RailAnchor->GetElement(0,3)));
  double beta = atan2((this->Distractor1.SliderAnchor->GetElement(2,3)-this->Distractor1.RailAnchor->GetElement(2,3)),(this->Distractor1.SliderAnchor->GetElement(0,3)-this->Distractor1.RailAnchor->GetElement(0,3)));

  double PistonRotationAngle_deg = ((beta-gamma)*180/M_PI);

  vtkTransform* PistonRotation = vtkTransform::New();
  PistonRotation->RotateY(PistonRotationAngle_deg);

  vtkMatrix4x4* PistonTransformationMatrix = vtkMatrix4x4::New();
  PistonTransformationMatrix->Identity();

  vtkTransform* PistonInvertTranslation = vtkTransform::New();
  PistonInvertTranslation->Translate(-PistonAnchor[0],-PistonAnchor[1],-PistonAnchor[2]);
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

  double tx = newSliderCenterX - this->Distractor1.CylinderAnchor->GetElement(0,3);
  double ty = 0.0;
  double tz = newSliderCenterY - this->Distractor1.CylinderAnchor->GetElement(2,3);

  vtkTransform* CylinderInvertTranslation = vtkTransform::New();
  CylinderInvertTranslation->Translate(newSliderCenterX,this->Distractor1.CylinderAnchor->GetElement(1,3),newSliderCenterY);

  vtkTransform* CylinderTranslation = vtkTransform::New();
  CylinderTranslation->Translate(-newSliderCenterX,-this->Distractor1.CylinderAnchor->GetElement(1,3),-newSliderCenterY);

  vtkTransform* CylinderRotation = vtkTransform::New();
  CylinderRotation->RotateY(PistonRotationAngle_deg);

  vtkMatrix4x4* CylinderTransformationMatrix = vtkMatrix4x4::New();
  CylinderTransformationMatrix->Identity();

  vtkTransform* CylinderTransformation = vtkTransform::New();
  CylinderTransformation->Translate(tx,ty,tz);
  CylinderTransformation->PreMultiply();
  CylinderTransformation->Concatenate(CylinderTranslation);
  CylinderTransformation->PostMultiply();
  CylinderTransformation->Concatenate(CylinderRotation);
  CylinderTransformation->PostMultiply();
  CylinderTransformation->Concatenate(CylinderInvertTranslation);
  CylinderTransformation->GetMatrix(CylinderTransformationMatrix);

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

  CylinderInvertTranslation->Delete();
  CylinderTranslation->Delete();
  CylinderRotation->Delete();
  CylinderTransformationMatrix->Delete();
  CylinderTransformation->Delete();
}






