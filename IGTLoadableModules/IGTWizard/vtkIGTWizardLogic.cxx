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

#include "vtkIGTWizardLogic.h"

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

#include "vtkIGTWizardGUI.h"

// for communication with robot and scanner
#include "BRPTPRInterface.h"

vtkCxxRevisionMacro(vtkIGTWizardLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkIGTWizardLogic);

//---------------------------------------------------------------------------
const int vtkIGTWizardLogic::PhaseTransitionMatrix[vtkIGTWizardLogic::NumPhases][vtkIGTWizardLogic::NumPhases] =
  {
               /*     next workphase     */
      /*    */ /* St  Pl  Cl  Tg  Mn  Em */
      /* St */ {  1,  1,  0/*,  0,  0,  0 */ },
      /* Pl */ {  1,  1,  1/*,  0,  0,  0 */ },
      /* Cl */ {  1,  1,  1/*,  1,  1,  0 */ },
//      /* Tg */ {  1,  1,  1,  1,  1,  0  },
//      /* Mn */ {  1,  1,  1,  1,  1,  0  },
//      /* Em */ {  1,  1,  1,  1,  1,  0  },
  };

//---------------------------------------------------------------------------
const char *vtkIGTWizardLogic::WorkPhaseKey[vtkIGTWizardLogic::NumPhases] =
  { /* define in BRPTPRInterface.h */
  /* Su */ BRPTPR_START_UP   ,
  /* Pl */ BRPTPR_PLANNING   ,
  /* Cl */ BRPTPR_CALIBRATION,
//  /* Tg */ BRPTPR_TARGETING  ,
//  /* Mn */ BRPTPR_MANUAL     ,
//  /* Em */ BRPTPR_EMERGENCY  ,
  };

//---------------------------------------------------------------------------
vtkIGTWizardLogic::vtkIGTWizardLogic()
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
  this->DataCallbackCommand->SetCallback(vtkIGTWizardLogic::DataCallback);

  this->TimerOn = 0;

}


//---------------------------------------------------------------------------
vtkIGTWizardLogic::~vtkIGTWizardLogic()
{

    if (this->DataCallbackCommand)
    {
      this->DataCallbackCommand->Delete();
    }

}

//---------------------------------------------------------------------------
void vtkIGTWizardLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->vtkObject::PrintSelf(os, indent);

    os << indent << "vtkIGTWizardLogic:             " << this->GetClassName() << "\n";

}

//---------------------------------------------------------------------------
void vtkIGTWizardLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
    vtkIGTWizardLogic *self = reinterpret_cast<vtkIGTWizardLogic *>(clientData);
    vtkDebugWithObjectMacro(self, "In vtkIGTWizardLogic DataCallback");
    self->UpdateAll();
}

//---------------------------------------------------------------------------
void vtkIGTWizardLogic::UpdateAll()
{

}


int vtkIGTWizardLogic::Enter()
{
  return 1;
}


//---------------------------------------------------------------------------
int vtkIGTWizardLogic::SwitchWorkPhase(int newwp)
{
  if (IsPhaseTransitionable(newwp))
    {
    this->PrevPhase     = this->CurrentPhase;
    this->CurrentPhase  = newwp;
    this->PhaseComplete = false;

    /*
    char* command = NULL;
    switch(this->CurrentPhase)
      {
      case StartUp:
        this->TimerOn = 0;
        command = "START_UP";
        break;
      case Planning:
        this->TimerOn = 0;
        command = "PLANNING";
        break;
      case Calibration:
        this->TimerOn = 0;
        command = "CALIBRATION";
        break;
      case Targeting:
        command = "TARGETING";
        if (!this->TimerOn)
          {
          this->TimerOn = 1;
          vtkKWTkUtilities::CreateTimerHandler(this->GetGUI()->GetApplication(), 200, this, "TimerHandler");
          }
        break;
      case Manual:
        command = "MANUAL";
        if (!this->TimerOn)
          {
          this->TimerOn = 1;
          vtkKWTkUtilities::CreateTimerHandler(this->GetGUI()->GetApplication(), 200, this, "TimerHandler");
          }
        break;
      case Emergency:
        this->TimerOn = 0;
        command = "EMERGENCY";
        break;
      default:
        break;
      }
    */
    return 1;
    }
  return 0;
}


//----------------------------------------------------------------------------
void vtkIGTWizardLogic::TimerHandler()
{
  if (this->TimerOn)
    {
    vtkKWTkUtilities::CreateTimerHandler(this->GetGUI()->GetApplication(), 200, this, "TimerHandler");
    }
}


//---------------------------------------------------------------------------
int vtkIGTWizardLogic::IsPhaseTransitionable(int nextwp)
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
int vtkIGTWizardLogic::WorkPhaseStringToID(const char* string)
{
  for (int i = 0; i < vtkIGTWizardLogic::NumPhases; i ++)
    {
    if (strcmp(vtkIGTWizardLogic::WorkPhaseKey[i], string) == 0)
      {
      return i;
      }
    }
  return -1; // Nothing found.
}



