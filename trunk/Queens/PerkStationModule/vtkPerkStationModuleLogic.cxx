/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkPerkStationModuleLogic.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationModule.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkSlicerColorLogic.h"

#include <itksys/SystemTools.hxx>

#include <algorithm>

#include <vtksys/stl/string>
#include <vtksys/SystemTools.hxx>
#include "vtkMath.h"


vtkPerkStationModuleLogic* vtkPerkStationModuleLogic::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPerkStationModuleLogic");
  if(ret)
    {
      return (vtkPerkStationModuleLogic*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPerkStationModuleLogic;
}


//----------------------------------------------------------------------------
vtkPerkStationModuleLogic::vtkPerkStationModuleLogic()
{
  this->PerkStationModuleNode = NULL;
}

//----------------------------------------------------------------------------
vtkPerkStationModuleLogic::~vtkPerkStationModuleLogic()
{
  vtkSetMRMLNodeMacro(this->PerkStationModuleNode, NULL);
}

//----------------------------------------------------------------------------
void vtkPerkStationModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
}
//---------------------------------------------------------------------------
bool vtkPerkStationModuleLogic::ReadConfigFile(istream &file)
{
  char currentLine[256];  
  char* attName = "";
  char* attValue = "";
  char* pdest;
  int nCharCount = 0;
  unsigned int indexEndOfAttribute = 0;
  unsigned int indexStartOfValue = 0;
  unsigned int indexEndOfValue = 0;

  // local variables to temporarily store the loaded values
  // we apply these values to MRML node only if all the requied fields are successfully loaded
  double tool_tip_offset[3] = {0.0, 0.0, 0.0};
  int needle_tool_port = 0, reference_body_tool_port = 0;
  std::vector<double> tracker_to_phantom_matrix, phantom_to_imageRAS_matrix;

  // flags to check if all the required fields are loaded or not
  bool is_tracker_to_phantom_matrix_read = false;
  bool is_phantom_to_imageRAS_matrix_read = false;

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
        if (!strcmp(attName, "NeedleToolPort"))
            {
            // read needle tool port number into a local variable, needle_port
            std::stringstream ss;
            ss << attValue;
            ss >> needle_tool_port;              
            }
        else if (!strcmp(attName, "ReferenceBodyToolPort"))
            {
            // read reference body tool port number into a local variable, reference_body_tool_port
            std::stringstream ss;
            ss << attValue;
            ss >> reference_body_tool_port;              
            }
        else if (!strcmp(attName, "TrackerToPhantomMatrix"))
            {
            // read tracker to phantom matrix into a local array, tracker_to_phantom_matrix
            std::stringstream ss;
            ss << attValue;
            double d;
            while (ss >> d)
              {
              tracker_to_phantom_matrix.push_back(d);
              }
            if (tracker_to_phantom_matrix.size()==16)
              {
              is_tracker_to_phantom_matrix_read = true;
              }
            else
              {
              vtkErrorMacro("TrackerToPhantomMatrix field in configuration file could not be loaded correctly.");
              }
            }
        else if (!strcmp(attName, "PhantomToImageMatrix"))
            {
            // read phantom to image matrix into a local array, phantom_to_imageRAS_matrix
            std::stringstream ss;
            ss << attValue;
            double d;
            while (ss >> d)
              {
              phantom_to_imageRAS_matrix.push_back(d);
              }
            if (phantom_to_imageRAS_matrix.size()==16)
              {
              is_phantom_to_imageRAS_matrix_read = true;
              }
            else
              {
              vtkErrorMacro("PhantomToImageMatrix field in configuration file could not be loaded correctly.");
              }
            }
        else if (!strcmp(attName, "ToolTipOffset"))
            {
            // read tool tip offset into a local array, tool_tip_offset
            std::stringstream ss;
            ss << attValue;
            double d;
            std::vector<double> tmpVec;
            while (ss >> d)
              {
              tmpVec.push_back(d);
              }
            if (tmpVec.size()==3)
              {
              memcpy(tool_tip_offset, &(tmpVec[0]), sizeof(double)*3);
              }
            }
         else
            {
            // the attribute name does not match to any of the defined attribute names.
            vtkErrorMacro("Attribute name " << attName << " is not defined.");
            }     
        
        }// end if testing for it is a valid attName

    } // end while going through the file

  // if TrackerToPhantomMatrix and PhantomToImageMatrix were properly loaded, we consider that the config file was loaded successfully.
  if (is_tracker_to_phantom_matrix_read && is_phantom_to_imageRAS_matrix_read)
    {
    // if successful, update MRML node and return 'true'
    this->GetPerkStationModuleNode()->SetNeedleToolPort(needle_tool_port);          
    this->GetPerkStationModuleNode()->SetReferenceBodyToolPort(reference_body_tool_port);             
    this->GetPerkStationModuleNode()->SetToolTipOffset(tool_tip_offset);

    vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    matrix->DeepCopy( &(tracker_to_phantom_matrix[0]) );
    this->GetPerkStationModuleNode()->SetTrackerToPhantomMatrix(matrix);
    matrix->DeepCopy( &(phantom_to_imageRAS_matrix[0]) );
    this->GetPerkStationModuleNode()->SetPhantomToImageRASMatrix(matrix);
    matrix->Delete();

    return true;
    } 
  else
    {
    // if failed, just return 'false'
    return false;
    }
}
//----------------------------------------------------------------------------------
char *vtkPerkStationModuleLogic::strrev(char *s,int n)
{
       int i=0;
       while (i<n/2)
       {
               *(s+n) = *(s+i);       //uses the null character as the temporary storage.
               *(s+i) = *(s + n - i -1);
               *(s+n-i-1) = *(s+n);
               i++;
       }
       *(s+n) = '\0';

       return s;
}

