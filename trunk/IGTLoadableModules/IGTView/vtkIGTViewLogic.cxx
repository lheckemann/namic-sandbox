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

#include "vtkIGTViewLogic.h"

#include "vtkMatrix4x4.h"
#include "vtkMRMLCrosshairNode.h"


vtkCxxRevisionMacro(vtkIGTViewLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkIGTViewLogic);

//---------------------------------------------------------------------------
vtkIGTViewLogic::vtkIGTViewLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkIGTViewLogic::DataCallback);

  this->TransformNodeSelected = false;
  this->locatorTransformNode = NULL;
  //this->locatorPosition = NULL;
  this->Crosshair = NULL;
  this->CrosshairEnabled = false;
}


//---------------------------------------------------------------------------
vtkIGTViewLogic::~vtkIGTViewLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
  
  if(this->locatorTransformNode)
    {
      this->locatorTransformNode->Delete();
    }

  if(this->Crosshair)
    {
      this->Crosshair->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkIGTViewLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkIGTViewLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkIGTViewLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkIGTViewLogic *self = reinterpret_cast<vtkIGTViewLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkIGTViewLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkIGTViewLogic::UpdateAll()
{

}

void vtkIGTViewLogic::Reslice(vtkMRMLSliceNode* RedSlice, vtkMRMLSliceNode* YellowSlice, vtkMRMLSliceNode* GreenSlice)
{

  if(this->TransformNodeSelected)
    {
      vtkMatrix4x4* locatorMatrix = vtkMatrix4x4::New();
      this->locatorTransformNode->GetMatrixTransformToWorld(locatorMatrix);

      this->locatorPosition[0] = locatorMatrix->GetElement(0,3);
      this->locatorPosition[1] = locatorMatrix->GetElement(1,3);
      this->locatorPosition[2] = locatorMatrix->GetElement(2,3);

      locatorMatrix->Delete();

      // Reslice
      RedSlice->JumpSlice(this->locatorPosition[0],this->locatorPosition[1],this->locatorPosition[2]);
      YellowSlice->JumpSlice(this->locatorPosition[0],this->locatorPosition[1],this->locatorPosition[2]);
      GreenSlice->JumpSlice(this->locatorPosition[0],this->locatorPosition[1],this->locatorPosition[2]);

      UpdateCrosshair();
    }
}

void vtkIGTViewLogic::ObliqueOrientation(vtkMRMLSliceNode* slice, const char* ReslicingType)
{
  if(this->TransformNodeSelected)
    {
      vtkMatrix4x4* TransformationMatrix = vtkMatrix4x4::New();
      this->locatorTransformNode->GetMatrixTransformToWorld(TransformationMatrix);

      
      double tx = TransformationMatrix->GetElement(0,0);
      double ty = TransformationMatrix->GetElement(1,0);
      double tz = TransformationMatrix->GetElement(2,0);

      double mx = TransformationMatrix->GetElement(0,1);
      double my = TransformationMatrix->GetElement(1,1);
      double mz = TransformationMatrix->GetElement(2,1);

      double nx = TransformationMatrix->GetElement(0,2);
      double ny = TransformationMatrix->GetElement(1,2);
      double nz = TransformationMatrix->GetElement(2,2);

      double px = TransformationMatrix->GetElement(0,3);
      double py = TransformationMatrix->GetElement(1,3);
      double pz = TransformationMatrix->GetElement(2,3);
            
      if(!strcmp(ReslicingType, "Inplane0"))
  {
    slice->SetSliceToRASByNTP(-nx,-ny,-nz,-tx,-ty,-tz,px,py,pz,0);
  }
      else if (!strcmp(ReslicingType, "Inplane90"))
  {
    slice->SetSliceToRASByNTP(-tx,-ty,-tz,-mx,-my,-mz,px,py,pz,0);
  }
      else if (!strcmp(ReslicingType, "Probe's Eye"))
  {
    slice->SetSliceToRASByNTP(mx,my,mz,-tx,-ty,-tz,px,py,pz,0);
  }
      slice->UpdateMatrices();

      // Update Crosshair position
      this->locatorPosition[0] = px;
      this->locatorPosition[1] = py;
      this->locatorPosition[2] = pz;
      UpdateCrosshair();

      TransformationMatrix->Delete();
    }
}

void vtkIGTViewLogic::UpdateCrosshair()
{
  if(this->Crosshair && this->CrosshairEnabled)
    {
      // Set Crosshair
      this->Crosshair->SetCrosshairName("Locator");
      this->Crosshair->SetCrosshairBehavior(vtkMRMLCrosshairNode::Normal);
      this->Crosshair->SetCrosshairThickness(vtkMRMLCrosshairNode::Fine);
      this->Crosshair->SetNavigation(1);
      this->Crosshair->SetCrosshairMode(vtkMRMLCrosshairNode::ShowAll);
    }
  
  if(this->locatorPosition)
    {
      this->Crosshair->SetCrosshairRAS(this->locatorPosition[0], this->locatorPosition[1], this->locatorPosition[2]);
    }

}




