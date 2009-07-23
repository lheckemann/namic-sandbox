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
#include "vtkTRProstateBiopsyTargetDescriptor.h"
#include "vtkTRProstateBiopsyCalibrationAlgo.h"
#include "vtkTRProstateBiopsyConfigFile.h"
#include "vtkTRProstateBiopsyMath.h"

#include <vtksys/stl/string>
#include <vtksys/SystemTools.hxx>

#include "vtkMath.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkAppendPolyData.h"
#include "vtkActor.h"
#include "vtkRenderer.h"


#include "itkMetaDataObject.h"

static const char TRPB_CALIBRATION[]="CALIBRATION";
static const char TRPB_TARGETING[]="TARGETING";
static const char TRPB_SEGMENTATION[]="SEGMENTATION";
static const char TRPB_VERIFICATION[]="VERIFICATION";

/*
vtkCxxRevisionMacro(vtkTRProstateBiopsyLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkTRProstateBiopsyLogic);
*/

//---------------------------------------------------------------------------
const int vtkTRProstateBiopsyLogic::PhaseTransitionMatrix[vtkTRProstateBiopsyLogic::NumPhases][vtkTRProstateBiopsyLogic::NumPhases] =
  {
      /*     next workphase     */
      /*    */ /* Cl  Sg Tg  Ve*/
      /* Cl */ {  1,  1,  1,  1  },
      /* Sg */ {  1,  1,  1,  1  },
      /* Tg */ {  1,  1,  1,  1  },
      /* Ve */ {  1,  1,  1,  1  },
  };

//---------------------------------------------------------------------------
const char *vtkTRProstateBiopsyLogic::WorkPhaseKey[vtkTRProstateBiopsyLogic::NumPhases] =
  {
  /* Cl */ TRPB_CALIBRATION ,
  /* Sg */ TRPB_SEGMENTATION,
  /* Tg */ TRPB_TARGETING   ,
  /* Ve */ TRPB_VERIFICATION,
  };