//----------------------------------------------------------------------------------
bool vtkPerkStationModuleLogic::DoubleEqual(double val1, double val2)
{
  bool result = false;
    
  if(fabs(val2-val1) < 0.0001)
      result = true;

  return result;
}

//---------------------------------------------------------------------------
void vtkPerkStationModuleLogic::SetSliceViewFromVolume( vtkSlicerApplication *app,vtkMRMLVolumeNode *volumeNode)
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
  this->ComputePermutationFromOrientation(matrix, permutation, flip);

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
vtkMRMLScalarVolumeNode *vtkPerkStationModuleLogic::AddVolumeToScene(vtkSlicerApplication* app, const char *fileName, VolumeType volumeType/*=VOL_GENERIC*/)
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
  case VOL_CALIBRATE_PLAN:
    this->PerkStationModuleNode->SetPlanningVolumeNode(volumeNode); 
    this->PerkStationModuleNode->SetPlanningVolumeRef(volumeNode->GetID());
    this->PerkStationModuleNode->AddVolumeInformationToList(volumeNode, fileName, "Planning");
    break;
 
  case VOL_VALIDATION:
    this->PerkStationModuleNode->SetValidationVolumeNode(volumeNode);   
    this->PerkStationModuleNode->SetValidationVolumeRef(volumeNode->GetID());
    this->PerkStationModuleNode->AddVolumeInformationToList(volumeNode, fileName, "Validation");
    break;
  case VOL_GENERIC:
    this->PerkStationModuleNode->AddVolumeInformationToList(volumeNode, fileName, "Other");
    break;
  default:
    vtkErrorMacro("AddVolumeToScene: unknown volume type: " << volumeType);
  }
  
  volumeNode->Modified();
  this->Modified();

  return volumeNode;
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkPerkStationModuleLogic::AddArchetypeVolume(vtkSlicerApplication* app, const char* fileName, const char *volumeName)
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


