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
#include "vtkTRProstateBiopsyTargetDescriptor.h"

#include "vtkMath.h"
#include "vtkImageMedian3D.h"
#include "vtkImageClip.h"
#include "vtkPointData.h"
#include "vtkMetaImageWriter.h"

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_transpose.h>
#include <itksys/SystemTools.hxx>

#include <algorithm>

#include <vtksys/stl/string>
#include <vtksys/SystemTools.hxx>

vtkCxxRevisionMacro(vtkTRProstateBiopsyLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkTRProstateBiopsyLogic);


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

//---------------------------------------------------------------------------
vtkTRProstateBiopsyLogic::vtkTRProstateBiopsyLogic()
{
  this->CurrentPhase         = Calibration;
  this->PrevPhase            = Calibration;
  this->PhaseComplete        = false;
  this->PhaseTransitionCheck = true;

  this->TRProstateBiopsyModuleNode = NULL;

  this->USBEncoder = NULL;

  //this->CalibrationSliceNodeXML = NULL;

  // Timer Handling
  //this->DataCallbackCommand = vtkCallbackCommand::New();
  //this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  //this->DataCallbackCommand->SetCallback(vtkTRProstateBiopsyLogic::DataCallback);

  this->CoordinatesVector.clear();


}


//---------------------------------------------------------------------------
vtkTRProstateBiopsyLogic::~vtkTRProstateBiopsyLogic()
{
  //if (this->DataCallbackCommand)
  //  {
  //  this->DataCallbackCommand->Delete();
  //  }

  vtkSetMRMLNodeMacro(this->TRProstateBiopsyModuleNode, NULL);
  
}


