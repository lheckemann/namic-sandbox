/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkTRProstateBiopsy.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkTRProstateBiopsyMath.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkSlicerColorLogic.h"
#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"
#include "vtkTRProstateBiopsyUSBOpticalEncoder.h"

#include <vtksys/stl/string>
#include <vtksys/SystemTools.hxx>

vtkCxxRevisionMacro(vtkTRProstateBiopsyLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkTRProstateBiopsyLogic);

vtkCxxSetObjectMacro(vtkTRProstateBiopsyLogic, CalibrationFiducialListNode,
                     vtkMRMLFiducialListNode);
vtkCxxSetObjectMacro(vtkTRProstateBiopsyLogic, TargetFiducialListNode,
                     vtkMRMLFiducialListNode);

vtkCxxSetObjectMacro(vtkTRProstateBiopsyLogic, CalibrationVolumeNode,
                     vtkMRMLScalarVolumeNode);
vtkCxxSetObjectMacro(vtkTRProstateBiopsyLogic, TargetingVolumeNode,
                     vtkMRMLScalarVolumeNode);
vtkCxxSetObjectMacro(vtkTRProstateBiopsyLogic, VerificationVolumeNode,
                     vtkMRMLScalarVolumeNode);

//---------------------------------------------------------------------------
const int vtkTRProstateBiopsyLogic::PhaseTransitionMatrix[vtkTRProstateBiopsyLogic::NumPhases][vtkTRProstateBiopsyLogic::NumPhases] =
  {
      /*     next workphase     */
      /*    */ /* Cl  Tg  Ve*/
      /* Cl */ {  1,  1,  1  },
      /* Tg */ {  1,  1,  1  },
      /* Ve */ {  1,  1,  1  },
  };

//---------------------------------------------------------------------------
const char *vtkTRProstateBiopsyLogic::WorkPhaseKey[vtkTRProstateBiopsyLogic::NumPhases] =
  {
  /* Cl */ TRPB_CALIBRATION ,
  /* Tg */ TRPB_TARGETING   ,
  /* Ve */ TRPB_VERIFICATION,
  };

//---------------------------------------------------------------------------
vtkTRProstateBiopsyLogic::vtkTRProstateBiopsyLogic()
{
  this->CurrentPhase         = Calibration;
  this->PrevPhase            = Calibration;
  this->PhaseComplete        = false;
  this->PhaseTransitionCheck = true;

  this->CalibrationFiducialListNodeID = NULL;
  this->CalibrationFiducialListNode   = NULL;
  
  this->TargetFiducialListNodeID = NULL;
  this->TargetFiducialListNode   = NULL;
  
  this->CalibrationVolumeNode = NULL;
  this->TargetingVolumeNode = NULL;
  this->VerificationVolumeNode = NULL;

  this->USBEncoder = NULL;

  //this->CalibrationSliceNodeXML = NULL;

  // Timer Handling
  //this->DataCallbackCommand = vtkCallbackCommand::New();
  //this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  //this->DataCallbackCommand->SetCallback(vtkTRProstateBiopsyLogic::DataCallback);

  


}


