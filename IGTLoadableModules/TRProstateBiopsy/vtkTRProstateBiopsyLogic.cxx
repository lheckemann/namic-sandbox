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

#include "vtkTRProstateBiopsyLogic.h"

#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerVolumesGUI.h"
#include "vtkSlicerVolumesLogic.h"
#include "vtkSlicerColorLogic.h"


// for DICOM read
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"
#include "itkSpatialOrientationAdapter.h"

vtkCxxRevisionMacro(vtkTRProstateBiopsyLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkTRProstateBiopsyLogic);

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
#ifndef IGSTK_OFF
    igstk::RealTimeClock::Initialize();
#endif

    this->CurrentPhase         = Calibration;
    this->PrevPhase            = Calibration;
    this->PhaseComplete        = false;
    this->PhaseTransitionCheck = true;

    this->LocatorTransform     = NULL;
    this->LocatorMatrix        = NULL;

    // Timer Handling

    this->DataCallbackCommand = vtkCallbackCommand::New();
    this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
    this->DataCallbackCommand->SetCallback(vtkTRProstateBiopsyLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkTRProstateBiopsyLogic::~vtkTRProstateBiopsyLogic()
{
  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
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

  //----------------------------------------------------------------
  // Get Needle Position

  // Position / orientation parameters:
  //   (px, py, pz) : position
  //   (nx, ny, nz) : normal vector
  //   (tx, ty, tz) : transverse vector
  //   (sx, sy, sz) : vector orthogonal to n and t ( n x t )
  
  float px, py, pz, nx, ny, nz, tx, ty, tz;
  float sx, sy, sz;
  
  this->LocatorMatrix = NULL;

  if (!this->LocatorMatrix)
    {
    this->LocatorMatrix = vtkMatrix4x4::New();

    this->LocatorMatrix->SetElement(0, 0, 0.0);  // px
    this->LocatorMatrix->SetElement(1, 0, 0.0);  // py
    this->LocatorMatrix->SetElement(2, 0, 0.0);  // pz

    this->LocatorMatrix->SetElement(0, 1, 0.0);  // nx
    this->LocatorMatrix->SetElement(1, 1, 0.0);  // ny
    this->LocatorMatrix->SetElement(2, 1, 1.0);  // nz

    this->LocatorMatrix->SetElement(0, 2, 1.0);  // tx
    this->LocatorMatrix->SetElement(1, 2, 0.0);  // ty
    this->LocatorMatrix->SetElement(2, 2, 0.0);  // tz
    }
  
  px = this->LocatorMatrix->GetElement(0, 0);
  py = this->LocatorMatrix->GetElement(1, 0);
  pz = this->LocatorMatrix->GetElement(2, 0);
  
  nx = this->LocatorMatrix->GetElement(0, 1);
  ny = this->LocatorMatrix->GetElement(1, 1);
  nz = this->LocatorMatrix->GetElement(2, 1);
  
  tx = this->LocatorMatrix->GetElement(0, 2);
  ty = this->LocatorMatrix->GetElement(1, 2);
  tz = this->LocatorMatrix->GetElement(2, 2);
  
  sx = ny*tz-nz*ty;
  sy = nz*tx-nx*tz;
  sz = nx*ty-ny*tx;
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::AddRealtimeVolumeNode(vtkSlicerApplication* app, const char* name)
{
  vtkSlicerVolumesGUI   *volGui   = (vtkSlicerVolumesGUI*)app->GetModuleGUIByName("Volumes");
  vtkSlicerVolumesLogic* volLogic = (vtkSlicerVolumesLogic*)(volGui->GetLogic());

  if (this->VolumeNode == NULL)
    {
    this->VolumesLogic = volLogic;
    this->VolumeNode = AddVolumeNode(volLogic, name);
    }
}

//---------------------------------------------------------------------------
int vtkTRProstateBiopsyLogic::SwitchWorkPhase(int newwp)
{
  if (IsPhaseTransitable(newwp))
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
vtkMRMLVolumeNode* vtkTRProstateBiopsyLogic::AddVolumeNode(
                vtkSlicerVolumesLogic* volLogic,
                const char* volumeNodeName)
{
  vtkMRMLVolumeNode *volumeNode = NULL;

  if (volumeNode == NULL)  // if volume node has not been created
    {
    vtkMRMLVolumeDisplayNode *displayNode = NULL;
    vtkMRMLScalarVolumeNode *scalarNode = vtkMRMLScalarVolumeNode::New();
    vtkImageData* image = vtkImageData::New();

    float fov = 300.0;
    image->SetDimensions(256, 256, 1);
    image->SetExtent(0, 255, 0, 255, 0, 0 );
    image->SetSpacing( fov/256, fov/256, 10 );
    image->SetOrigin( -fov/2, -fov/2, -0.0 );
    image->SetScalarTypeToShort();
    image->AllocateScalars();
        
    short* dest = (short*) image->GetScalarPointer();
    if (dest)
      {
      memset(dest, 0x00, 256*256*sizeof(short));
      image->Update();
      }
        
    scalarNode->SetAndObserveImageData(image);
        
    /* Based on the code in vtkSlicerVolumeLogic::AddHeaderVolume() */
    displayNode = vtkMRMLVolumeDisplayNode::New();
    scalarNode->SetLabelMap(0);
    volumeNode = scalarNode;
        
    if (volumeNode != NULL)
      {
      volumeNode->SetName(volumeNodeName);
      volLogic->GetMRMLScene()->SaveStateForUndo();
            
      vtkDebugMacro("Setting scene info");
      volumeNode->SetScene(volLogic->GetMRMLScene());
      displayNode->SetScene(volLogic->GetMRMLScene());
            
      vtkDebugMacro("Set basic display info");
      double range[2];
      volumeNode->GetImageData()->GetScalarRange(range);
            
      vtkDebugMacro("Adding node..");
      volLogic->GetMRMLScene()->AddNode(displayNode);
            
      vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
      displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
            
      volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
            
      vtkDebugMacro("Name vol node "<<volumeNode->GetClassName());
      vtkDebugMacro("Display node "<<displayNode->GetClassName());
            
      volLogic->GetMRMLScene()->AddNode(volumeNode);
      vtkDebugMacro("Node added to scene");
            
      volLogic->SetActiveVolumeNode(volumeNode);
      volLogic->Modified();
      }
    }

  return volumeNode;
}