//----------------------------------------------------------------------------
bool vtkTRProstateBiopsyLogic::DoubleEqual(double val1, double val2)
{
  bool result = false;
    
  if(fabs(val2-val1) < 0.0001)
      result = true;

  return result;
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
void vtkTRProstateBiopsyLogic::SetSliceViewFromVolume( vtkSlicerApplication *app,vtkMRMLVolumeNode *volumeNode)
{
  if (!volumeNode)
    {
    return;
    }

  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  vtkMatrix4x4 *permutationMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4 *rotationMatrix = vtkMatrix4x4::New();

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
  vtkTRProstateBiopsyMath::ComputePermutationFromOrientation(
    matrix, permutation, flip);

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

  matrix->Delete();
  permutationMatrix->Delete();
  rotationMatrix->Delete();
}
//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkTRProstateBiopsyLogic::AddVolumeToScene(
  vtkSlicerApplication* app, const char *fileName)
{
  vtksys_stl::string volumeNameString =
    vtksys::SystemTools::GetFilenameName(fileName);

  vtkMRMLScalarVolumeNode *volumeNode =
    this->AddArchetypeVolume(app, fileName, volumeNameString.c_str());

  //vtkSetMRMLNodeMacro(this->CalibrationVolumeNode, volumeNode );
  //this->TRProstateBiopsyModuleNode->SetCalibrationVolumeNode(volumeNode);

  this->Modified();

  this->SetSliceViewFromVolume(app, volumeNode);

  return volumeNode;
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkTRProstateBiopsyLogic::AddCalibrationVolume(
  vtkSlicerApplication* app, const char *fileName)
{
  vtksys_stl::string volumeNameString =
    vtksys::SystemTools::GetFilenameName(fileName);

  vtkMRMLScalarVolumeNode *volumeNode =
    this->AddArchetypeVolume(app, fileName, volumeNameString.c_str());

  //vtkSetMRMLNodeMacro(this->CalibrationVolumeNode, volumeNode );
  this->TRProstateBiopsyModuleNode->SetCalibrationVolumeNode(volumeNode);

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

//  vtkSetMRMLNodeMacro(this->TargetingVolumeNode, volumeNode );
  this->TRProstateBiopsyModuleNode->SetTargetingVolumeNode(volumeNode);
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

  //vtkSetMRMLNodeMacro(this->VerificationVolumeNode, volumeNode );
  this->TRProstateBiopsyModuleNode->SetVerificationVolumeNode(volumeNode);

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
void vtkTRProstateBiopsyLogic::ReadConfigFile()
{
  // read the MR config file

  // assumption
    std::string curDir = itksys::SystemTools::GetCurrentWorkingDirectory();

  if (itksys::SystemTools::FileExists("TRPBConfig.cfg"))
  {
  ifstream file("TRPBConfig.cfg");
  char currentLine[256];  
  char* attName = "";
  char* attValue = "";
  char* pdest;
  int nCharCount = 0;
  unsigned int indexEndOfAttribute = 0;
  unsigned int indexStartOfValue = 0;
  unsigned int indexEndOfValue = 0;

  int paramSetCount = 0;
  while(!file.eof())
     {
     // first get each line,
     // then parse each line on basis of attName, and attValue
     // this can be done as delimiters '='[]' is used to separate out name from value
        file.getline(&currentLine[0], 256, '\n');   
        nCharCount = strlen(currentLine);
        indexEndOfAttribute = strcspn(currentLine,"=");
        if(indexEndOfAttribute >0)
          {
          attName = new char[indexEndOfAttribute+1];
          strncpy(attName, currentLine,indexEndOfAttribute);
          attName[indexEndOfAttribute] = '\0';
          pdest = strchr(currentLine, '"');   
          indexStartOfValue = (int)(pdest - currentLine + 1);
          pdest = strrchr(currentLine, '"');
          indexEndOfValue = (int)(pdest - currentLine + 1);
          attValue = new char[indexEndOfValue-indexStartOfValue+1];
          strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
          attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

          // at this point, we have line separated into, attributeName, and attributeValue
          // now we need to do string matching on attributeName, and further parse attributeValue as it may have more than one value
          if (!strcmp(attName, "NumberOfNeedles"))
              {
              std::stringstream ss;
              ss << attValue;
              int val;
              ss >> val;
              this->GetTRProstateBiopsyModuleNode()->SetNumberOfNeedles(val);
              this->GetTRProstateBiopsyModuleNode()->SetupNeedlesList();

              if (val < 0)
                  break;
              char tagName[256]="";
              for (unsigned int i = 0; i < val; i++)
                {
                // read for each needle

                // form the tage name
                // needle type
                sprintf(tagName,"Needle%dType", i+1);
                
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue;
                    std::string str;
                    ss >> str;
                    this->GetTRProstateBiopsyModuleNode()->SetNeedleType(i, str);
                    }
                  }

                // form the tag name
                // needle depth
                sprintf(tagName,"Needle%dDepth", i+1);
                ss.clear();
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue;
                    float val;
                    ss >> val;
                    this->GetTRProstateBiopsyModuleNode()->SetNeedleDepth(i, val);
                    }
                  }

                // form the tage name
                // needle overshoot
                sprintf(tagName,"Needle%dOvershoot", i+1);
                ss.clear();
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue;
                    float val;
                    ss >> val;
                    this->GetTRProstateBiopsyModuleNode()->SetNeedleOvershoot(i, val);
                    }
                  }

                // form the tage name
                // needle description
                sprintf(tagName,"Needle%dDescription", i+1);
                ss.clear();
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue; 
                    std::string str;
                    ss >> str;
                    this->GetTRProstateBiopsyModuleNode()->SetNeedleDescription(i, str);
                    }
                  }

                // form the tage name
                // needle uid
                sprintf(tagName,"Needle%dUID", i+1);
                ss.clear();
                file.getline(&currentLine[0], 256, '\n');   
                nCharCount = strlen(currentLine);
                indexEndOfAttribute = strcspn(currentLine,"=");
                if(indexEndOfAttribute >0)
                  {
                  attName = new char[indexEndOfAttribute+1];
                  strncpy(attName, currentLine,indexEndOfAttribute);
                  attName[indexEndOfAttribute] = '\0';
                  pdest = strchr(currentLine, '"');   
                  indexStartOfValue = (int)(pdest - currentLine + 1);
                  pdest = strrchr(currentLine, '"');
                  indexEndOfValue = (int)(pdest - currentLine + 1);
                  attValue = new char[indexEndOfValue-indexStartOfValue+1];
                  strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
                  attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

                  if (!strcmp(attName, tagName))
                    {
                    std::stringstream ss;
                    ss << attValue;
                    unsigned int val;
                    ss >> val;
                    this->GetTRProstateBiopsyModuleNode()->SetNeedleUID(i, val);
                    }
                  }
                }
              }
          else if (!strcmp(attName, "ExperimentSavePath"))
              {
              std::stringstream ss;
              ss << attValue;
              std::string str;
              ss >> str;
              this->GetTRProstateBiopsyModuleNode()->SetExperimentSavePathInConfigFile(str.c_str());              
              }
            else
              {
              // error in file?
              }     
           
          }// end if testing for it is a valid attName

        } // end while going through the file
     
    file.close();
  }
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
void vtkTRProstateBiopsyLogic::SegmentRegisterMarkers(double thresh[4], double fidDims[3], double radius, bool bUseRadius, double initialAngle)
{  
  double P1[3], P2[3], v1[3], v2[3], I1[3], I2[3];

  // segement the axis using initial guesses
  this->SegmentAxis(1, thresh, fidDims, radius, bUseRadius, initialAngle, P1, v1);
  this->SegmentAxis(2, thresh, fidDims, radius, bUseRadius, initialAngle, P2, v2);

  // markers segmented, now register probe
  this->FindProbe(P1, P2, v1, v2, I1, I2, initialAngle);


    
}
//--------------------------------------------------------------------------------------
void vtkTRProstateBiopsyLogic::SegmentAxis(int nAxis, double thresh[4], double fidDims[3], double radius, bool bUseRadius, double initialAngle, double P1[3], double v1[3])
{
    /// \todo Show resliced object in a window, like ITK-SNAP

    double marker_1_RAS[3], marker_2_RAS[3];
    double PNormal1[3];
    

    // marker 1
    this->GetTRProstateBiopsyModuleNode()->GetCalibrationMarker(2*nAxis-1, marker_1_RAS[0], marker_1_RAS[1], marker_1_RAS[2]); 

    // marker 2
    this->GetTRProstateBiopsyModuleNode()->GetCalibrationMarker(2*nAxis, marker_2_RAS[0], marker_2_RAS[1], marker_2_RAS[2]); 
    
    // get index
    vtkMRMLScalarVolumeNode *calibVol = this->GetTRProstateBiopsyModuleNode()->GetCalibrationVolumeNode();
    vtkMatrix4x4 *rasToIJK = vtkMatrix4x4::New();
    vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
    calibVol->GetIJKToRASMatrix(ijkToRAS);
    vtkMatrix4x4::Invert(ijkToRAS, rasToIJK);

    double rasIn[4] = {marker_1_RAS[0], marker_1_RAS[1], marker_1_RAS[2], 1};
    double ijkOut[4];   
    rasToIJK->MultiplyPoint(rasIn, ijkOut);
    float m_1_IJK[3] = {ijkOut[0], ijkOut[1], ijkOut[2]};
    
    rasIn[0] = marker_2_RAS[0];
    rasIn[1] = marker_2_RAS[1];
    rasIn[2] = marker_2_RAS[2];

    rasToIJK->MultiplyPoint(rasIn, ijkOut);
    float m_2_IJK[3] = {ijkOut[0], ijkOut[1], ijkOut[2]};

    // slice plane 1 normal
    PNormal1[0] = m_1_IJK[0] - m_2_IJK[0];
    PNormal1[1] = m_1_IJK[1] - m_2_IJK[1];
    PNormal1[2] = m_1_IJK[2] - m_2_IJK[2];
    vtkMath::Normalize(PNormal1);

    // Marker 1 and Marker 2 are coincidental points? It should not happen!
    if ( this->DoubleEqual(PNormal1[0],0.0) && this->DoubleEqual(PNormal1[1],0.0) && this->DoubleEqual(PNormal1[2],0.0) ) {     
        PNormal1[2]=1.0;
    }

    // empty
    P1[0]=P1[1]=P1[2]=0.0;
    v1[0]=v1[1]=v1[2]=0.0;
    
    // /* Automatic segmentation
    bool lFound = false;
    this->CoordinatesVector.clear();
    
    // First set of circle centers
    lFound = SegmentCircle( m_1_IJK, PNormal1, thresh[0+2*nAxis-2], fidDims, radius, bUseRadius, initialAngle); 
    // bring back in RAS space  
    double ijkIn[4] = {m_1_IJK[0], m_1_IJK[1], m_1_IJK[2], 1};
    double rasOut[4];   
    ijkToRAS->MultiplyPoint(ijkIn, rasOut);
    double m_1_ras[3] = {rasOut[0], rasOut[1], rasOut[2]};
    this->GetTRProstateBiopsyModuleNode()->SetCalibrationMarker(2*nAxis-1, m_1_ras); 

    // 2nd set of circle centers
    lFound = SegmentCircle( m_2_IJK, PNormal1, thresh[1+2*nAxis-2], fidDims, radius, bUseRadius, initialAngle);
    // bring back in RAS space  
    ijkIn[0] = m_2_IJK[0];
    ijkIn[1] = m_2_IJK[1];
    ijkIn[2] = m_2_IJK[2];
    ijkToRAS->MultiplyPoint(ijkIn, rasOut);
    double m_2_ras[3] = {rasOut[0], rasOut[1], rasOut[2]};
    this->GetTRProstateBiopsyModuleNode()->SetCalibrationMarker(2*nAxis, m_2_ras);

    int vecSize = this->CoordinatesVector.size();

    // Use CoordVector to find the line 
    this->RemoveOutliners(P1,v1, marker_1_RAS, marker_2_RAS);

    // we did not find anything, so take the clicked points (too bad)
    if ( this->DoubleEqual(v1[0],0.0) && this->DoubleEqual(v1[1],0.0) && this->DoubleEqual(v1[2],0.0) ) {
                // CoordVector.clear(); // otherwise it will throw out badly \todo new alg?
                itk::Point<double,3> coord;
            
                coord[0] = marker_1_RAS[0];
                coord[1] = marker_1_RAS[1];
                coord[2] = marker_1_RAS[2];
                this->CoordinatesVector.push_back(coord); // def1
                coord[0] = marker_1_RAS[0];
                coord[1] = marker_1_RAS[1];
                coord[2] = marker_1_RAS[2];
                this->CoordinatesVector.push_back(coord); // def2
                RemoveOutliners(P1,v1, marker_1_RAS, marker_2_RAS);
    }

    
    // Coordinates can be freed
    this->CoordinatesVector.clear();
    // End Automatic segmentation */
}
//--------------------------------------------------------------------------------------
bool vtkTRProstateBiopsyLogic::SegmentCircle(float markerCenterGuess[3],const double normal[3],  double thresh, double fidDims[3], double radius, bool bUseRadius, double initialAngle)
{

    vtkMRMLScalarVolumeNode *calibVol = this->GetTRProstateBiopsyModuleNode()->GetCalibrationVolumeNode();
    
    vtkMatrix4x4 *rasToIJK = vtkMatrix4x4::New();
    vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
    calibVol->GetIJKToRASMatrix(ijkToRAS);
    vtkMatrix4x4::Invert(ijkToRAS, rasToIJK);

    
    double spacing[3];
    calibVol->GetSpacing(spacing);

    // voi filter
    vtkImageClip *voiFilter = vtkImageClip::New();

    // median filter
    vtkImageMedian3D *medianFilter = vtkImageMedian3D::New();

    // threshold filter
    vtkImageThreshold *thresholdFilter = vtkImageThreshold::New();

    int x1, x2, y1, y2, z1, z2;
    double w = fidDims[0]/spacing[0];
    double h = fidDims[1]/spacing[1];
    double d = fidDims[2]/spacing[2];

    double maxSize;
    if (w>h) 
        maxSize=w; 
    else 
        maxSize=h;

    if (d>maxSize) 
        maxSize=d;
    
    maxSize = maxSize*2; // double: circle radius

    // VOI in Volume Data Space
    x1 = markerCenterGuess[0] - maxSize;
    x2 = markerCenterGuess[0] + maxSize;
    y1 = markerCenterGuess[1] - maxSize;
    y2 = markerCenterGuess[1] + maxSize;
    z1 = markerCenterGuess[2] - maxSize;
    z2 = markerCenterGuess[2] + maxSize;

    // extract voi
    voiFilter->SetInput(calibVol->GetImageData());  
    voiFilter->SetOutputWholeExtent(x1,x2,y1,y2,z1,z2);
    voiFilter->Update();

    // median filter
    medianFilter->SetInput(voiFilter->GetOutput());
    medianFilter->SetKernelSize(3,3,3);
    medianFilter->Update();

    // threshold filter
    double range[2];
    calibVol->GetImageData()->GetScalarRange(range);        
    double nThreshold = (range[1]-range[0])* (thresh/100.0) + range[0];
    thresholdFilter->SetInput(medianFilter->GetOutput());
    thresholdFilter->ThresholdByUpper(nThreshold);  
    thresholdFilter->SetOutValue(range[0]);
    thresholdFilter->SetInValue(range[1]);
    thresholdFilter->Update();
    
    // the output voi to use
    vtkImageData *binaryVOI = thresholdFilter->GetOutput();


    /*
    // write the image to see if it is correct
    vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
    writer->SetFileName("VOI.mha");
    writer->SetInput(binaryVOI);
    writer->Write();
    writer->Delete();
*/


    // number of slices generated from this dataset (Volume Of Interest)
    int nTotalSlices = 0;
    
    double Pv1[3];
    double PPoint1[3];

    // Point 1: anywhere along the axis
    if ( fabs(normal[0])<fabs(normal[1])) {
        PPoint1[0]=1; PPoint1[1]=0; PPoint1[2]=0;
    } else {
        PPoint1[0]=0; PPoint1[1]=1; PPoint1[2]=0;
    }
    vtkMath::Normalize(PPoint1);
    vtkMath::Cross(normal, PPoint1, Pv1);
    vtkMath::Normalize(Pv1);

    // Point 2: perpendicular to P1
    double Pv2[3];
    vtkMath::Cross(normal, Pv1, Pv2);
    vtkMath::Normalize(Pv2);

    
    // origin
    double Dorigin[3];
    Dorigin[0] = 0.0;
    Dorigin[1] = 0.0;
    Dorigin[2] = 0.0;

    // Data spacing
    double *Dspacing = binaryVOI->GetSpacing();

    // Otherwise things will get complicated!
    double spacingX= 1.0;
    double spacingY= 1.0;

    // Data extent
    int *Dextent = binaryVOI->GetExtent();
    double DextentDbl[6];
    DextentDbl[0]=Dextent[0]; DextentDbl[1]=Dextent[1]; DextentDbl[2]=Dextent[2];
    DextentDbl[3]=Dextent[3]; DextentDbl[4]=Dextent[4]; DextentDbl[5]=Dextent[5];

    // slice plane origin: Starts from the clicked coordinate
    double POrigin[3];
    POrigin[0] = markerCenterGuess[0]; // Marker position --> POrigin
    POrigin[1] = markerCenterGuess[1];
    POrigin[2] = markerCenterGuess[2];

    double DPointOrigin[3];
    DPointOrigin[0]=(POrigin[0]-Dorigin[0])/spacing[0];
    DPointOrigin[1]=(POrigin[1]-Dorigin[1])/spacing[1];
    DPointOrigin[2]=(POrigin[2]-Dorigin[2])/spacing[2];
    
    // bounds checking max error
    double delta[3];
    delta[0]=delta[1]=delta[2]=0.001;


    // slicing steps
    double slicingSteps[3];
    slicingSteps[0] = normal[0]; // todo calculate from spacing!
    slicingSteps[1] = normal[1];
    slicingSteps[2] = normal[2];

    bool lHadSteps=false; // debug

    // Go back to the first Origin in the VOI
    while (vtkMath::PointIsWithinBounds(DPointOrigin, DextentDbl, delta) ) 
    {
        POrigin[0]=POrigin[0]-slicingSteps[0];
        POrigin[1]=POrigin[1]-slicingSteps[1];
        POrigin[2]=POrigin[2]-slicingSteps[2];
        DPointOrigin[0]=(POrigin[0]-Dorigin[0])/spacing[0];
        DPointOrigin[1]=(POrigin[1]-Dorigin[1])/spacing[1];
        DPointOrigin[2]=(POrigin[2]-Dorigin[2])/spacing[2];
        lHadSteps=true;
    }
    /// \todo if (!lHadSteps) write 'nothing found!'; - it should never happen!

    // Too much, one step forward (no longer within bounds)
    POrigin[0]=POrigin[0]+slicingSteps[0];
    POrigin[1]=POrigin[1]+slicingSteps[1];
    POrigin[2]=POrigin[2]+slicingSteps[2];
    DPointOrigin[0]=(POrigin[0]-Dorigin[0])/spacing[0];
    DPointOrigin[1]=(POrigin[1]-Dorigin[1])/spacing[1];
    DPointOrigin[2]=(POrigin[2]-Dorigin[2])/spacing[2];

    // Reslicing axes: Rotate, translate & scale
    vtkMatrix4x4 *ResliceAxes = vtkMatrix4x4::New();    
    ResliceAxes->Identity();

    // reslicing axes rotation (rotate & translate)
    vtkMatrix4x4 *ResliceAxesRotate = vtkMatrix4x4::New();  
    ResliceAxesRotate->Identity();

    // Direction cosines
    for (int i=0; i<3; i++) {
        ResliceAxesRotate->SetElement(0,i,Pv1[i]);
        ResliceAxesRotate->SetElement(1,i,Pv2[i]);
        ResliceAxesRotate->SetElement(2,i,normal[i]);
    }
    ResliceAxesRotate->Transpose();

    // debug: Determinant is 1.0
    if ( !this->DoubleEqual(ResliceAxesRotate->Determinant(), 1.0) )
        return false;

    // Magnify 3x3x1
    vtkMatrix4x4  *ResliceAxesScale = vtkMatrix4x4::New();  
    ResliceAxesScale->Identity();
    ResliceAxesScale->SetElement(0,0,1.0/2.0);
    ResliceAxesScale->SetElement(1,1,1.0/2.0);

    // Final matrix
    vtkMatrix4x4::Multiply4x4( ResliceAxesRotate, ResliceAxesScale, ResliceAxes );
    ResliceAxes->SetElement(0,3,POrigin[0]);
    ResliceAxes->SetElement(1,3,POrigin[1]);
    ResliceAxes->SetElement(2,3,POrigin[2]);
    ResliceAxes->Modified(); // Will not work without this!

    // Affine transformation for point conversion
    vtkTransform *ResliceTransform = vtkTransform::New();   
    ResliceTransform->SetMatrix(ResliceAxes);

    // Create slicer object
    vtkImageReslice *imageReslicer = vtkImageReslice::New();    
    imageReslicer->SetInput(binaryVOI);
    imageReslicer->SetResliceAxes(ResliceAxes);
    imageReslicer->SetOutputDimensionality(2);

    // Output parameters
    // Gobbi: I _always_ call SetOutputOrigin, SetOutputExtent, and SetOutputSpacing
    imageReslicer->SetOutputSpacing(spacingX,spacingY,1);
    int dispsize = 2.0*16; /// \todo - make the "16" a variable!
    imageReslicer->SetOutputOrigin(-(dispsize/2), -(dispsize/2), 0);
    imageReslicer->SetOutputExtent(0,dispsize,0,dispsize,0,0);
    imageReslicer->SetBackgroundColor( 0.0, 0.0, 0.0, 0.0 );
    imageReslicer->SetInterpolationModeToCubic();
    imageReslicer->Update();

    // Origin and spacing of the resliced image
    vtkImageData *inData = imageReslicer->GetOutput();
    double *origin = inData->GetOrigin();   
    int *extent = inData->GetWholeExtent();

    
    // The resliced volume is 3D, but with Z1=0 and Z2=0
    int comp=inData->GetNumberOfScalarComponents();
    
    
    // width and height used _only_ to allocate memory
    int width=abs(extent[0]-extent[1])+1;
    int height=abs(extent[2]-extent[3])+1;
    int tempStorageSize = width*height;
    unsigned int *tempStorage;
    double ix, iy, iz;

    // It will be used for counting the circle centers (optimization)
    tempStorage = new unsigned int[tempStorageSize+8];

    // Looking for mean center along the resliced planes
    double meanx=0.0, meany=0.0, meanz=0.0;
    int meanNr=0;

    // For every every slice
    while (vtkMath::PointIsWithinBounds(DPointOrigin, DextentDbl, delta) ) {
        nTotalSlices++;
        bool lCircleFound;

        // if returns false, the center was not found (not enough votes?)
        lCircleFound = CalculateCircleCenter(inData, tempStorage, tempStorageSize,  nThreshold, 2*radius/10.0, &ix, &iy, &iz, 18, true);

        if (lCircleFound) {
            double scanner[3];

            double pos[3];
            pos[0]=ix;
            pos[1]=iy;
            pos[2]=iz;

            // ResliceDataCoord -> Data Point
            ResliceTransform->TransformPoint(pos, scanner);

            // Mean circle center - for visualization only
            meanx += scanner[0];
            meany += scanner[1];
            meanz += scanner[2];
            meanNr++;

            // Data Point --> World (Scanner coord)
            double ijkIn[4] = {scanner[0], scanner[1], scanner[2], 1};
            double rasOut[4];   
            ijkToRAS->MultiplyPoint(ijkIn, rasOut);

            // Collect circle centers into CoordVector
            itk::Point<double,3> coord;
            coord[0] = rasOut[0];
            coord[1] = rasOut[1];
            coord[2] = rasOut[2];
            this->CoordinatesVector.push_back(coord);

            //ShowSegmentedCenter(scanner[0],scanner[1],scanner[2]);
        } // if Circle center found

        // POrigin: move along the normal (get next slice)
        POrigin[0]=POrigin[0]+slicingSteps[0];
        POrigin[1]=POrigin[1]+slicingSteps[1];
        POrigin[2]=POrigin[2]+slicingSteps[2];
        DPointOrigin[0]=(POrigin[0]-Dorigin[0])/spacing[0];
        DPointOrigin[1]=(POrigin[1]-Dorigin[1])/spacing[1];
        DPointOrigin[2]=(POrigin[2]-Dorigin[2])/spacing[2];


        // The new reslicing axes
        ResliceAxesRotate->SetElement(0,3,0);
        ResliceAxesRotate->SetElement(1,3,0);
        ResliceAxesRotate->SetElement(2,3,0);
        vtkMatrix4x4::Multiply4x4( ResliceAxesRotate, ResliceAxesScale, ResliceAxes );
        ResliceAxes->SetElement(0,3,POrigin[0]);
        ResliceAxes->SetElement(1,3,POrigin[1]);
        ResliceAxes->SetElement(2,3,POrigin[2]);
        ResliceAxes->Modified();

        // ResliceTransform: new reslicing matrix
        ResliceTransform->SetMatrix(ResliceAxes);

        // ImageReslice: new reslicing matrix
        imageReslicer->SetResliceAxes(ResliceAxes);

        // Do it! (Reslice)
        imageReslicer->Update();
    } // while(all slices)

    delete[] tempStorage;
    imageReslicer->Delete();
    ResliceAxes->Delete();
    ResliceAxesRotate->Delete();
    ResliceAxesScale->Delete();

    
    // Return value: compute mean value of circle centers (for visualisation only)
    if (meanNr>0) {     
        markerCenterGuess[0]=meanx/meanNr;
        markerCenterGuess[1]=meany/meanNr;
        markerCenterGuess[2]=meanz/meanNr;      
        return true; // done, we found at least 1 center
    }
    return false; // failed, no Circle center found
}


