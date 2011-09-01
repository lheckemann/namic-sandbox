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

#include "vtkMRMLScalarVolumeNode.h"
#include "vtkCollection.h"
#include "vtkKWScale.h"

#include "vtkUltrasound4DLogic.h"

vtkCxxRevisionMacro(vtkUltrasound4DLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkUltrasound4DLogic);

//---------------------------------------------------------------------------
vtkUltrasound4DLogic::vtkUltrasound4DLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkUltrasound4DLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkUltrasound4DLogic::~vtkUltrasound4DLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkUltrasound4DLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkUltrasound4DLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkUltrasound4DLogic::DataCallback(vtkObject *caller,
                                        unsigned long eid, void *clientData, void *callData)
{
  vtkUltrasound4DLogic *self = reinterpret_cast<vtkUltrasound4DLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkUltrasound4DLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUltrasound4DLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
void vtkUltrasound4DLogic::CopyVolume(vtkMRMLScalarVolumeNode *destinationNode, vtkMRMLScalarVolumeNode *sourceNode)
{
  if(sourceNode->GetImageData()->GetActualMemorySize() == destinationNode->GetImageData()->GetActualMemorySize())
    {
    memcpy(destinationNode->GetImageData()->GetScalarPointer(),
           sourceNode->GetImageData()->GetScalarPointer(),
           destinationNode->GetImageData()->GetActualMemorySize()*1024);

    destinationNode->GetImageData()->Update();
    destinationNode->GetImageData()->Modified();

    destinationNode->SetAndObserveImageData(destinationNode->GetImageData());
    destinationNode->Modified();
    }
}

//---------------------------------------------------------------------------
void vtkUltrasound4DLogic::PlayVolumes(vtkMRMLScalarVolumeNode *destinationNode, vtkCollection *ListOfNodes, vtkKWScale *scale)
{
// TODO: Thread (with Mutex ?)

}