//-------------------------------------------------------------------------
// Compute the dimension order for the matrix, and look for flips.
void vtkPerkStationModuleLogic::ComputePermutationFromOrientation(
  vtkMatrix4x4 *matrix, int permutation[3], int flip[3])
{
  if (matrix == 0)
    {
    permutation[0] = 0;
    permutation[1] = 1;
    permutation[2] = 2;
    flip[0] = 0;
    flip[1] = 0;
    flip[2] = 0;

    return;
    }

  // There are 6 permutations for 3 dimensions.  In addition,
  // if each of those dimensions can be flipped, then there are
  // 8 (two to the power of three) possible flips.  That would
  // give 48 different possibilities, but since we don't consider
  // any combinations that result in left-handed rotations, the
  // total number of combinations that we test is 24.

  // Convert the matrix into three column vectors
  double vectors[3][4];
  int i = 0;
  int j = 0;
  for (i = 0; i < 3; i++)
    {
    double *v = vectors[i];
    for (j = 0; j < 4; j++)
      {
      v[j] = 0.0;
      }
    v[i] = 1.0;
    matrix->MultiplyPoint(v, v);
    }

  // Here's how the algorithm works.  We want to find a matrix
  // composed only of permutations and flips that has the closest
  // possible orientation (in terms of absolute orientation angle)
  // to our DirectionCosines.

  // The orientation angle for any matrix A is given by:
  //
  //   cos(angle/2) = sqrt(1 + trace(A))/2
  //
  // Therefore, the minimum angle occurs when the trace is
  // at its maximum.

  // So our method is to calculate the traces of all the various
  // permutations and flips, and just use the one with the largest
  // trace.

  // First check if the matrix includes an odd number of flips,
  // since if it does, it specifies a left-handed rotation.
  double d = vtkMath::Determinant3x3(vectors[0], vectors[1], vectors[2]);
  int oddPermutation = (d < 0);

  // Calculate all the traces, including any combination of
  // permutations and flips that represent right-handed
  // orientations.
  int imax = 0;
  int jmax = 0;
  int kmax = 0;
  int lmax = 0;
  double maxtrace = -1e30;

  for (i = 0; i < 3; i++)
    {
    for (j = 0; j < 2; j++)
      {
      double xval = vectors[i][0];
      double yval = vectors[(i + 1 + j) % 3][1];
      double zval = vectors[(i + 2 - j) % 3][2];
      for (int k = 0; k < 2; k++)
        {
        for (int l = 0; l < 2; l++)
          {
          // The (1 - 2*k) gives a sign from a boolean.
          // For z, we want to set the sign that will
          // not change the handedness ("^" is XOR).
          double xtmp = xval * (1 - 2*k);
          double ytmp = yval * (1 - 2*l);
          double ztmp = zval * (1 - 2*(j ^ k ^ l ^ oddPermutation));

          double trace = xtmp + ytmp + ztmp;

          // Find maximum trace
          if (trace > maxtrace)
            {
            maxtrace = trace;
            imax = i;
            jmax = j;
            kmax = k;
            lmax = l;
            }
          }
        }
      }
    }

  // Find the permutation to map each column of the orientation
  // matrix to a spatial dimension x, y, or z.
  int xidx = imax;
  int yidx = (imax + 1 + jmax) % 3;
  int zidx = (imax + 2 - jmax) % 3;

  permutation[0] = xidx;
  permutation[1] = yidx;
  permutation[2] = zidx;

  flip[xidx] = kmax;
  flip[yidx] = lmax;
  flip[zidx] = (jmax ^ kmax ^ lmax ^ oddPermutation);
}

//----------------------------------------------------------------------------
void vtkPerkStationModuleLogic::SetAutoScaleScalarVolume(vtkMRMLScalarVolumeNode *volumeNode)
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
  //int modifyOld=displayNode->StartModify(); 
  displayNode->DisableModifiedEventOn();
  displayNode->SetAutoWindowLevel(true);
  displayNode->SetAutoThreshold(false);
  displayNode->SetApplyThreshold(false);  
  volumeNode->CalculateScalarAutoLevels(NULL,NULL);
  displayNode->SetAutoWindowLevel(false);
  displayNode->SetAutoThreshold(false);
  displayNode->SetApplyThreshold(false);
  displayNode->DisableModifiedEventOff();
  displayNode->Modified();
  //displayNode->EndModify(modifyOld); 
}