//------------------------------------------------------------------------------------------------------------
/// \brief Calculates the circle center.
/// Assumes spacingX == spacingY (for the circle)
/// Assumes 2D inData
bool vtkTRProstateBiopsyLogic::CalculateCircleCenter(vtkImageData *inData, unsigned int *tempStorage, int tempStorageSize, double nThersholdVal, double nRadius, double *gx, double *gy, double *gz, int nVotedNeeded, bool lDebug)
{/// \todo Make all calculations floating point!
    
    // "0" the Temporal data storage
    memset(tempStorage, 0, tempStorageSize*sizeof(unsigned int)); // tCircleAccumulate = int 
    
    // spacing should not be 0!
    double spacing[3];
    inData->GetSpacing(spacing);

    if ( this->DoubleEqual(spacing[0],0.0) || this->DoubleEqual(spacing[1],0.0) || this->DoubleEqual(spacing[2],0.0) )  {
        return false;
    }

    // assume spacingY==spacingY -  depends on vtkImageReslicer settings!
    

    // convert mm to pixel
    double r = nRadius/fabs(spacing[0]);

    // Calculate width & height
    int *extent = inData->GetWholeExtent();
    int width = abs(extent[0]-extent[1])+1;
    int height = abs(extent[2]-extent[3])+1;
    //assert( width*height <= tempStorageSize );
    tempStorageSize=width*height;

    // Increment circle centers (possibile centers are also on a circle)
    vtkDataArray * da = inData->GetPointData()->GetScalars();

    double r_exp=r*r; // r^2
    double x_exp;     // x^2
    double d;
    int x=-1;
    int y=0;
    unsigned int max = 0;
    int x1, x2, y1, y2;
    int idx;

    // Cumulate circle centers
    for (int i=0;i<tempStorageSize;i++) {
        x++;
        if (x>=width) {
            x=0; y++;
        }
        if ( *(da->GetTuple(i)) < nThersholdVal ) continue;

        x1=x-vtkMath::Round(r);
        x2=x+vtkMath::Round(r);

        if ( x1>=0 ) {
            idx=x1+y*width;
            if (idx>=tempStorageSize)
                idx=tempStorageSize-1;
            tempStorage[idx]++;
            if (tempStorage[idx]>max)
                max=tempStorage[idx];
        }

        if ( x2<width ) {
            idx=x2+y*width;
            if (idx>=tempStorageSize)
                idx=tempStorageSize-1;
            tempStorage[idx]++;
            if (tempStorage[idx]>max)
                max=tempStorage[idx];
        }

        if (x1<0)
            x1=0;
        if (x2>=width)
            x2=width-1;


        for (int cx=x1;cx<=x2;cx++) {
            x_exp=(x-cx)*(x-cx); // (x-j)^2 
            if (x_exp>r_exp) { // out
                continue;
            }
            d=sqrt( r_exp-x_exp );
            if (d<1.0) 
                d=1.0;

            y1=vtkMath::Round(y-d); // top
            y2=vtkMath::Round(y+d); // bottom

            if ( (y1>=0) && (y1<height) ) { // top
                idx=cx+y1*width;
                if (idx>=tempStorageSize)
                    idx=tempStorageSize-1;
                tempStorage[idx]++;
                if (tempStorage[idx]>max) 
                    max=tempStorage[idx];
            } // top

            if ( (y2>=0) && (y2<height) ) { // bottom
                idx=cx+y2*width;
                if (idx>=tempStorageSize)
                    idx=tempStorageSize-1;
                tempStorage[idx]++;
                if (tempStorage[idx]>max)
                    max=tempStorage[idx];
            } // bottom
        } // for cx
    } // for i - Cumulate circle centers

    // calculate mean center position
    x=y=0;
    double meanx=0.0, meany=0.0;
    int meannr=0;
    for (int i=0;i<tempStorageSize;i++) {
        if (tempStorage[i]==max) {
            meanx+=x;
            meany+=y;
            meannr++;
        }
        x++;
        if (x>=width) {
            x=0;
            y++;
        }
    }

    // since max exists for sure!
    //assert(meannr>0);

    // Get the mean of the centers
    *gx=meanx/meannr;
    *gy=meany/meannr;
    *gz=extent[4];
    //assert(extent[4]==extent[5]); // 2D

    *gx-=( (width-1)  /2.0 );
    *gy-=( (height-1) /2.0 ); 

    // debug
    x=y=0;
    if (lDebug) {
    //  f << "Max:" << max << " Needed: " << nVotedNeeded << " 0:" << *gx << " Y:" << *gy << " Centers:" << meannr << std::endl;
        //if (max>=nVotedNeeded) f << "Center found!" << endl;
        //f << "Width:" << width << 
        //  " Height:" << height << 
        //  " DataSize:" << tempStorageSize << 
        //  " (" << width*height << ")" << std::endl;
        //f << "Radius:" << nRadius << " In pixels:" << r << std::endl;
        //f << extent[0] << " - " << extent[1] << "   "   << extent[2] << " - " << extent[3] << "   "  << extent[4] << " - " << extent[5] << std::endl;
        //f << std::endl;
    }

/*  if (lDebug) {
        f << std::endl << std::endl;
        x=y=0;
        for (int i=0;i<tempStorageSize;i++) {
            if ( (x==vtkMath::Round(meanx/meannr)) && (y==vtkMath::Round(meany/meannr)) ) {
                f << "0 ";
            } else {
                if ( *(da->GetTuple(i)) > nThersholdVal ) {
                    f << "H ";
                } else {
                    f << "· ";
                }
            }

            x++;
            if (x>=width) {
                f << std::endl;
                x=0;
                y++;
            }
        }

        f << std::endl;
    } // debug
*/
    if (max<nVotedNeeded) 
        return false;

    return true;
}