//---------------------------------------------------------------------------
vtkTRProstateBiopsyLogic::~vtkTRProstateBiopsyLogic()
{
  //if (this->DataCallbackCommand)
  //  {
  //  this->DataCallbackCommand->Delete();
  //  }


  this->SetCalibrationFiducialListNodeID(NULL);
  this->SetTargetFiducialListNodeID(NULL);

  vtkSetMRMLNodeMacro(this->CalibrationFiducialListNode, NULL);
  vtkSetMRMLNodeMacro(this->TargetFiducialListNode, NULL);

  vtkSetMRMLNodeMacro(this->CalibrationVolumeNode, NULL);
  vtkSetMRMLNodeMacro(this->TargetingVolumeNode, NULL);
  vtkSetMRMLNodeMacro(this->VerificationVolumeNode, NULL);
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkTRProstateBiopsyLogic:             " << this->GetClassName() << "\n";
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::DataCallback(vtkObject *caller, 
                                            unsigned long eid,
                                            void *clientData,
                                            void *callData)
{
  vtkTRProstateBiopsyLogic *self =
          reinterpret_cast<vtkTRProstateBiopsyLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkTRProstateBiopsyLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::UpdateAll()
{
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::SetSliceViewFromVolume(
  vtkSlicerApplication *app,
  vtkMRMLVolumeNode *volumeNode)
{
  if (!volumeNode)
    {
    return;
    }

  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  vtkMatrix4x4 *permutationMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4 *rotationMatrix = vtkMatrix4x4::New();

  volumeNode->GetIJKToRASDirectionMatrix(matrix);
  slicerCerr("matrix");
  slicerCerr("   " << matrix->GetElement(0,0) <<
             "   " << matrix->GetElement(0,1) <<
             "   " << matrix->GetElement(0,2));
  slicerCerr("   " << matrix->GetElement(1,0) <<
             "   " << matrix->GetElement(1,1) <<
             "   " << matrix->GetElement(1,2));
  slicerCerr("   " << matrix->GetElement(2,0) <<
             "   " << matrix->GetElement(2,1) <<
             "   " << matrix->GetElement(2,2));

  int permutation[3];
  int flip[3];
  vtkTRProstateBiopsyMath::ComputePermutationFromOrientation(
    matrix, permutation, flip);

  slicerCerr("permutation " << permutation[0] << " " <<
             permutation[1] << " " << permutation[2]);
  slicerCerr("flip " << flip[0] << " " <<
             flip[1] << " " << flip[2]);

  permutationMatrix->SetElement(0,0,0);
  permutationMatrix->SetElement(1,1,0);
  permutationMatrix->SetElement(2,2,0);

  permutationMatrix->SetElement(0, permutation[0],
                     (flip[permutation[0]] ? -1 : 1));
  permutationMatrix->SetElement(1, permutation[1],
                     (flip[permutation[1]] ? -1 : 1));
  permutationMatrix->SetElement(2, permutation[2],
                     (flip[permutation[2]] ? -1 : 1));

  slicerCerr("permutationMatrix");
  slicerCerr("   " << permutationMatrix->GetElement(0,0) <<
             "   " << permutationMatrix->GetElement(0,1) <<
             "   " << permutationMatrix->GetElement(0,2));
  slicerCerr("   " << permutationMatrix->GetElement(1,0) <<
             "   " << permutationMatrix->GetElement(1,1) <<
             "   " << permutationMatrix->GetElement(1,2));
  slicerCerr("   " << permutationMatrix->GetElement(2,0) <<
             "   " << permutationMatrix->GetElement(2,1) <<
             "   " << permutationMatrix->GetElement(2,2));

  permutationMatrix->Invert();
  vtkMatrix4x4::Multiply4x4(matrix, permutationMatrix, rotationMatrix); 

  vtkSlicerApplicationLogic *appLogic =
    app->GetApplicationGUI()->GetApplicationLogic();

  
  // Set the slice views to match the volume slice orientation
  for (int i = 0; i < 3; i++)
    {
    static const char *panes[3] = { "Red", "Yellow", "Green" };

    vtkMatrix4x4 *newMatrix = vtkMatrix4x4::New();

    vtkSlicerSliceLogic *slice = appLogic->GetSliceLogic(
      const_cast<char *>(panes[i]));
    
    vtkMRMLSliceNode *sliceNode = slice->GetSliceNode();

    // Need to find window center and rotate around that

    // Default matrix orientation for slice
    newMatrix->SetElement(0, 0, 0.0);
    newMatrix->SetElement(1, 1, 0.0);
    newMatrix->SetElement(2, 2, 0.0);
    if (i == 0)
      {
      newMatrix->SetElement(0, 0, -1.0);
      newMatrix->SetElement(1, 1, 1.0);
      newMatrix->SetElement(2, 2, 1.0);
      }
    else if (i == 1)
      {
      newMatrix->SetElement(1, 0, -1.0);
      newMatrix->SetElement(2, 1, 1.0);
      newMatrix->SetElement(0, 2, 1.0);
      }
    else if (i == 2)
      {
      newMatrix->SetElement(0, 0, -1.0);
      newMatrix->SetElement(2, 1, 1.0);
      newMatrix->SetElement(1, 2, 1.0);
      }

    // Next, set the orientation to match the volume
    sliceNode->SetOrientationToReformat();
    vtkMatrix4x4::Multiply4x4(rotationMatrix, newMatrix, newMatrix);
    sliceNode->SetSliceToRAS(newMatrix);
    sliceNode->UpdateMatrices();
    newMatrix->Delete();
    }

  matrix->Delete();
  permutationMatrix->Delete();
  rotationMatrix->Delete();
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkTRProstateBiopsyLogic::AddCalibrationVolume(
  vtkSlicerApplication* app, const char *fileName)
{
  vtksys_stl::string volumeNameString =
    vtksys::SystemTools::GetFilenameName(fileName);

  vtkMRMLScalarVolumeNode *volumeNode =
    this->AddArchetypeVolume(app, fileName, volumeNameString.c_str());

  vtkSetMRMLNodeMacro(this->CalibrationVolumeNode, volumeNode );
  this->Modified();

  this->SetSliceViewFromVolume(app, volumeNode);

  return volumeNode;
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkTRProstateBiopsyLogic::AddTargetingVolume(
  vtkSlicerApplication* app, const char *fileName)
{
  vtksys_stl::string volumeNameString =
    vtksys::SystemTools::GetFilenameName(fileName);

  vtkMRMLScalarVolumeNode *volumeNode =
    this->AddArchetypeVolume(app, fileName, volumeNameString.c_str());

  vtkSetMRMLNodeMacro(this->TargetingVolumeNode, volumeNode );
  this->Modified();

  this->SetSliceViewFromVolume(app, volumeNode);

  return volumeNode;
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkTRProstateBiopsyLogic::AddVerificationVolume(
  vtkSlicerApplication* app, const char *fileName)
{
  vtksys_stl::string volumeNameString =
    vtksys::SystemTools::GetFilenameName(fileName);

  vtkMRMLScalarVolumeNode *volumeNode =
    this->AddArchetypeVolume(app, fileName, volumeNameString.c_str());

  vtkSetMRMLNodeMacro(this->VerificationVolumeNode, volumeNode );
  this->Modified();

  this->SetSliceViewFromVolume(app, volumeNode);

  return volumeNode;
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkTRProstateBiopsyLogic::AddArchetypeVolume(
  vtkSlicerApplication* app, const char* fileName, const char *volumeName)
{
  // Crete the necessary MRML nodes
  vtkMRMLScalarVolumeDisplayNode *displayNode =
    vtkMRMLScalarVolumeDisplayNode::New();
  vtkMRMLScalarVolumeNode *scalarNode =
    vtkMRMLScalarVolumeNode::New();
  vtkMRMLVolumeArchetypeStorageNode *storageNode =
    vtkMRMLVolumeArchetypeStorageNode::New();

  // Do not attemp to automatically window-level the volume
  displayNode->SetAutoWindowLevel(false);
  displayNode->SetInterpolate(true);
  
  // check to see if can read this type of file
  storageNode->SetFileName(fileName);
  if (storageNode->SupportedFileType(fileName) == 0)
    {
    vtkErrorMacro("AddArchetypeVolume: can't read this kind of file: "
                  << fileName);
    return 0;
    }
  storageNode->SetCenterImage(false);
  storageNode->SetSingleFile(false);
  storageNode->SetUseOrientationFromFile(true);
  storageNode->AddObserver(vtkCommand::ProgressEvent,
                           this->LogicCallbackCommand);

  scalarNode->SetName(volumeName);
  scalarNode->SetLabelMap(false);
  
  this->GetMRMLScene()->SaveStateForUndo();  

  scalarNode->SetScene(this->GetMRMLScene());
  displayNode->SetScene(this->GetMRMLScene());

  vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
  displayNode->SetAndObserveColorNodeID(
    colorLogic->GetDefaultVolumeColorNodeID());
  colorLogic->Delete();

  vtkDebugMacro("LoadArchetypeVolume: adding storage node to the scene");
  storageNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(storageNode);
  vtkDebugMacro("LoadArchetypeVolume: adding display node to the scene");
  this->GetMRMLScene()->AddNode(displayNode);
  
  scalarNode->SetAndObserveStorageNodeID(storageNode->GetID());
  scalarNode->SetAndObserveDisplayNodeID(displayNode->GetID());

  vtkDebugMacro("LoadArchetypeVolume: adding scalar node to the scene");
  scalarNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(scalarNode);

  // read the volume into the node
  vtkDebugMacro("AddArchetypeVolume: about to read data into scalar node "
                << scalarNode->GetName());
  if (this->GetDebug())
    {
    storageNode->DebugOn();
    }
  storageNode->ReadData(scalarNode);
  vtkDebugMacro("AddArchetypeVolume: finished reading data into scalarNode");

  storageNode->RemoveObservers(vtkCommand::ProgressEvent,
                               this->LogicCallbackCommand);
 
  scalarNode->Delete();
  storageNode->Delete();
  displayNode->Delete();

  return scalarNode;
}

//---------------------------------------------------------------------------
int vtkTRProstateBiopsyLogic::SwitchWorkPhase(int newwp)
{
  if (this->IsPhaseTransitable(newwp))
    {
    this->PrevPhase     = this->CurrentPhase;
    this->CurrentPhase  = newwp;
    this->PhaseComplete = false;
    
    return 1;
    }

  return 0;
}

//---------------------------------------------------------------------------
int vtkTRProstateBiopsyLogic::IsPhaseTransitable(int nextwp)
{
  if (nextwp < 0 || nextwp > NumPhases)
    {
    return 0;
    }
  
  if (this->PhaseTransitionCheck == 0)
    {
    return 1;
    }
  
  if (this->PhaseComplete)
    {
    return this->PhaseTransitionMatrix[this->CurrentPhase][nextwp];
    }
  else
    {
    return this->PhaseTransitionMatrix[this->PrevPhase][nextwp];
    }

  return 0;
}


//---------------------------------------------------------------------------
int vtkTRProstateBiopsyLogic::WorkPhaseStringToID(const char* string)
{
  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i ++)
    {
    if (strcmp(vtkTRProstateBiopsyLogic::WorkPhaseKey[i], string) == 0)
      {
      return i;
      }
    }
  return -1; // Nothing found.
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::InitializeOpticalEncoder()
{
 
  // Question, how to stop the timer, started by 'after' command ??
  //USBencoderTimer.Stop();
  if (this->USBEncoder)
    {
    this->USBEncoder->CloseUSBdevice();
    }

  // update text actors
  //Text_DeviceRotation->SetLabel("Opening encoder...");
  //Text_NeedleAngle->SetLabel("");
  
  this->USBEncoder = vtkTRProstateBiopsyUSBOpticalEncoder::New();

  if ( this->USBEncoder->OpenUSBdevice() ) 
    {
    // USB device exists, start the timer
    this->DeviceRotation = -1;
    this->NeedleAngle = -1;
    this->OpticalEncoderInitialized = true;
        
    } 
  else 
    {
    // No USB device, say so
    this->OpticalEncoderInitialized = false;
    // update text actors
    //Text_DeviceRotation->SetLabel("No optical encoder found!");
    //Text_NeedleAngle->SetLabel("");
    }
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::ResetOpticalEncoder()
{
    this->USBEncoder->SetChannelValueToZero(1);
    this->USBEncoder->SetChannelValueToZero(0);

    // Update it now
/*  wxTimerEvent event;
    this->OnTimer(event);*/
}