//----------------------------------------------------------------------------
vtkTRProstateBiopsyLogic* vtkTRProstateBiopsyLogic::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTRProstateBiopsyLogic");
  if(ret)
    {
      return (vtkTRProstateBiopsyLogic*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkTRProstateBiopsyLogic;
}

//---------------------------------------------------------------------------
vtkTRProstateBiopsyLogic::vtkTRProstateBiopsyLogic()
{
  this->CurrentPhase         = Calibration;
  this->PrevPhase            = Calibration;
  this->PhaseComplete        = false;
  this->PhaseTransitionCheck = true;

  this->CalibrationAlgo=vtkSmartPointer<vtkTRProstateBiopsyCalibrationAlgo>::New();

  this->TRProstateBiopsyModuleNode = NULL;

  this->USBEncoder = NULL;
  DeviceRotation=0;
  NeedleAngle=0;
  OpticalEncoderInitialized=false;

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

  if (this->USBEncoder)
    {
    this->USBEncoder->Delete();
    this->USBEncoder=NULL;
    }

  vtkSetAndObserveMRMLNodeMacro(this->TRProstateBiopsyModuleNode, NULL);
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
  vtkTRProstateBiopsyLogic *self = reinterpret_cast<vtkTRProstateBiopsyLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkTRProstateBiopsyLogic DataCallback");
  self->UpdateAll();
}
//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::UpdateAll()
{

}
//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::ProcessMRMLEvents(vtkObject * caller, unsigned long event, void * callData)
{
  vtkMRMLTRProstateBiopsyModuleNode* node = vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast(caller);
  if (node != NULL && this->TRProstateBiopsyModuleNode == node && event == vtkCommand::ModifiedEvent)
  {
    
  }
}
//---------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::SetSliceViewFromVolume( vtkSlicerApplication *app,vtkMRMLVolumeNode *volumeNode)
{
  if (!volumeNode)
    {
    return;
    }

  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> permutationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> rotationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

  volumeNode->GetIJKToRASDirectionMatrix(matrix);
  //slicerCerr("matrix");
  //slicerCerr("   " << matrix->GetElement(0,0) <<
//             "   " << matrix->GetElement(0,1) <<
  //           "   " << matrix->GetElement(0,2));
  //slicerCerr("   " << matrix->GetElement(1,0) <<
    //         "   " << matrix->GetElement(1,1) <<
    //         "   " << matrix->GetElement(1,2));
  //slicerCerr("   " << matrix->GetElement(2,0) <<
   //          "   " << matrix->GetElement(2,1) <<
    //         "   " << matrix->GetElement(2,2));

  int permutation[3];
  int flip[3];
  vtkTRProstateBiopsyMath::ComputePermutationFromOrientation(matrix, permutation, flip);

  //slicerCerr("permutation " << permutation[0] << " " <<
//             permutation[1] << " " << permutation[2]);
  //slicerCerr("flip " << flip[0] << " " <<
  //           flip[1] << " " << flip[2]);

  permutationMatrix->SetElement(0,0,0);
  permutationMatrix->SetElement(1,1,0);
  permutationMatrix->SetElement(2,2,0);

  permutationMatrix->SetElement(0, permutation[0],
                     (flip[permutation[0]] ? -1 : 1));
  permutationMatrix->SetElement(1, permutation[1],
                     (flip[permutation[1]] ? -1 : 1));
  permutationMatrix->SetElement(2, permutation[2],
                     (flip[permutation[2]] ? -1 : 1));

  //slicerCerr("permutationMatrix");
  //slicerCerr("   " << permutationMatrix->GetElement(0,0) <<
//             "   " << permutationMatrix->GetElement(0,1) <<
  //           "   " << permutationMatrix->GetElement(0,2));
  //slicerCerr("   " << permutationMatrix->GetElement(1,0) <<
    //         "   " << permutationMatrix->GetElement(1,1) <<
      //       "   " << permutationMatrix->GetElement(1,2));
  //slicerCerr("   " << permutationMatrix->GetElement(2,0) <<
        //     "   " << permutationMatrix->GetElement(2,1) <<
          //   "   " << permutationMatrix->GetElement(2,2));

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

}
//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkTRProstateBiopsyLogic::AddVolumeToScene(vtkSlicerApplication* app, const char *fileName, VolumeType volumeType/*=VOL_GENERIC*/)
{
  if (fileName==0)
  {
    vtkErrorMacro("AddVolumeToScene: invalid filename");
    return 0;
  }

  vtksys_stl::string volumeNameString = vtksys::SystemTools::GetFilenameName(fileName);
  vtkMRMLScalarVolumeNode *volumeNode = this->AddArchetypeVolume(app, fileName, volumeNameString.c_str());

  this->SetAutoScaleScalarVolume(volumeNode);
  this->SetSliceViewFromVolume(app, volumeNode);

  switch (volumeType)
  {
  case VOL_CALIBRATION:
    this->TRProstateBiopsyModuleNode->SetCalibrationVolumeNode(volumeNode);
    this->TRProstateBiopsyModuleNode->AddVolumeInformationToList(volumeNode, fileName, "Calibration");
    break;
  case VOL_TARGETING:
    this->TRProstateBiopsyModuleNode->SetTargetingVolumeNode(volumeNode);
    this->TRProstateBiopsyModuleNode->AddVolumeInformationToList(volumeNode, fileName, "Targeting");
    break;
  case VOL_VERIFICATION:
    this->TRProstateBiopsyModuleNode->SetVerificationVolumeNode(volumeNode);
    this->TRProstateBiopsyModuleNode->AddVolumeInformationToList(volumeNode, fileName, "Verification");
    break;
  case VOL_GENERIC:
    this->TRProstateBiopsyModuleNode->AddVolumeInformationToList(volumeNode, fileName, "Other");
    break;
  default:
    vtkErrorMacro("AddVolumeToScene: unknown volume type: " << volumeType);
  }
  
  volumeNode->Modified();
  this->Modified();

  return volumeNode;
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkTRProstateBiopsyLogic::AddArchetypeVolume(vtkSlicerApplication* app, const char* fileName, const char *volumeName)
{
  // Set up storageNode
  vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode> storageNode = vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode>::New(); 
  storageNode->SetFileName(fileName);
  // check to see if can read this type of file
  if (storageNode->SupportedFileType(fileName) == 0)
    {
    vtkErrorMacro("AddArchetypeVolume: can't read this kind of file: " << fileName);
    return 0;
    }
  storageNode->SetCenterImage(false);
  storageNode->SetSingleFile(false);
  storageNode->SetUseOrientationFromFile(true);

  // Set up scalarNode
  vtkSmartPointer<vtkMRMLScalarVolumeNode> scalarNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  scalarNode->SetName(volumeName);
  scalarNode->SetLabelMap(false);

  // Set up displayNode
  vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> displayNode = vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode>::New();   
  displayNode->SetAutoWindowLevel(false);
  displayNode->SetInterpolate(true);  
  vtkSmartPointer<vtkSlicerColorLogic> colorLogic = vtkSmartPointer<vtkSlicerColorLogic>::New(); 
  displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
  
  // Add nodes to scene
  this->GetMRMLScene()->SaveStateForUndo();  
  vtkDebugMacro("LoadArchetypeVolume: adding storage node to the scene");
  storageNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(storageNode);
  vtkDebugMacro("LoadArchetypeVolume: adding display node to the scene");
  displayNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(displayNode);
  vtkDebugMacro("LoadArchetypeVolume: adding scalar node to the scene");
  scalarNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(scalarNode);
  scalarNode->SetAndObserveStorageNodeID(storageNode->GetID());
  scalarNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  
  // Read the volume into the node
  vtkDebugMacro("AddArchetypeVolume: about to read data into scalar node " << scalarNode->GetName());
  storageNode->AddObserver(vtkCommand::ProgressEvent, this->LogicCallbackCommand);
  if (this->GetDebug())
    {
    storageNode->DebugOn();
    }
  storageNode->ReadData(scalarNode);
  vtkDebugMacro("AddArchetypeVolume: finished reading data into scalarNode");
  storageNode->RemoveObservers(vtkCommand::ProgressEvent, this->LogicCallbackCommand);
 
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


//--------------------------------------------------------------------------------------
bool vtkTRProstateBiopsyLogic::AddTargetToNeedle(std::string needleType, double rasLocation[3], unsigned int & targetDescIndex)
{
  if (!this->TRProstateBiopsyModuleNode)
      return false;

  // 1) get the needle information based on needleType 
  int needleIndex;
  for (needleIndex=0; needleIndex < this->TRProstateBiopsyModuleNode->GetNumberOfNeedles(); needleIndex++)
    {
    if (this->TRProstateBiopsyModuleNode->GetNeedleType(needleIndex) == needleType)
        break;
    }

  if (needleIndex >= this->TRProstateBiopsyModuleNode->GetNumberOfNeedles())
      return false;

  vtkSmartPointer<vtkTRProstateBiopsyTargetDescriptor> targetDesc=vtkSmartPointer<vtkTRProstateBiopsyTargetDescriptor>::New();
  targetDesc->SetRASLocation(rasLocation[0], rasLocation[1], rasLocation[2]);
  targetDesc->SetNeedleType(this->TRProstateBiopsyModuleNode->GetNeedleType(needleIndex),this->TRProstateBiopsyModuleNode->GetNeedleUID(needleIndex), this->TRProstateBiopsyModuleNode->GetNeedleDepth(needleIndex), this->TRProstateBiopsyModuleNode->GetNeedleOvershoot(needleIndex));
  targetDesc->SetNeedleListIndex(needleIndex);
  // record the FoR str
  std::string FoR = this->GetFoRStrFromVolumeNode(this->TRProstateBiopsyModuleNode->GetTargetingVolumeNode());
  targetDesc->SetFoRStr(FoR);

  // 2) calculate targeting parameters for active needle, store in a target descriptor
  if(!this->CalibrationAlgo->FindTargetingParams(this->TRProstateBiopsyModuleNode,targetDesc))
    {
    // error message
    // to do
    return false;
    }

  int fiducialIndex = -1;

   // 4) store the target descriptors in a list
  targetDescIndex = this->TRProstateBiopsyModuleNode->AddTargetDescriptor(targetDesc);
 
   // 3) add the target to targets FiducialListNode, searching which list to add to, knowing which needle list is being populated to?
  if (!this->TRProstateBiopsyModuleNode->AddTargetToFiducialList(rasLocation, needleIndex, targetDescIndex+1, fiducialIndex))
  {
  // error
  // to do
  return false;
  }
  
  targetDesc->SetFiducialIndex(fiducialIndex);
 
  // 5) visual feedback of creation of the click
  
  return true;
}
//--------------------------------------------------------------------------------------
bool vtkTRProstateBiopsyLogic::IsTargetReachable(int needleIndex, double rasLocation[3])
{

  if (!this->TRProstateBiopsyModuleNode)
      return false;

  static vtkTRProstateBiopsyTargetDescriptor *targetDesc=NULL;
  if (targetDesc==NULL)
  {
    targetDesc=vtkTRProstateBiopsyTargetDescriptor::New();
  }
  //vtkSmartPointer<vtkTRProstateBiopsyTargetDescriptor> targetDesc=vtkSmartPointer<vtkTRProstateBiopsyTargetDescriptor>::New();

  targetDesc->SetRASLocation(rasLocation[0], rasLocation[1], rasLocation[2]);
  
  targetDesc->SetNeedleType(this->TRProstateBiopsyModuleNode->GetNeedleType(needleIndex),this->TRProstateBiopsyModuleNode->GetNeedleUID(needleIndex), this->TRProstateBiopsyModuleNode->GetNeedleDepth(needleIndex), this->TRProstateBiopsyModuleNode->GetNeedleOvershoot(needleIndex));

  targetDesc->SetNeedleListIndex(needleIndex);

  // record the FoR str
//  std::string FoR = this->GetFoRStrFromVolumeNode(this->TRProstateBiopsyModuleNode->GetTargetingVolumeNode());
//  targetDesc->SetFoRStr(FoR);

  // 2) calculate targeting parameters for active needle, store in a target descriptor
  if(!this->CalibrationAlgo->FindTargetingParams(this->TRProstateBiopsyModuleNode,targetDesc))
    {
    // error message
    // to do
    return false;
    }

  if (targetDesc->GetIsOutsideReach())
  {
    return false;
  }

  return true;
}
//-------------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::SaveVolumesToExperimentFile(ostream &of)
{
  // go through each item in the volume list, and use that information to write to file
  int numberOfVolumes = this->TRProstateBiopsyModuleNode->GetNumberOfOpenVolumes();

  if (numberOfVolumes > 0)
    {
    for (int volNr = 0; volNr < numberOfVolumes; volNr++)
      {
      // for each volume write the following information to file
      // following information needs to be written to the file
      // 1) Volume Type
      // 2) Volume location file path
      // 3) Series number from dictionary
      // 4) Series description from dictionary
      // 5) FOR UID from dictionary
      // 6) Origin from node
      // 7) Pixel spacing from node
     
      // 1) volume type
      of << " VolumeType=\"" ;
      of << this->TRProstateBiopsyModuleNode->GetTypeOfVolumeAtIndex(volNr) << " ";
      of << "\" \n";

      // 2) disk location of volume     
      of << " VolumeDiskLocation=\"" ;
      of << this->TRProstateBiopsyModuleNode->GetDiskLocationOfVolumeAtIndex(volNr) << " ";
      of << "\" \n";

      // get active or not

      // get the mrml scalar volume node
      vtkMRMLScalarVolumeNode *volNode = this->TRProstateBiopsyModuleNode->GetVolumeNodeAtIndex(volNr);

      if (!volNode)
        {
        // error
        continue;
        }
      // remaining information to be had from the meta data dictionary     
      const itk::MetaDataDictionary &volDictionary = volNode->GetMetaDataDictionary();
      std::string tagValue;

      // series number
      tagValue.clear(); itk::ExposeMetaData<std::string>( volDictionary, "0020|0011", tagValue );
      of << " VolumeSeriesNumber=\"" ;
      of << tagValue << " ";
      of << "\" \n";

      // series description
      tagValue.clear(); itk::ExposeMetaData<std::string>( volDictionary, "0008|103e", tagValue );
      of << " VolumeSeriesDescription=\"" ;
      of << tagValue << " ";
      of << "\" \n";
           
      // frame of reference uid
      tagValue.clear(); itk::ExposeMetaData<std::string>( volDictionary, "0020|0052", tagValue );
      of << " VolumeFORUID=\"" ;
      of << tagValue << " ";
      of << "\" \n";

      // origin
      of << " VolumeOrigin=\"" ;
      double origin[3];
      volNode->GetOrigin(origin);
      for(int i = 0; i < 3; i++)
      of << origin[i] << " ";
      of << "\" \n";
          
      // spacing
      of << " VolumeSpacing=\"" ;
      double spacing[3];
      volNode->GetSpacing(spacing);
      for(int i = 0; i < 3; i++)
        of << spacing[i] << " ";
        of << "\" \n";

      }
    }
 

}
//--------------------------------------------------------------------------------------
std::string vtkTRProstateBiopsyLogic::GetFoRStrFromVolumeNode(vtkMRMLScalarVolumeNode *volNode)
{
  if (volNode==NULL)
  {
    vtkErrorMacro("Cannot get FoR, VolumeNode is undefined");
    return std::string("");
  }

  // remaining information to be had from the meta data dictionary     
  const itk::MetaDataDictionary &volDictionary = volNode->GetMetaDataDictionary();
  std::string tagValue; 

  // frame of reference uid
  tagValue.clear(); itk::ExposeMetaData<std::string>( volDictionary, "0020|0052", tagValue );
  
  return tagValue;
}

vtkImageData* vtkTRProstateBiopsyLogic::GetCalibMarkerPreProcOutput(int i)
{
  return this->CalibrationAlgo->GetCalibMarkerPreProcOutput(i);
}

// Perform AutoWindowLevel computation with CalculateScalarAutoLevels
// Need to do it manually, because automatic implementation in vtkMRMLScalarVolumeNode::UpdateFromMRML()
// assumes that it is a stats volume if name contains 'stat'.
// The name could easily contain 'stat' as we are dealing with 'prostate' images.
void vtkTRProstateBiopsyLogic::SetAutoScaleScalarVolume(vtkMRMLScalarVolumeNode *volumeNode)
{
  if (volumeNode==0)
  {
    vtkErrorMacro("Invalid volumeNode in SetAutoScaleScalarVolume");
    return;
  }
  vtkMRMLScalarVolumeDisplayNode *displayNode=volumeNode->GetScalarVolumeDisplayNode();
  if (displayNode==0)
  {
    vtkErrorMacro("Invalid displayNode in SetAutoScaleScalarVolume");
    return;
  }
  int modifyOld=displayNode->StartModify(); 
  displayNode->SetAutoWindowLevel(true);
  displayNode->SetAutoThreshold(false);
  displayNode->SetApplyThreshold(false);
  volumeNode->CalculateScalarAutoLevels(NULL,NULL);
  displayNode->SetAutoWindowLevel(false);
  displayNode->SetAutoThreshold(false);
  displayNode->SetApplyThreshold(false);
  displayNode->Modified();
  displayNode->EndModify(modifyOld); 
}

void vtkTRProstateBiopsyLogic::ReadConfigFile()
{
  vtkSmartPointer<vtkTRProstateBiopsyConfigFile> configFile = vtkSmartPointer<vtkTRProstateBiopsyConfigFile>::New();
  configFile->ReadConfigFile(this->TRProstateBiopsyModuleNode);
}

bool vtkTRProstateBiopsyLogic::SegmentRegisterMarkers(double thresh[4], double fidDims[3], double radius, bool bUseRadius, double initialAngle, std::string &resultDetails)
{
  TRProstateBiopsyCalibrationFromImageInput in;

  for (int i=0; i<CALIB_MARKER_COUNT; i++)
  {
    this->TRProstateBiopsyModuleNode->GetCalibrationMarker(i, in.MarkerInitialPositions[i][0], in.MarkerInitialPositions[i][1], in.MarkerInitialPositions[i][2]); 
    in.MarkerSegmentationThreshold[i]=thresh[i];
  }
  for (int i=0; i<3; i++)
  {
    in.MarkerDimensions[i]=fidDims[i];
  }
  in.MarkerRadius=radius;
  in.RobotInitialAngle=initialAngle;
  vtkMRMLScalarVolumeNode *calibVol = this->TRProstateBiopsyModuleNode->GetCalibrationVolumeNode();
  if (calibVol==0)
  {
    vtkErrorMacro("SegmentRegisterMarkers CalibrationVolume is invalid");
    resultDetails="Calibration volume is invalid";
    return false;
  }
  vtkSmartPointer<vtkMatrix4x4> ijkToRAS = vtkSmartPointer<vtkMatrix4x4>::New(); 
  calibVol->GetIJKToRASMatrix(ijkToRAS);
  in.VolumeIJKToRASMatrix=ijkToRAS;
  calibVol->GetSpacing(in.VolumeSpacing);  
  in.VolumeImageData=calibVol->GetImageData();

  TRProstateBiopsyCalibrationFromImageOutput res;

  bool success=true;
  if (!this->CalibrationAlgo->CalibrateFromImage(in, res))
  {
    // calibration failed

    // clean up clibration result in MRML node
    this->TRProstateBiopsyModuleNode->ResetCalibrationData();

    std::ostrstream os;
    // if a marker not found, then make the error report more precise
    for (int i=0; i<CALIB_MARKER_COUNT; i++)
    {      
      if (!res.MarkerFound[i])
      {
        os << "Marker "<<i+1<<" cannot be detected. ";
        success=false;
      }
    } 
    os << "Calibration failed." << std::ends;
    resultDetails=os.str();
    os.rdbuf()->freeze();    
  }
  else
  {    
    // calibration is successful
    for (int i=0; i<CALIB_MARKER_COUNT; i++)
    {
      // update marker positions in MRML node
      this->TRProstateBiopsyModuleNode->SetCalibrationMarker(i, res.MarkerPositions[i]);
      in.MarkerSegmentationThreshold[i]=thresh[i];
    }

    const TRProstateBiopsyCalibrationData calibData=this->CalibrationAlgo->GetCalibrationData();
    this->TRProstateBiopsyModuleNode->SetCalibrationData(calibData);

    resultDetails="Calibration is successfully completed.";    
    success=true;  
  }
  this->TRProstateBiopsyModuleNode->Modified();
  return success;
}