/// Calculate probe position from fiducial pairs
void vtkTRProstateBiopsyLogic::FindProbe(double P1[3], double P2[3], double v1[3], double v2[3], double I1[3], double I2[3], double initialAngle)
{
    

    /*
    // plane normal to display lines
    window->GetTransformedNormal(normal);
    normal[0]=-(normal[0]/5.0);
    normal[1]=-(normal[1]/5.0);
    normal[2]=-(normal[2]/5.0);

    // to show the lines found
    #define DDIST 120.0 
    double e1[3], e2[3];

    // Transfer to data coordinate system
    // Assume: patient HFS
    // 0 is increasing toward the patient's left
    // Y is increasing to the patient's posterior
    // Z is increasing to the head (superior) direction
    // DICOm: LPS, Algor: LPS, Normally: RAS

    if (v1[2]<0) {
        v1[0]=-v1[0];v1[1]=-v1[1];v1[2]=-v1[2];
    }

    if (v2[2]<0) {
        v2[0]=-v2[0];v2[1]=-v2[1];v2[2]=-v2[2];
    }

    // P1, v1 from Linefinder
    e1[0]=P1[0]+(-DDIST)*v1[0];
    e1[1]=P1[1]+(-DDIST)*v1[1];
    e1[2]=P1[2]+(-DDIST)*v1[2];

    e2[0]=P1[0]+(+DDIST)*v1[0];
    e2[1]=P1[1]+(+DDIST)*v1[1];
    e2[2]=P1[2]+(+DDIST)*v1[2];
    FidLine1->SetPoint1(e1[0]+normal[0], e1[1]+normal[1], e1[2]+normal[2]);
    FidLine1->SetPoint2(e2[0]+normal[0], e2[1]+normal[1], e2[2]+normal[2]);

    // P2, v2 from Linefinder
    e1[0]=P2[0]+(-DDIST)*v2[0];
    e1[1]=P2[1]+(-DDIST)*v2[1];
    e1[2]=P2[2]+(-DDIST)*v2[2];

    e2[0]=P2[0]+(+DDIST)*v2[0];
    e2[1]=P2[1]+(+DDIST)*v2[1];
    e2[2]=P2[2]+(+DDIST)*v2[2];
    FidLine2->SetPoint1(e1[0]+normal[0], e1[1]+normal[1], e1[2]+normal[2]);
    FidLine2->SetPoint2(e2[0]+normal[0], e2[1]+normal[1], e2[2]+normal[2]);

    // show lines
    if (line_window) {
        line_window->RenderAll();
    }

    */

    //this->OverlayFiducialAxisLines();

    // Axel algorithm, part 1 (see Targeting for part 2)

    // Find intersecting points
    double p12[3];
    p12[0]=P2[0]-P1[0];
    p12[1]=P2[1]-P1[1];
    p12[2]=P2[2]-P1[2];
    // vtkMath::Normalize(p12); - not ntomalized here!
    double v12=vtkMath::Dot(v1,v2);

    double t1=(vtkMath::Dot(p12,v1)-vtkMath::Dot(p12,v2)*v12)/(1-v12*v12);
    double t2=-(vtkMath::Dot(p12,v2)-vtkMath::Dot(p12,v1)*v12)/(1-v12*v12);

    // I1 global
    I1[0] = P1[0] + t1*v1[0];
    I1[1] = P1[1] + t1*v1[1];
    I1[2] = P1[2] + t1*v1[2];
    this->GetTRProstateBiopsyModuleNode()->Setv1(v1);
    this->GetTRProstateBiopsyModuleNode()->SetI1(I1);

    // I2 global
    I2[0] = P2[0] + t2*v2[0];
    I2[1] = P2[1] + t2*v2[1];
    I2[2] = P2[2] + t2*v2[2];
    this->GetTRProstateBiopsyModuleNode()->Setv2(v2);
    this->GetTRProstateBiopsyModuleNode()->SetI2(I2);

    //Calculate and display distance and angle
    double Id[3];
    Id[0]=I2[0]-I1[0];
    Id[1]=I2[1]-I1[1];
    Id[2]=I2[2]-I1[2]; 
    double d=vtkMath::Norm(Id);
    

    // Angle between lines (alpha global)
    double alpha= acos(  fabs(vtkMath::Dot(v1,v2)) );
    double alpha_degree=alpha*180.0/vtkMath::Pi();
        
    // save results to module node
    this->GetTRProstateBiopsyModuleNode()->SetAxesAngleDegrees(alpha_degree);
    this->GetTRProstateBiopsyModuleNode()->SetRobotRegistrationAngleDegrees(initialAngle);
    this->GetTRProstateBiopsyModuleNode()->SetAxesDistance(d);
    this->GetTRProstateBiopsyModuleNode()->SetFiducialSegmentationRegistrationComplete(true);
    // Save segmented data to FoR
    /*
    tFiducialPositions & Marker=this->FoR->GetFiducialPosPointer();
    Marker.registration_angle_rad=(spinAngle->GetValue()*vtkMath::Pi())/180.0;  
    
    Marker.alpha_rad=alpha;
    FLOAT_COPY(I1,Marker.I1);
    FLOAT_COPY(I2,Marker.I2);
    FLOAT_COPY(v1,Marker.v1);
    FLOAT_COPY(v2,Marker.v2);
    Marker.lEmpty=false;

    */
}

    struct PointTooFar {
            PointTooFar(double defp[3],double defv[3],double max_dist):defp_p(defp),defv_p(defv),max_distance(max_dist) {}
            double *defp_p;
            double *defv_p;
            double max_distance;
            
            bool operator()( itk::Point<double, 3> x) {
                return vtkTRProstateBiopsyMath::PointDistanceFromLineM(x,defp_p,defv_p)>max_distance;
            }
            };

