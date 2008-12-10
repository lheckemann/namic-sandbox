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

#include "vtkProstateNavLogic.h"

#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerColorLogic.h"


// for DICOM read
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"
#include "itkSpatialOrientationAdapter.h"

// for communication with robot and scanner
#include "BRPTPRInterface.h"

vtkCxxRevisionMacro(vtkProstateNavLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkProstateNavLogic);

//---------------------------------------------------------------------------
const int vtkProstateNavLogic::PhaseTransitionMatrix[vtkProstateNavLogic::NumPhases][vtkProstateNavLogic::NumPhases] =
  {
               /*     next workphase     */
      /*    */ /* St  Pl  Cl  Tg  Mn  Em */
      /* St */ {  1,  1,  0,  0,  0,  1  },
      /* Pl */ {  1,  1,  1,  0,  0,  1  },
      /* Cl */ {  1,  1,  1,  1,  1,  1  },
      /* Tg */ {  1,  1,  1,  1,  1,  1  },
      /* Mn */ {  1,  1,  1,  1,  1,  1  },
      /* Em */ {  1,  1,  1,  1,  1,  1  },
  };

//---------------------------------------------------------------------------
const char *vtkProstateNavLogic::WorkPhaseKey[vtkProstateNavLogic::NumPhases] =
  { /* define in BRPTPRInterface.h */
  /* Su */ BRPTPR_START_UP   ,
  /* Pl */ BRPTPR_PLANNING   ,
  /* Cl */ BRPTPR_CALIBRATION,
  /* Tg */ BRPTPR_TARGETING  ,
  /* Mn */ BRPTPR_MANUAL     ,
  /* Em */ BRPTPR_EMERGENCY  ,
  };

//---------------------------------------------------------------------------
vtkProstateNavLogic::vtkProstateNavLogic()
{
  this->CurrentPhase         = StartUp;
  this->PrevPhase            = StartUp;
  this->PhaseComplete        = false;
  this->PhaseTransitionCheck = true;
  
  this->RobotWorkPhase       = -1;
  this->ScannerWorkPhase     = -1;
  
  // Timer Handling
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkProstateNavLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkProstateNavLogic::~vtkProstateNavLogic()
{

    if (this->DataCallbackCommand)
    {
      this->DataCallbackCommand->Delete();
    }

}

//---------------------------------------------------------------------------
void vtkProstateNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->vtkObject::PrintSelf(os, indent);

    os << indent << "vtkProstateNavLogic:             " << this->GetClassName() << "\n";

}

//---------------------------------------------------------------------------
void vtkProstateNavLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
    vtkProstateNavLogic *self = reinterpret_cast<vtkProstateNavLogic *>(clientData);
    vtkDebugWithObjectMacro(self, "In vtkProstateNavLogic DataCallback");
    self->UpdateAll();
}

//---------------------------------------------------------------------------
void vtkProstateNavLogic::UpdateAll()
{

}


//---------------------------------------------------------------------------
int vtkProstateNavLogic::SwitchWorkPhase(int newwp)
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
int vtkProstateNavLogic::IsPhaseTransitable(int nextwp)
{
  if (nextwp < 0 || nextwp > NumPhases)
    {
    return 0;
    }
  
  if (PhaseTransitionCheck == 0)
    {
    return 1;
    }
  
  if (PhaseComplete)
    {
    return PhaseTransitionMatrix[CurrentPhase][nextwp];
    }
  else
    {
    return PhaseTransitionMatrix[PrevPhase][nextwp];
    }
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::ConnectTracker(const char* filename)
{

    return 1;
}


//---------------------------------------------------------------------------
int vtkProstateNavLogic::DisconnectTracker()
{

    return 1;
}


//---------------------------------------------------------------------------
int vtkProstateNavLogic::RobotStop()
{

  std::cerr << "vtkProstateNavLogic::RobotStop()" << std::endl;
  return 1;

}


//---------------------------------------------------------------------------
int vtkProstateNavLogic::RobotMoveTo(float px, float py, float pz,
                                     float nx, float ny, float nz,
                                     float tx, float ty, float tz)
{

  std::cerr << "vtkProstateNavLogic::RobotMoveTo()" << std::endl;


  return 1;
}


//---------------------------------------------------------------------------
int vtkProstateNavLogic::RobotMoveTo(float position[3], float orientation[3])
{
  std::cerr << "vtkProstateNavLogic::RobotMoveTo()" << std::endl;

  return 1;
}


//---------------------------------------------------------------------------
int vtkProstateNavLogic::ScanStart()
{

  return 1;
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::ScanPause()
{
  return 1;
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::ScanStop()
{

  return 1;
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::WorkPhaseStringToID(const char* string)
{
  for (int i = 0; i < vtkProstateNavLogic::NumPhases; i ++)
    {
    if (strcmp(vtkProstateNavLogic::WorkPhaseKey[i], string) == 0)
      {
      return i;
      }
    }
  return -1; // Nothing found.
}