/// Remove outlaying points
void vtkTRProstateBiopsyLogic::RemoveOutliners(double P_[3], double v_[3], double def1[3],double def2[3])
{
    // Initial threshold to disregard center points, which are too far away from initial line.
    double outlier_thres_initial=4;

    // Threshold to call center points outliers.
    double outlier_thres=.5;

    
    std::vector<itk::Point<double,3> > CoordVectorCopy (this->CoordinatesVector);
    

    
    int vecSize = CoordVectorCopy.size();
    double def_v[3];
    def_v[0]=def2[0]-def1[0];
    def_v[1]=def2[1]-def1[1];
    def_v[2]=def2[2]-def1[2];
    vtkMath::Normalize(def_v);  

    // Remove outside points (>4mm)
    CoordVectorCopy.erase( std::remove_if( CoordVectorCopy.begin(), CoordVectorCopy.end(), PointTooFar(def1,def_v,outlier_thres_initial) ),
                  CoordVectorCopy.end() );

    vecSize = CoordVectorCopy.size();

    double dist, max_distance;
    std::vector<itk::Point<double,3> >::const_iterator i;

    do {
        this->Linefinder(P_,v_,CoordVectorCopy);
        max_distance=0;
        for (i=CoordVectorCopy.begin(); i!=CoordVectorCopy.end(); ++i){
            dist=vtkTRProstateBiopsyMath::PointDistanceFromLineM(*i,P_,v_);
            if (dist>max_distance) max_distance=dist;
        }
        if (max_distance>=outlier_thres) {
            CoordVectorCopy.erase( std::remove_if( CoordVectorCopy.begin(), CoordVectorCopy.end(), PointTooFar(P_,v_,max_distance-0.001) ),
                 CoordVectorCopy.end() );
        }
    } while (max_distance>=outlier_thres);

    
    
}


/// Find the best line fitting CoordVector
void vtkTRProstateBiopsyLogic::Linefinder(double P_[3], double v_[3], std::vector<itk::Point<double,3> > CoordVector)
{// Axel line algorithm



    // CoordVector is "A"
    int m=CoordVector.size();
    if (m<2) {
        // At least two points are necessary to calculate a line
        return;
    }

    std::vector<itk::Point<double,3> >::iterator it;

    // calculate the mean point P
    // P=sum(A)/m;
    double P[3];
    P[0] = 0.0;
    P[1] = 0.0;
    P[2] = 0.0;

    it = CoordVector.begin();
    while (it != CoordVector.end()) {
        P[0] += (*it)[0];
        P[1] += (*it)[1];
        P[2] += (*it)[2];
        it++;
    }
    P[0]/=m;
    P[1]/=m;
    P[2]/=m;

    // P_m=(P'*ones(1,m))';
    // subtract the mean point from each matrix A coordinate
    // B=A-P_m;
    /*it = CoordVector.begin();
    while (it != CoordVector.end()) {
        (*it)[0] -= P[0];
        (*it)[1] -= P[1];
        (*it)[2] -= P[2];
        it++;
    }
    */
    double curVal[3];
    for (int k = 0; k<CoordVector.size(); k++)
    {
      curVal[0] = CoordVector[k][0];
      curVal[1] = CoordVector[k][1];
      curVal[2] = CoordVector[k][2];
      CoordVector[k][0] -= P[0];
      CoordVector[k][1] -= P[1];
      CoordVector[k][2] -= P[2];
      curVal[0] = CoordVector[k][0];
      curVal[1] = CoordVector[k][1];
      curVal[2] = CoordVector[k][2];
    }
    vnl_matrix<double> A(m, 3);
    it = CoordVector.begin();
    int i =0;
    while ( (it != CoordVector.end()) && (i<m) ) {
        A(i,0) = (*it)[0];
        A(i,1) = (*it)[1];
        A(i,2) = (*it)[2];
        it++;
        i++;
    }
    vnl_svd<double> svdData(A);
    /*try {
        svdData.recompose();
        //nmrSVD(A, svdData);
    } catch(...) {
        assert(false); // "An exception occured, check cisstLog.txt." 
        return;
    }*/

    P_[0]=P[0]; P_[1]=P[1]; P_[2]=P[2];

    const vnl_matrix<double>& V = svdData.V();
    /*for (int l = 0; l<3; l++)
        for (int m = 0; m<3; m++)
            slicerCerr(V(l,m));*/
    vnl_matrix<double> trV = vnl_transpose(V);

    v_[0]=trV(0,0);
    v_[1]=trV(0,1);
    v_[2]=trV(0,2);
    vtkMath::Normalize(v_);
}
//-------------------------------------------------------------------------------
/// Calculations to find the targeting parameters (point -> rotation & deepth)
bool vtkTRProstateBiopsyLogic::FindTargetingParams(vtkTRProstateBiopsyTargetDescriptor *target)
{
    if (!this->GetTRProstateBiopsyModuleNode())
        return false;
    if (!target) 
        return false;
    if (!this->GetTRProstateBiopsyModuleNode()->GetFiducialSegmentationRegistrationComplete()) 
        return false;

    // Axel algorithm, part 1 :
    //  When fiducials segmented

    // Axel algorithm, part 2
    //  Calculate Targeting Parameters

    double ras[3]; // Target
    target->GetRASLocation(ras[0], ras[1], ras[2]); 
    
    double lps[3];
    lps[0] = -ras[0];
    lps[1] = -ras[1];
    lps[2] = ras[2];
    
    double I1[3];
    this->GetTRProstateBiopsyModuleNode()->GetI1(I1);
    I1[0] = -I1[0];
    I1[1] = -I1[1];

    double v1[3];
    this->GetTRProstateBiopsyModuleNode()->Getv1(v1);
    v1[2] = -v1[2];

    double v2[3];
    this->GetTRProstateBiopsyModuleNode()->Getv2(v2);
    v2[2] = -v2[2];


    // Insertion deepth offset (in mm)
    double offset;
    offset = target->GetNeedleTypeDepth();
    // offset = -13; - Biopsy needle
    // offset = 1.5; - Seed placement needle, gold markers
    
    
    double H_zero[3];
    
    // Original hinge point before rotation
    double l=14.5/sin(this->GetTRProstateBiopsyModuleNode()->GetAxesAngleDegrees() * vtkMath::Pi()/180);

    double H_before[3];
    H_before[0] = I1[0] - l*v2[0];
    H_before[1] = I1[1] - l*v2[1];
    H_before[2] = I1[2] - l*v2[2];

    // Axis of Rotation is v1
    // ToZeroRotMatrix=eye(3)+sin(-registration_angle_rad)*skew+skew*skew*(1-cos(-registration_angle_rad));
    RotatePoint(H_before, this->GetTRProstateBiopsyModuleNode()->GetRobotRegistrationAngleDegrees()*vtkMath::Pi()/180, this->GetTRProstateBiopsyModuleNode()->GetAxesAngleDegrees() * vtkMath::Pi()/180, v1, I1, H_zero);


    // The rotated (to zero) needle axis
    double v2_zero[3];
    v2_zero[0] = I1[0] - H_zero[0];
    v2_zero[1] = I1[1] - H_zero[1];
    v2_zero[2] = I1[2] - H_zero[2];
    vtkMath::Normalize(v2_zero);

    //  Calculate Rotation
    // n1: Orthogonal to probe (v1) and "0" needle
    double n1[3];
    vtkMath::Cross(v1,v2_zero,n1);
    vtkMath::Normalize(n1);

    // Distance from Target T to the plane with direction, is: 
    double tVec[3];
    tVec[0] = lps[0] - I1[0];
    tVec[1] = lps[1] - I1[1];
    tVec[2] = lps[2] - I1[2];

    // n2: 0 needle axes at 90 deg
    double n2[3];
    vtkMath::Cross(v1,n1,n2);
    vtkMath::Normalize(n2);

    // Distance from Target T to the plane with direction, is: 
    double x = -vtkMath::Dot(n2,tVec);
    double y = -vtkMath::Dot(n1,tVec);

    // Rotation with atan2
    double rotation = atan2(y,x);
    double rotation_degree = -rotation*180.0/vtkMath::Pi();

    // Text -------------------------------------------------------------
    target->SetRotation(rotation_degree);

    // RotMatrix=eye(3)+sin(-rotation)*skew+skew*skew*(1-cos(-rotation));
    double H_after[3];
    RotatePoint(H_zero, rotation, this->GetTRProstateBiopsyModuleNode()->GetAxesAngleDegrees()*vtkMath::Pi()/180, v1, I1, H_after);
        
    // Needle Trajectory Vector
    double v_needle[3];
    v_needle[0] = lps[0] - H_after[0];
    v_needle[1] = lps[1] - H_after[1];
    v_needle[2] = lps[2] - H_after[2];

    target->SetHingePosition(H_after[0], H_after[1], H_after[2]);

    /// \todo Pipe from H_after (along to v_needle traj.) ending "Overshoot" mm after the target
    // T[?]+overshoot*v_needle[?]

    double mNorm = vtkMath::Normalize(v_needle);

    // Needle Angle: fix it!
    double needle_angle = acos( 1.0 * vtkMath::Dot(v_needle,v1) );

    double needle_angle_degree = needle_angle*180.0/vtkMath::Pi();
    // The absolute needle angle to reach the target is %d degrees
        
    /// \todo Temp function! - compensate (no cheating?) // ccs
    double alpha_degree = this->GetTRProstateBiopsyModuleNode()->GetAxesAngleDegrees();

    // Cheating ot not? 
    needle_angle_degree = needle_angle_degree - (alpha_degree-37.0); // cheating

    if ( (needle_angle_degree < 17.5) || (needle_angle_degree > 37.0 + 1.8 /* !!! */ ) ) {
        // can't reach the target!
        target->SetIsOutside(true);
        //target->SetColor(0.3, 0.3, 1.0);
    } else {
        target->SetIsOutside(false);
    }

    // Text -------------------------------------------------------------
    target->SetNeedleAngle(needle_angle_degree);

    // Insertion depth
    double needle_angle_initial = alpha_degree*vtkMath::Pi()/180;

    double n_length = 29.16 + 4 + 14.5 / sin(needle_angle_initial);
    double n_slide = n_length-14.5/sin(needle_angle);

    double insM[3];
    insM[0] = H_after[0] - lps[0];
    insM[1] = H_after[1] - lps[1];
    insM[2] = H_after[2] - lps[2];

    double n_insertion=vtkMath::Norm(insM)+n_slide+offset;

    // Text -------------------------------------------------------------
    target->SetDepthCM(n_insertion/10.0);

    return true;
}


/// Internal function used by FindTargetingParams
bool vtkTRProstateBiopsyLogic::RotatePoint(double H_before[3], double rotation_rad, double alpha_rad, double mainaxis[3], double I[3], /*out*/double H_after[3])
{
        double skew[3][3] = {
           {             0, -mainaxis[2],  mainaxis[1] },
           {  mainaxis[2],             0, -mainaxis[0] },
           { -mainaxis[1],  mainaxis[0],             0 }
        };

        // RotMatrix=eye(3)+sin(-rotation_rad)*skew+skew*skew*(1-cos(-rotation_rad));
        double RotMatrix[3][3];
        vtkMath::Identity3x3(RotMatrix);

        RotMatrix[0][0]+=sin(-rotation_rad)*skew[0][0];
        RotMatrix[0][1]+=sin(-rotation_rad)*skew[0][1];
        RotMatrix[0][2]+=sin(-rotation_rad)*skew[0][2];

        RotMatrix[1][0]+=sin(-rotation_rad)*skew[1][0];
        RotMatrix[1][1]+=sin(-rotation_rad)*skew[1][1];
        RotMatrix[1][2]+=sin(-rotation_rad)*skew[1][2];

        RotMatrix[2][0]+=sin(-rotation_rad)*skew[2][0];
        RotMatrix[2][1]+=sin(-rotation_rad)*skew[2][1];
        RotMatrix[2][2]+=sin(-rotation_rad)*skew[2][2];

        double skew2[3][3];
        vtkMath::Multiply3x3(skew, skew, skew2);

        skew2[0][0]*=1-cos(-rotation_rad);
        skew2[0][1]*=1-cos(-rotation_rad);
        skew2[0][2]*=1-cos(-rotation_rad);

        skew2[1][0]*=1-cos(-rotation_rad);
        skew2[1][1]*=1-cos(-rotation_rad);
        skew2[1][2]*=1-cos(-rotation_rad);

        skew2[2][0]*=1-cos(-rotation_rad);
        skew2[2][1]*=1-cos(-rotation_rad);
        skew2[2][2]*=1-cos(-rotation_rad);

        RotMatrix[0][0]+=skew2[0][0];
        RotMatrix[0][1]+=skew2[0][1];
        RotMatrix[0][2]+=skew2[0][2];

        RotMatrix[1][0]+=skew2[1][0];
        RotMatrix[1][1]+=skew2[1][1];
        RotMatrix[1][2]+=skew2[1][2];

        RotMatrix[2][0]+=skew2[2][0];
        RotMatrix[2][1]+=skew2[2][1];
        RotMatrix[2][2]+=skew2[2][2];

        double rotation_point[3];
        rotation_point[0]=I[0]-14.5/tan(alpha_rad)*mainaxis[0];
        rotation_point[1]=I[1]-14.5/tan(alpha_rad)*mainaxis[1];
        rotation_point[2]=I[2]-14.5/tan(alpha_rad)*mainaxis[2];

        double rotation_vector[3];
        rotation_vector[0]=H_before[0]-rotation_point[0];
        rotation_vector[1]=H_before[1]-rotation_point[1];
        rotation_vector[2]=H_before[2]-rotation_point[2];

        double after_rotation_vector[3];
        vtkMath::Multiply3x3(RotMatrix, rotation_vector, after_rotation_vector);

        H_after[0]=rotation_point[0]+after_rotation_vector[0];
        H_after[1]=rotation_point[1]+after_rotation_vector[1];
        H_after[2]=rotation_point[2]+after_rotation_vector[2];

        return true;
}



//--------------------------------------------------------------------------------------
bool vtkTRProstateBiopsyLogic::AddTargetToNeedle(std::string needleType, double rasLocation[3], unsigned int & targetDescIndex, unsigned int & fiducialIndex)
{
  if (!this->GetTRProstateBiopsyModuleNode())
      return false;

  // 1) get the needle information based on needleType 
  for (int needleIndex=0; needleIndex < this->GetTRProstateBiopsyModuleNode()->GetNumberOfNeedles(); needleIndex++)
    {
    if (this->GetTRProstateBiopsyModuleNode()->GetNeedleType(needleIndex) == needleType)
        break;
    }

  vtkTRProstateBiopsyTargetDescriptor *targetDesc = vtkTRProstateBiopsyTargetDescriptor::New();
  targetDesc->SetRASLocation(rasLocation[0], rasLocation[1], rasLocation[2]);
  targetDesc->SetNeedleType(this->GetTRProstateBiopsyModuleNode()->GetNeedleType(needleIndex),this->GetTRProstateBiopsyModuleNode()->GetNeedleUID(needleIndex), this->GetTRProstateBiopsyModuleNode()->GetNeedleDepth(needleIndex), this->GetTRProstateBiopsyModuleNode()->GetNeedleOvershoot(needleIndex));

  // 2) calculate targeting parameters for active needle, store in a target descriptor
  if(!this->FindTargetingParams(targetDesc))
    {
    // error message
    // to do
    return false;
    }
  // 3) store the target descriptors in a list
  targetDescIndex = this->GetTRProstateBiopsyModuleNode()->AddTargetDescriptor(targetDesc);
  // 4) add the target to targets FiducialListNode, searching which list to add to, knowing which needle list is being populated to?
  if (!this->GetTRProstateBiopsyModuleNode()->AddTargetToFiducialList(rasLocation, needleIndex, targetDescIndex+1, fiducialIndex))
  {
  // error
  // to do
  return false;
  }
  // 5) visual feedback of creation of the click
  
  return true;
}
