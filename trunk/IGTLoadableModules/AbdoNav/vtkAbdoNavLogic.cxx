/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

/* AbdoNav includes */
#include "vtkAbdoNavLogic.h"

/* IGT includes */
#include "vtkIGTPat2ImgRegistration.h"

/* MRML includes */
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLLinearTransformNode.h"

/* STL includes */
#include <limits>
#include <string>

/* VTK includes */
#include "vtkAppendPolyData.h"
#include "vtkCellArray.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkMath.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkRegularPolygonSource.h"
#include "vtkRenderer.h"
#include "vtkRotationalExtrusionFilter.h"
#include "vtkSmartPointer.h"
#include "vtkTransformPolyDataFilter.h"

//----------------------------------------------------------------
// Marker geometry definitions relative to a tool's local
// coordinate system (see Polaris Vicra Tool Kit Guide).
//----------------------------------------------------------------
const float markerA_8700338[] = {0.00,   0.00,   0.00};
const float markerB_8700338[] = {0.00,  25.28,  16.15};
const float markerC_8700338[] = {0.00,   0.00,  65.00};
const float markerD_8700338[] = {0.00, -18.65,  29.61};
const float markerA_8700339[] = {0.00,   0.00,   0.00};
const float markerB_8700339[] = {0.00,  28.59,  41.02};
const float markerC_8700339[] = {0.00,   0.00,  88.00};
const float markerD_8700339[] = {0.00, -44.32,  40.45};
const float markerA_8700340[] = {0.00,   0.00,   0.00};
const float markerB_8700340[] = {0.00,   0.00,  50.00};
const float markerC_8700340[] = {0.00,  25.00, 100.00};
const float markerD_8700340[] = {0.00, -25.00, 135.00};

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAbdoNavLogic, "$Revision: $");
vtkStandardNewMacro(vtkAbdoNavLogic);


//---------------------------------------------------------------------------
vtkAbdoNavLogic::vtkAbdoNavLogic()
{
  //----------------------------------------------------------------
  // Initialize logic values.
  //----------------------------------------------------------------
  this->AbdoNavNode = NULL;
  this->Pat2ImgReg = vtkIGTPat2ImgRegistration::New();
  this->RegistrationTransform = NULL;
  this->RelativeTrackingTransform = NULL;
  this->LocatorFreezePosition = NULL;
  this->Crosshair = NULL;
  this->AppGUI = NULL;
  this->Actor2DGreen = NULL;
  this->Actor2DRed = NULL;
  for (int i = 0; i < 3; i++)
    {
    this->SliceNode[i] = NULL;
    }
  // initialize each slice node's (Red == 0, Yellow == 1, Green == 2) slice driver to "User"
  for (int i = 0; i < 3; i++)
    {
    this->SliceDriver[i] = "User";
    }
  // initialize each slice node's (Red == 0, Yellow == 1, Green == 2) orientation with Slicer's
  // default setting (Red == Axial, Yellow == Sagittal, Green == Coronal)
  this->SliceOrientation[0] = "Axial";
  this->SliceOrientation[1] = "Sagittal";
  this->SliceOrientation[2] = "Coronal";
  this->RegistrationPerformed = 0;
  this->RecordLocatorPosition = 0;
  this->ShowCrosshair = 0;
  this->DrawNeedleProjection = 0;
  this->FreezeReslicing = 0;
  this->ObliqueReslicing = 0;
}


//---------------------------------------------------------------------------
vtkAbdoNavLogic::~vtkAbdoNavLogic()
{
  //----------------------------------------------------------------
  // Clean up logic values.
  //
  // Must *NOT* delete the following:
  //  - AbdoNavNode (handled by Slicer)
  //  - RegistrationTransform (handled by Slicer)
  //  - SliceNode[3] (handled by Slicer)
  //
  // Must delete the following (even though not created by AbdoNav):
  //  - RelativeTrackingTransform (will cause leaks when Slicer gets
  //    closed after loading a previously saved scene)
  //  - Crosshair (will cause leaks when Slicer gets closed)
  //----------------------------------------------------------------
  // remove AbdoNavNode observers
  vtkSetMRMLNodeMacro(this->AbdoNavNode, NULL);

  if (this->Pat2ImgReg)
    {
    this->Pat2ImgReg->Delete();
    }
  if (this->RelativeTrackingTransform)
    {
    this->RelativeTrackingTransform->Delete();
    }
  if (this->LocatorFreezePosition)
    {
    this->LocatorFreezePosition->Delete();
    }
  if (this->Crosshair)
    {
    this->Crosshair->Delete();
    }
  if (this->AppGUI)
    {
    this->AppGUI->Delete();
    }
  if (this->Actor2DGreen)
    {
    this->Actor2DGreen->Delete();
    }
  if (this->Actor2DRed)
    {
    this->Actor2DRed->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkAbdoNavLogic: " << this->GetClassName() << "\n";
  os << indent << "vtkAbdoNavNode: " << this->AbdoNavNode << "\n";
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData)
{
  vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);
  if (node != NULL && this->AbdoNavNode != NULL &&
      this->AbdoNavNode->GetTrackingTransformID() != NULL &&
      strcmp(node->GetID(), this->AbdoNavNode->GetTrackingTransformID()) == 0 &&
      event == vtkMRMLTransformableNode::TransformModifiedEvent)
    {

    if (this->RegistrationPerformed == true)
      {
      this->UpdateAll();
      }

    }
}


//---------------------------------------------------------------------------
int vtkAbdoNavLogic::PerformRegistration()
{
  vtkMRMLFiducialListNode* fnode;
  float* tipOffset = NULL;

  if (this->AbdoNavNode == NULL)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::PerformRegistration(...): "
                  "Couldn't retrieve AbdoNavNode!");
    return EXIT_FAILURE;
    }
  else if (this->AbdoNavNode->GetGuidanceToolType() == NULL || (this->AbdoNavNode->GetGuidanceToolType() != NULL &&
           !strcmp(this->AbdoNavNode->GetGuidanceToolType(), "")))
    {
    vtkErrorMacro("in vtkAbdoNavLogic::PerformRegistration(...): "
                  "Guidance tool type not specified!");
    return EXIT_FAILURE;
    }
  else
    {
    fnode = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetRegistrationFiducialListID()));
    tipOffset = this->AbdoNavNode->GetGuidanceTipOffset();

    if (fnode == NULL)
      {
      vtkErrorMacro("in vtkAbdoNavLogic::PerformRegistration(...): "
                    "Couldn't retrieve registration fiducial list!");
      return EXIT_FAILURE;
      }
    else if (fnode->GetNumberOfFiducials() < 3)
      {
      vtkErrorMacro("in vtkAbdoNavLogic::PerformRegistration(...): "
                    "Need to identify at least three fiducials in image space!");
      return EXIT_FAILURE;
      }
    else if (isnanf(tipOffset[0]))
      {
      vtkErrorMacro("in vtkAbdoNavLogic::PerformRegistration(...): "
                    "Couldn't retrieve offset of the guidance needle!");
      return EXIT_FAILURE;
      }
    }

  // initialize marker geometry definition depending on
  // which guidance needle tool type is being used
  float tool[4][3];
  if (!strcmp(this->AbdoNavNode->GetGuidanceToolType(), "8700338"))
    {
    tool[0][0] = markerA_8700338[0];
    tool[0][1] = markerA_8700338[1];
    tool[0][2] = markerA_8700338[2];
    tool[1][0] = markerB_8700338[0];
    tool[1][1] = markerB_8700338[1];
    tool[1][2] = markerB_8700338[2];
    tool[2][0] = markerC_8700338[0];
    tool[2][1] = markerC_8700338[1];
    tool[2][2] = markerC_8700338[2];
    tool[3][0] = markerD_8700338[0];
    tool[3][1] = markerD_8700338[1];
    tool[3][2] = markerD_8700338[2];
    }
  else if (!strcmp(this->AbdoNavNode->GetGuidanceToolType(), "8700339"))
    {
    tool[0][0] = markerA_8700339[0];
    tool[0][1] = markerA_8700339[1];
    tool[0][2] = markerA_8700339[2];
    tool[1][0] = markerB_8700339[0];
    tool[1][1] = markerB_8700339[1];
    tool[1][2] = markerB_8700339[2];
    tool[2][0] = markerC_8700339[0];
    tool[2][1] = markerC_8700339[1];
    tool[2][2] = markerC_8700339[2];
    tool[3][0] = markerD_8700339[0];
    tool[3][1] = markerD_8700339[1];
    tool[3][2] = markerD_8700339[2];
    }
  else if (!strcmp(this->AbdoNavNode->GetGuidanceToolType(), "8700340"))
    {
    tool[0][0] = markerA_8700340[0];
    tool[0][1] = markerA_8700340[1];
    tool[0][2] = markerA_8700340[2];
    tool[1][0] = markerB_8700340[0];
    tool[1][1] = markerB_8700340[1];
    tool[1][2] = markerB_8700340[2];
    tool[2][0] = markerC_8700340[0];
    tool[2][1] = markerC_8700340[1];
    tool[2][2] = markerC_8700340[2];
    tool[3][0] = markerD_8700340[0];
    tool[3][1] = markerD_8700340[1];
    tool[3][2] = markerD_8700340[2];
    }

  // Temporarily store source and target landmarks in order
  // to calculate the FRE (target == image cosy, source ==
  // guidance cosy).
  // TODO: It would be better to implement Getters in class
  //       vtkIGTPat2ImgRegistration and even better if the
  //       FRE were calculated in there since it is just a
  //       wrapper for class vtkLandmarkTransform anyway.
  //       Another option might be to use vtkLandmarkTransform
  //       directly.
  vtkPoints* targetLandmarks = vtkPoints::New();
  targetLandmarks->SetDataTypeToFloat();
  targetLandmarks->SetNumberOfPoints(fnode->GetNumberOfFiducials());
  vtkPoints* sourceLandmarks = vtkPoints::New();
  sourceLandmarks->SetDataTypeToFloat();
  sourceLandmarks->SetNumberOfPoints(fnode->GetNumberOfFiducials());

  // initialize least-squares solver
  this->Pat2ImgReg->SetNumberOfPoints(fnode->GetNumberOfFiducials());

  // pass point-pairs to least-squares solver
  float* tmp = NULL;
  std::cout.setf(ios::scientific, ios::floatfield);
  std::cout.precision(8);
  std::cout << "===========================================================================" << std::endl;
  std::cout << "Registration input parameters:" << std::endl;
  for (int i = 0; i < fnode->GetNumberOfFiducials(); i++)
    {
    if (!strcmp(tip, fnode->GetNthFiducialLabelText(i)))
      {
      tmp = fnode->GetNthFiducialXYZ(i);
      this->Pat2ImgReg->AddPoint(i, tmp[0], tmp[1], tmp[2], tipOffset[0], tipOffset[1], tipOffset[2]);
      targetLandmarks->InsertPoint(i, tmp[0], tmp[1], tmp[2]);
      sourceLandmarks->InsertPoint(i, tipOffset[0], tipOffset[1], tipOffset[2]);
      std::cout << tip << ",," << tmp[0]       << ",," << tmp[1]       << ",," << tmp[2]       << ",,[RAS]" << std::endl;
      std::cout << tip << ",," << tipOffset[0] << ",," << tipOffset[1] << ",," << tipOffset[2] << ",,[XYZ]" << std::endl;
      }
    else if (!strcmp(markerA, fnode->GetNthFiducialLabelText(i)))
      {
      tmp = fnode->GetNthFiducialXYZ(i);
      this->Pat2ImgReg->AddPoint(i, tmp[0], tmp[1], tmp[2], tool[0][0], tool[0][1], tool[0][2]);
      targetLandmarks->InsertPoint(i, tmp[0], tmp[1], tmp[2]);
      sourceLandmarks->InsertPoint(i, tool[0][0], tool[0][1], tool[0][2]);
      std::cout << markerA << ",," << tmp[0]       << ",," << tmp[1]       << ",," << tmp[2]       << ",,[RAS]" << std::endl;
      std::cout << markerA << ",," << tool[0][0]   << ",," << tool[0][1]   << ",," << tool[0][2]   << ",,[XYZ]" << std::endl;
      }
    else if (!strcmp(markerB, fnode->GetNthFiducialLabelText(i)))
      {
      tmp = fnode->GetNthFiducialXYZ(i);
      this->Pat2ImgReg->AddPoint(i, tmp[0], tmp[1], tmp[2], tool[1][0], tool[1][1], tool[1][2]);
      targetLandmarks->InsertPoint(i, tmp[0], tmp[1], tmp[2]);
      sourceLandmarks->InsertPoint(i, tool[1][0], tool[1][1], tool[1][2]);
      std::cout << markerB << ",," << tmp[0]       << ",," << tmp[1]       << ",," << tmp[2]       << ",,[RAS]" << std::endl;
      std::cout << markerB << ",," << tool[1][0]   << ",," << tool[1][1]   << ",," << tool[1][2]   << ",,[XYZ]" << std::endl;
      }
    else if (!strcmp(markerC, fnode->GetNthFiducialLabelText(i)))
      {
      tmp = fnode->GetNthFiducialXYZ(i);
      this->Pat2ImgReg->AddPoint(i, tmp[0], tmp[1], tmp[2], tool[2][0], tool[2][1], tool[2][2]);
      targetLandmarks->InsertPoint(i, tmp[0], tmp[1], tmp[2]);
      sourceLandmarks->InsertPoint(i, tool[2][0], tool[2][1], tool[2][2]);
      std::cout << markerC << ",," << tmp[0]       << ",," << tmp[1]       << ",," << tmp[2]       << ",,[RAS]" << std::endl;
      std::cout << markerC << ",," << tool[2][0]   << ",," << tool[2][1]   << ",," << tool[2][2]   << ",,[XYZ]" << std::endl;
      }
    else if (!strcmp(markerD, fnode->GetNthFiducialLabelText(i)))
      {
      tmp = fnode->GetNthFiducialXYZ(i);
      this->Pat2ImgReg->AddPoint(i, tmp[0], tmp[1], tmp[2], tool[3][0], tool[3][1], tool[3][2]);
      targetLandmarks->InsertPoint(i, tmp[0], tmp[1], tmp[2]);
      sourceLandmarks->InsertPoint(i, tool[3][0], tool[3][1], tool[3][2]);
      std::cout << markerD << ",," << tmp[0]       << ",," << tmp[1]       << ",," << tmp[2]       << ",,[RAS]" << std::endl;
      std::cout << markerD << ",," << tool[3][0]   << ",," << tool[3][1]   << ",," << tool[3][2]   << ",,[XYZ]" << std::endl;
      }
    }

  // calculate registration matrix
  int error = this->Pat2ImgReg->DoRegistration();
  if (error)
    {
    return EXIT_FAILURE;
    }

  // get registration matrix
  vtkMatrix4x4* registrationMatrix = vtkMatrix4x4::New();
  registrationMatrix->DeepCopy(this->Pat2ImgReg->GetLandmarkTransformMatrix());

  //----------------------------------------------------------------
  // Calculate FRE.
  //
  // NOTE: FRE calculation *MUST* be performed before replacing the
  //       translatory component of the registration matrix by the
  //       guidance needle's tip offset!
  //----------------------------------------------------------------
  // target == image cosy
  double target[3];
  // source == guidance cosy
  double source3[3];
  double source4[4];
  double registeredSource4[4];

  double sum = 0.0;

  for (int r = 0; r < fnode->GetNumberOfFiducials(); r++)
    {
    targetLandmarks->GetPoint(r, target);
    sourceLandmarks->GetPoint(r, source3);
    source4[0] = source3[0];
    source4[1] = source3[1];
    source4[2] = source3[2];
    source4[3] = 1;
    registrationMatrix->MultiplyPoint(source4, registeredSource4);

    sum = sum + sqrt(( registeredSource4[0] - target[0] ) * ( registeredSource4[0] - target[0] )
                   + ( registeredSource4[1] - target[1] ) * ( registeredSource4[1] - target[1] )
                   + ( registeredSource4[2] - target[2] ) * ( registeredSource4[2] - target[2] ));
    }
  sum = sum / fnode->GetNumberOfFiducials();
  this->AbdoNavNode->SetFRE(sum);
  std::cout << "===========================================================================" << std::endl;
  std::cout << "FRE,," << this->AbdoNavNode->GetFRE() << std::endl;
  std::cout << "===========================================================================" << std::endl;
  std::cout << "Registration matrix *BEFORE* replacement:" << std::endl;
  std::cout << registrationMatrix->GetElement(0, 0) << ",," << registrationMatrix->GetElement(0, 1) << ",," << registrationMatrix->GetElement(0, 2) << ",," << registrationMatrix->GetElement(0, 3) << std::endl;
  std::cout << registrationMatrix->GetElement(1, 0) << ",," << registrationMatrix->GetElement(1, 1) << ",," << registrationMatrix->GetElement(1, 2) << ",," << registrationMatrix->GetElement(1, 3) << std::endl;
  std::cout << registrationMatrix->GetElement(2, 0) << ",," << registrationMatrix->GetElement(2, 1) << ",," << registrationMatrix->GetElement(2, 2) << ",," << registrationMatrix->GetElement(2, 3) << std::endl;
  std::cout << registrationMatrix->GetElement(3, 0) << ",," << registrationMatrix->GetElement(3, 1) << ",," << registrationMatrix->GetElement(3, 2) << ",," << registrationMatrix->GetElement(3, 3) << std::endl;

  // The calculated registration matrix describes the relationship between
  // the image coordinate system and the guidance needle's local coordinate
  // system at markerA. By performing the pivoting procedure, however, the
  // guidance needle's local coordinate system is translated from markerA
  // to the guidance needle's tip. Thus, the translatory component of the
  // registration matrix needs to be replaced by the guidance needle's tip
  // offset.

  // get guidance needle's tip offset
  for (int i = 0; i < fnode->GetNumberOfFiducials(); i++)
    {
    if (!strcmp(tip, fnode->GetNthFiducialLabelText(i)))
      {
      tmp = fnode->GetNthFiducialXYZ(i);
      }
    }
  // replace translatory component
  registrationMatrix->SetElement(0, 3, tmp[0]);
  registrationMatrix->SetElement(1, 3, tmp[1]);
  registrationMatrix->SetElement(2, 3, tmp[2]);
  std::cout << "===========================================================================" << std::endl;
  std::cout << "Registration matrix *AFTER* replacement:" << std::endl;
  std::cout << registrationMatrix->GetElement(0, 0) << ",," << registrationMatrix->GetElement(0, 1) << ",," << registrationMatrix->GetElement(0, 2) << ",," << registrationMatrix->GetElement(0, 3) << std::endl;
  std::cout << registrationMatrix->GetElement(1, 0) << ",," << registrationMatrix->GetElement(1, 1) << ",," << registrationMatrix->GetElement(1, 2) << ",," << registrationMatrix->GetElement(1, 3) << std::endl;
  std::cout << registrationMatrix->GetElement(2, 0) << ",," << registrationMatrix->GetElement(2, 1) << ",," << registrationMatrix->GetElement(2, 2) << ",," << registrationMatrix->GetElement(2, 3) << std::endl;
  std::cout << registrationMatrix->GetElement(3, 0) << ",," << registrationMatrix->GetElement(3, 1) << ",," << registrationMatrix->GetElement(3, 2) << ",," << registrationMatrix->GetElement(3, 3) << std::endl;
  std::cout.unsetf(ios::floatfield);
  std::cout.precision(6);

  // create/retrieve registration transform node
  if (this->AbdoNavNode->GetRegistrationTransformID() == NULL)
    {
    // create a new registration transform node and add it to the scene
    this->RegistrationTransform = vtkMRMLLinearTransformNode::New();
    this->RegistrationTransform->SetName("AbdoNav-RegistrationTransform");
    this->RegistrationTransform->SetDescription("Created by AbdoNav");
    this->GetMRMLScene()->AddNode(this->RegistrationTransform);
    this->RegistrationTransform->Delete();
    // set registration transform node ID in AbdoNavNode
    this->AbdoNavNode->SetRegistrationTransformID(this->RegistrationTransform->GetID());
    // get relative tracking transform node and make it observe the registration transform node
    if (this->AbdoNavNode->GetTrackingTransformID() != NULL)
      {
      vtkMRMLLinearTransformNode* relativeTrackingTransform = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackingTransformID()));
      relativeTrackingTransform->SetAndObserveTransformNodeID(this->RegistrationTransform->GetID());
      }
    else
      {
      vtkErrorMacro("in vtkAbdoNavLogic::PerformRegistration(): "
                    "Couldn't move relative tracking transform node below registration transform node "
                    "because vtkMRMLAbdoNavNode::GetTrackingTransformID() returned NULL!");
      }
    }
  else
    {
    this->RegistrationTransform = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetRegistrationTransformID()));
    }

  // copy registration matrix into registration transform node
  this->RegistrationTransform->GetMatrixTransformToParent()->DeepCopy(registrationMatrix);
  // indicate that the Scene contains unsaved changes; make Slicer's save dialog list this transform as modified
  this->RegistrationTransform->SetModifiedSinceRead(1);
  // indicate that registration has been performed
  this->RegistrationPerformed = 1;

  // clean up
  registrationMatrix->Delete();
  targetLandmarks->Delete();
  sourceLandmarks->Delete();

  return EXIT_SUCCESS;
}


//---------------------------------------------------------------------------
int vtkAbdoNavLogic::ParseToolBoxProperties()
{
  if (this->AbdoNavNode == NULL)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::ParseToolBoxProperties(...): "
                  "Couldn't retrieve AbdoNavNode!");
    return EXIT_FAILURE;
    }
  else if (this->AbdoNavNode->GetGuidanceToolType() == NULL || (this->AbdoNavNode->GetGuidanceToolType() != NULL &&
           !strcmp(this->AbdoNavNode->GetGuidanceToolType(), "")))
    {
    vtkErrorMacro("in vtkAbdoNavLogic::ParseToolBoxProperties(...): "
                  "Guidance tool type not specified!");
    return EXIT_FAILURE;
    }
  else if (this->AbdoNavNode->GetToolBoxPropertiesFile() == NULL)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::ParseToolBoxProperties(...): "
                  "Path to NDI ToolBox \".trackProperties\" file not specified!");
    return EXIT_FAILURE;
    }

  // start position of the x-, y- and z-offset
  int spos = 33;
  // variable to temporarily store each line of the file
  std::string line;
  // array to temporarily store the x-, y- and z-offset
  float offset[3];
  offset[0] = offset[1] = offset[2] = std::numeric_limits<float>::quiet_NaN();

  // get search pattern
  std::string pattern;
  if (!strcmp(this->AbdoNavNode->GetGuidanceToolType(), "8700338"))
    {
    pattern = tool_8700338;
    }
  else if (!strcmp(this->AbdoNavNode->GetGuidanceToolType(), "8700339"))
    {
    pattern = tool_8700339;
    }
  else if (!strcmp(this->AbdoNavNode->GetGuidanceToolType(), "8700340"))
    {
    pattern = tool_8700340;
    }

  // open NDI ToolBox ".trackProperties" file
  ifstream infile;
  infile.open(this->AbdoNavNode->GetToolBoxPropertiesFile());

  // if opening file successful & while not at the end of the file
  while (infile.good())
    {
    // read each line
    getline(infile, line);

    if (line.find(pattern + "x=") != std::string::npos)
      {
      // extract substring containing the x-offset
      // (due to passing std::string::npos as second argument, the resulting
      // substring contains all characters between the start position (spos)
      // and the end of the string)
      //std::cout << line.substr(spos, std::string::npos) << std::endl;
      // convert substring to double and save it
      offset[0] = atof(line.substr(spos, std::string::npos).c_str());
      // print converted value
      //std::cout << offset[0] << std::endl;
      }
    else if (line.find(pattern + "y=") != std::string::npos)
      {
      // extract substring containing the y-offset
      // (due to passing std::string::npos as second argument, the resulting
      // substring contains all characters between the start position (spos)
      // and the end of the string)
      //std::cout << line.substr(spos, std::string::npos) << std::endl;
      // convert substring to double and save it
      offset[1] = atof(line.substr(spos, std::string::npos).c_str());
      // print converted value
      //std::cout << offset[1] << std::endl;
      }
    else if (line.find(pattern + "z=") != std::string::npos)
      {
      // extract substring containing the z-offset
      // (due to passing std::string::npos as second argument, the resulting
      // substring contains all characters between the start position (spos)
      // and the end of the string)
      //std::cout << line.substr(spos, std::string::npos) << std::endl;
      // convert substring to double and save it
      offset[2] = atof(line.substr(spos, std::string::npos).c_str());
      // print converted value
      //std::cout << offset[2] << std::endl;
      }
    }

  // close NDI ToolBox ".trackProperties" file
  infile.close();

  // verify whether or not parsing was successful
  if (isnanf(offset[0]) || isnanf(offset[1]) || isnanf(offset[2]))
    {
    return EXIT_FAILURE;
    }

  // save converted values to vtkMRMLAbdoNavNode
  this->AbdoNavNode->SetGuidanceTipOffset(offset[0], offset[1], offset[2]);

  std::cout.setf(ios::scientific, ios::floatfield);
  std::cout.precision(8);
  std::cout << "===========================================================================" << std::endl;
  std::cout << "toolType,," << this->AbdoNavNode->GetGuidanceToolType() << std::endl;
  std::cout << "path,," << this->AbdoNavNode->GetToolBoxPropertiesFile() << std::endl;
  std::cout << "tipOffset,," << offset[0]   << ",," << offset[1]   << ",," << offset[2]   << ",,[XYZ]" << std::endl;
  std::cout.unsetf(ios::floatfield);
  std::cout.precision(6);

  return EXIT_SUCCESS;
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateAll()
{
  if (this->FreezeReslicing == false)
    {
    // get registered tracking data for reslicing
    vtkMatrix4x4* registeredTracker = vtkMatrix4x4::New();
    vtkMRMLLinearTransformNode* unregisteredTracker = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackingTransformID()));
    if (unregisteredTracker != NULL)
      {
      unregisteredTracker->GetMatrixTransformToWorld(registeredTracker);
      }
    else
      {
      return;
      }

    if (this->RecordLocatorPosition == true)
      {
      // number of tracking data samples to be averaged
      const static int MAX_SAMPLES = 5;
      // array to store a number of (MAX_SAMPLES) 3 dimensional coordinates
      static double samples[MAX_SAMPLES][3];
      // index of current tracking data sample
      static int currentSample = 0;

      if (currentSample < MAX_SAMPLES)
        {
        if (currentSample == 0)
          {
          //std::cout << "===========================================================================" << std::endl;
          }
        // store and print current tracking data sample
        samples[currentSample][0] = registeredTracker->Element[0][3];;
        samples[currentSample][1] = registeredTracker->Element[1][3];
        samples[currentSample][2] = registeredTracker->Element[2][3];
        //std::cout << "sample " << currentSample << " = " << samples[currentSample][0] << ",," << samples[currentSample][1] << ",," << samples[currentSample][2] << " [R,,A,,S]" << std::endl;
        currentSample++;
        }
      else if (currentSample == MAX_SAMPLES)
        {
        // average tracking data samples
        double avg[3];
        double sum;
        for (int i = 0; i < 3; i++)
          {
          sum = 0;
          for (int j = 0; j < MAX_SAMPLES; j++)
            {
            sum = sum + samples[j][i];
            }
          avg[i] = sum / MAX_SAMPLES;
          }

        // store & print averaged position, indicate to GUI that recording locator position is done
        this->RecordLocatorPosition = false;
        currentSample = 0;
        vtkMatrix4x4* avgM = vtkMatrix4x4::New();
        avgM->Identity();
        avgM->Element[0][3] = avg[0];
        avgM->Element[1][3] = avg[1];
        avgM->Element[2][3] = avg[2];
        vtkCollection* recPos = this->AbdoNavNode->GetRecordedPositions();
        recPos->AddItem(avgM);
        avgM->Delete();
        std::cout.setf(ios::scientific, ios::floatfield);
        std::cout.precision(8);
        std::cout << "RecordedPosition,," << (recPos->GetNumberOfItems() - 1) << ",," << avg[0] << ",," << avg[1] << ",," << avg[2] << ",,[RAS]" << std::endl;
        std::cout.unsetf(ios::floatfield);
        std::cout.precision(6);
        this->InvokeEvent(LocatorPositionRecorded);
        }
      }

    this->CheckSliceNodes();

    // update each slice node that is driven by the locator
    for (int i = 0; i < 3; i++)
      {
      // driven by locator
      if (this->SliceDriver[i] == "Locator")
        {
        if (registeredTracker != NULL)
          {
          this->ResliceNode(i, registeredTracker);
          }
        }
      }

    // update the crosshair
    if (this->Crosshair != NULL && this->ShowCrosshair == 1)
      {
      if (registeredTracker != NULL)
        {
        float px = registeredTracker->Element[0][3];
        float py = registeredTracker->Element[1][3];
        float pz = registeredTracker->Element[2][3];

        this->Crosshair->SetCrosshairMode(vtkMRMLCrosshairNode::ShowAll);
        this->Crosshair->SetCrosshairRAS(px, py, pz);
        }
      }
    else if (this->Crosshair != NULL && this->ShowCrosshair == 0)
      {
      this->Crosshair->SetCrosshairMode(vtkMRMLCrosshairNode::NoCrosshair);
      }

    // update the needle projection
    //
    // the check for this->AppGUI != NULL is necessary because
    // the event triggering this->UpdateAll() (and thereby also
    // triggering this->UpdateNeedleProjection(...) which reads
    // this->AppGUI) might occur before this->AppGUI is set via
    // vtkAbdoNavGUI
    if (this->AppGUI != NULL && this->DrawNeedleProjection == 1)
      {
      this->UpdateNeedleProjection(registeredTracker);
      }
    else
      {
      // don't show needle projection
      if (this->Actor2DGreen != NULL)
        {
        this->Actor2DGreen->SetVisibility(0);
        }
      if (this->Actor2DRed != NULL)
        {
        this->Actor2DRed->SetVisibility(0);
        }
      }

    // clean up
    registeredTracker->Delete();
    }
}


//---------------------------------------------------------------------------
int vtkAbdoNavLogic::ObserveTrackingTransformNode()
{
  if (this->AbdoNavNode == NULL)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::ObserveTrackingTransformNode(): "
                  "Couldn't retrieve AbdoNavNode!");
    return EXIT_FAILURE;
    }

  vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackingTransformID()));
  if (tnode)
    {
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
    vtkSetAndObserveMRMLNodeEventsMacro(this->RelativeTrackingTransform, tnode, nodeEvents);
    nodeEvents->Delete();
    tnode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    }
  else
    {
    vtkErrorMacro("in vtkAbdoNavLogic::ObserveTrackingTransformNode(): "
                  "Couldn't retrieve tracking transform node!");
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ToggleLocatorVisibility(int vis)
{
  const char* locatorName = "AbdoNav-Locator";
  vtkMRMLModelNode* locatorModel;
  vtkMRMLDisplayNode* locatorDisplay;

  // get locator if it exists
  locatorModel = this->FindLocator(locatorName);

  if (locatorModel != NULL)
    {
    // locator already exists, thus toggle visibility
    locatorDisplay = locatorModel->GetDisplayNode();
    locatorDisplay->SetVisibility(vis);
    locatorModel->Modified();
    this->GetMRMLScene()->Modified();
    }
  else if (locatorModel == NULL && vis == 1)
    {
    // locator doesn't exist yet but visibility is set to one, thus create it
    this->EnableLocatorDriver(locatorName);
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ToggleLocatorFreeze(int freeze)
{
  // TODO: all TransformModifiedEvents within the scope of this function seem to be unnecessary --> remove?

  const char* locatorName = "AbdoNav-Locator";
  vtkMRMLModelNode* locatorModel;

  // get locator if it exists
  locatorModel = this->FindLocator(locatorName);

  if (locatorModel != NULL && freeze == 1)
    {
    // freeze locator model
    locatorModel->SetAndObserveTransformNodeID(NULL);
    // need to retrieve, store and apply current transformation matrix:
    // otherwise, the locator model wouldn't be displayed at its current
    // position but would instead be reset to its initial position
    if (this->LocatorFreezePosition == NULL)
      {
      // LocatorFreezePosition is deleted in the destructor
      this->LocatorFreezePosition = vtkMatrix4x4::New();
      }
    // retrieve and store transform to world (transform to parent isn't sufficient since
    // the relative tracking transform node observes AbdoNav's registration transform node)
    vtkMatrix4x4* tfmToWorld = vtkMatrix4x4::New();
    vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackingTransformID()))->GetMatrixTransformToWorld(tfmToWorld);
    this->LocatorFreezePosition->DeepCopy(tfmToWorld);
    tfmToWorld->Delete();
    // apply current transformation matrix
    locatorModel->ApplyTransform(this->LocatorFreezePosition);
    locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    }
  else if (locatorModel != NULL && freeze == 0)
    {
    // need to apply the inverse transformation matrix so as to obtain the identity matrix
    vtkMatrix4x4::Invert(this->LocatorFreezePosition, this->LocatorFreezePosition);
    locatorModel->ApplyTransform(this->LocatorFreezePosition);
    // unfreeze locator model
    locatorModel->SetAndObserveTransformNodeID(vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackingTransformID()))->GetID());
    locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    }
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkAbdoNavLogic::FindLocator(const char* locatorName)
{
  vtkMRMLModelNode* locatorModel;

  // check if locator exists
  vtkCollection* collection = this->GetMRMLScene()->GetNodesByName(locatorName);
  if (collection != NULL && collection->GetNumberOfItems() == 0)
    {
    // locator doesn't exist yet
    locatorModel = NULL;
    }
  else if (collection != NULL && collection->GetNumberOfItems() != 0)
    {
    // locator already exists
    locatorModel = vtkMRMLModelNode::SafeDownCast(collection->GetItemAsObject(0));
    }
  // clean up
  collection->Delete();

  return locatorModel;
}


//---------------------------------------------------------------------------
int vtkAbdoNavLogic::EnableLocatorDriver(const char* locatorName)
{
  // TODO: all TransformModifiedEvents within the scope of this function seem to be unnecessary --> remove?

  if (this->AbdoNavNode == NULL)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::EnableLocatorDriver(...): "
                  "Couldn't retrieve AbdoNavNode!");
    return EXIT_FAILURE;
    }

  // create the locator model
  vtkMRMLModelNode* locatorModel = this->AddLocatorModel(locatorName, 0.0, 1.0, 1.0);
  // get the tracker transform node
  vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackingTransformID()));
  // make locator model observe the tracker transform node
  if (locatorModel && tnode)
    {
    locatorModel->SetAndObserveTransformNodeID(tnode->GetID());
    locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    }
  else
    {
    vtkErrorMacro("in vtkAbdoNavLogic::EnableLocatorDriver(...): "
                  "Enabling locator model failed!");
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkAbdoNavLogic::AddLocatorModel(const char* locatorName, double r, double g, double b)
{
  // TODO: all Update() calls within the scope of this function seem to be unnecessary --> remove?

  vtkSmartPointer<vtkAppendPolyData> locatorPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
  vtkSmartPointer<vtkMRMLModelNode> locatorModel = vtkSmartPointer<vtkMRMLModelNode>::New();
  vtkSmartPointer<vtkMRMLModelDisplayNode> locatorDisplay = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();

  // cylinder resolution
  int numberOfSides = 16;
  // measured handle diameter = 6 mm --> radius = 3 mm
  double radiusHandle = 3.0;
  // measured needle diameter = 1.5 mm --> radius = 0.75 mm
  // exact needle diameter = 17 gauge ~ 1.47 mm --> radius = 0.735 mm (according to vendor brochure (Galil Medical))
  double radiusNeedle = 0.75;

  //----------------------------------------------------------------
  // Create needle tip.
  //----------------------------------------------------------------
  // create needle tip from a cone
  vtkSmartPointer<vtkConeSource> tip = vtkSmartPointer<vtkConeSource>::New();
  tip->SetResolution(numberOfSides);
  tip->SetRadius(radiusNeedle);
  tip->SetHeight(3.0); // 3 mm
  tip->SetCenter(0.0, 0.0, -(3.0/2));
  tip->SetDirection(0.0, 0.0, 1.0);
  tip->Update();
  // add needle tip
  locatorPolyData->AddInput(tip->GetOutput());

  //----------------------------------------------------------------
  // Create needle shaft.
  //----------------------------------------------------------------
  // create needle shaft from a cylinder
  vtkSmartPointer<vtkCylinderSource> shaft = vtkSmartPointer<vtkCylinderSource>::New();
  shaft->SetResolution(numberOfSides);
  shaft->SetRadius(radiusNeedle);
  shaft->SetHeight(172.0);  // 172 mm (--> needle tip and shaft total 175 mm)
  shaft->SetCenter(0.0, 0.0, 0.0);
  shaft->Update();
  // move needle shaft to designated location
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterShaft = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transShaft = vtkSmartPointer<vtkTransform>::New();
  transShaft->RotateX(90.0);
  transShaft->Translate(0.0, -(3.0 + 172.0/2), 0.0);
  transShaft->Update();
  tfilterShaft->SetInput(shaft->GetOutput());
  tfilterShaft->SetTransform(transShaft);
  tfilterShaft->Update();
  // add needle shaft
  locatorPolyData->AddInput(tfilterShaft->GetOutput());

  //----------------------------------------------------------------
  // Create first part of the needle handle.
  //----------------------------------------------------------------
  // create first part of the needle handle from a cylinder
  vtkSmartPointer<vtkCylinderSource> fstHandle = vtkSmartPointer<vtkCylinderSource>::New();
  fstHandle->SetResolution(numberOfSides);
  fstHandle->SetRadius(radiusHandle);
  fstHandle->SetHeight(20.0);
  fstHandle->SetCenter(0.0, 0.0, 0.0);
  fstHandle->Update();
  // move first part of the needle handle to designated location
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterFstHandle = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transFstHandle = vtkSmartPointer<vtkTransform>::New();
  transFstHandle->RotateX(90.0);
  transFstHandle->Translate(0.0, -(3.0 + 172.0 + 20.0/2), 0.0);
  transFstHandle->Update();
  tfilterFstHandle->SetInput(fstHandle->GetOutput());
  tfilterFstHandle->SetTransform(transFstHandle);
  tfilterFstHandle->Update();
  // add first part of the needle handle
  locatorPolyData->AddInput(tfilterFstHandle->GetOutput());

  //----------------------------------------------------------------
  // Create right angle of the needle handle by extruding a circle.
  //----------------------------------------------------------------
  // create a circle from a regular polygon
  vtkSmartPointer<vtkRegularPolygonSource> angle = vtkSmartPointer<vtkRegularPolygonSource>::New();
  angle->SetNumberOfSides(numberOfSides);
  angle->SetRadius(radiusHandle);
  angle->SetCenter(10.0, 0.0, 0.0);
  angle->Update();
  // rotate circle so that the rotational extrusion filter yields the desired effect
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterAngle1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transAngle1 = vtkSmartPointer<vtkTransform>::New();
  transAngle1->RotateX(-90.0);
  transAngle1->Update();
  tfilterAngle1->SetInput(angle->GetOutput());
  tfilterAngle1->SetTransform(transAngle1);
  tfilterAngle1->Update();
  // extrude the circle: this yields a cylinder bended by 90 degrees
  vtkSmartPointer<vtkRotationalExtrusionFilter> efilterAngle = vtkSmartPointer<vtkRotationalExtrusionFilter>::New();
  efilterAngle->SetInput(tfilterAngle1->GetOutput());
  efilterAngle->SetAngle(90.0);
  efilterAngle->CappingOff();
  efilterAngle->Update();
  // move right angle of the needle handle to designated location
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterAngle2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transAngle2 = vtkSmartPointer<vtkTransform>::New();
  transAngle2->RotateX(-90.0);
  transAngle2->Translate(-10.0, +(3.0 + 172.0 + 20.0), 0.0);
  transAngle2->Update();
  tfilterAngle2->SetInput(efilterAngle->GetOutput());
  tfilterAngle2->SetTransform(transAngle2);
  tfilterAngle2->Update();
  // add right angle of the needle handle
  locatorPolyData->AddInput(tfilterAngle2->GetOutput());

  //----------------------------------------------------------------
  // Create second part of the needle handle.
  //----------------------------------------------------------------
  // create second part of the needle handle from a cylinder
  vtkSmartPointer<vtkCylinderSource> sndHandle = vtkSmartPointer<vtkCylinderSource>::New();
  sndHandle->SetResolution(numberOfSides);
  sndHandle->SetRadius(radiusHandle);
  sndHandle->SetHeight(50.0);
  sndHandle->SetCenter(0.0, 0.0, 0.0);
  sndHandle->Update();
  // move second part of the needle handle to designated location
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterSndHandle = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transSndHandle = vtkSmartPointer<vtkTransform>::New();
  transSndHandle->RotateZ(90.0);
  transSndHandle->Translate(0.0, +(10.0 + 50.0/2), -(3.0 + 172.0 + 20.0 + 10.0));
  transSndHandle->Update();
  tfilterSndHandle->SetInput(sndHandle->GetOutput());
  tfilterSndHandle->SetTransform(transSndHandle);
  tfilterSndHandle->Update();
  // add second part of the needle handle
  locatorPolyData->AddInput(tfilterSndHandle->GetOutput());

  //----------------------------------------------------------------
  // Compensate for different coordinate system definitions in Slicer3
  // and the NDI Polaris Vicra.
  //----------------------------------------------------------------
  locatorPolyData->Update();
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterVicra = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transVicra = vtkSmartPointer<vtkTransform>::New();
  transVicra->RotateX(180.0);
  transVicra->Update();
  tfilterVicra->SetInput(locatorPolyData->GetOutput());
  tfilterVicra->SetTransform(transVicra);
  tfilterVicra->Update();

  //----------------------------------------------------------------
  // Add locator model to the scene.
  //----------------------------------------------------------------
  // prepare model node
  locatorModel->SetAndObservePolyData(tfilterVicra->GetOutput());
  locatorModel->SetName(locatorName);
  locatorModel->SetHideFromEditors(0);
  // prepare model display node
  double color[3] = {r, g, b};
  locatorDisplay->SetPolyData(locatorModel->GetPolyData());
  locatorDisplay->SetColor(color);
  locatorDisplay->SetVisibility(1);
  // add both nodes to the scene
  this->GetMRMLScene()->SaveStateForUndo();
  locatorModel = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->AddNode(locatorModel));
  locatorDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->GetMRMLScene()->AddNode(locatorDisplay));
  if (locatorModel && locatorDisplay)
    {
    locatorModel->SetAndObserveDisplayNodeID(locatorDisplay->GetID());
    }
  else
    {
    vtkErrorMacro("in vtkAbdoNavLogic::AddLocator(...): "
                  "Adding locator model node and/or locator model display node failed!");
    }

  return locatorModel;
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateNeedleProjection(vtkMatrix4x4* registeredTracker)
{
  //----------------------------------------------------------------
  // Due to being a projection of the procedure needle's direction
  // vector, the line being drawn grows and shrinks accordingly. That
  // is, the line has maximum length when the needle is in-plane and
  // is invisible when the needle is perpendicular to the slice plane.
  // Since the physician has no means to determine whether or not the
  // line has maximum length (and thus, is in-plane), the projected
  // line consists of a green and a red part with the red part indica-
  // ting the off-plane angle. That is, the line is completely green
  // when the needle is in-plane and is almost completely red when the
  // needle is close to being perpendicular to the slice plane. Depen-
  // ding on whether the procedure needle is in front of the slice plane
  // or behind it, the line is either solid or dashed respectively.
  //----------------------------------------------------------------

  // calculate angle alpha between the procedure needle and the slice
  // plane according to the formula below, where u denotes the direction
  // vector of the needle and n denotes the normal vector of the slice
  // plane:
  //
  // alpha = 90deg - radtodeg( arccos( |u * n| / |u| * |n| ) )
  //       =         radtodeg( arcsin( |u * n| / |u| * |n| ) )
  //
  // last equation is true due to arccos( x ) = PI/2 - arcsin( x )

  // direction vector of the needle in RAS coordinates
  double needleDirec[3] = {registeredTracker->Element[0][2], registeredTracker->Element[1][2], registeredTracker->Element[2][2]};
  // normal vector of the slice plane in RAS coordinates
  double planeNormal[3] = {0, 0, 1};

  // calculate angle alpha
  double alpha;
  int angle;
  alpha = asin(fabs(vtkMath::Dot(needleDirec, planeNormal)) / (vtkMath::Norm(needleDirec) * vtkMath::Norm(planeNormal)));
  // convert alpha from radians to degrees and round the result
  alpha = vtkMath::DegreesFromRadians(alpha);
  angle = vtkMath::Round(alpha);

  // get matrix that converts XY coordinates to RAS
  vtkMatrix4x4* RASToXY = vtkMatrix4x4::New();
  RASToXY->DeepCopy(this->GetApplicationLogic()->GetSliceLogic("Red")->GetSliceNode()->GetXYToRAS());
  // invert it and convert registered tracking coordinates from RAS to XY
  RASToXY->Invert();
  vtkMatrix4x4* trackerXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(RASToXY, registeredTracker, trackerXY);

  vtkRenderer* renderer = this->AppGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderer();

  // defines the end of the red part of the projected line which at the same time
  // is the end of the projected line
  int eor = 150;
  // defines the end of the green part of the projected line which at the same time
  // is the start of the red part of the projected line
  double eog = eor - (angle * eor / 90);

  // define the three points: start, endOfGreen, endOfRed
  double      start[3] = {trackerXY->Element[0][3], trackerXY->Element[1][3], trackerXY->Element[2][3]};
  double endofGreen[3] = {trackerXY->Element[0][3] + eog * trackerXY->Element[0][2],
                          trackerXY->Element[1][3] + eog * trackerXY->Element[1][2],
                          trackerXY->Element[2][3] + eog * trackerXY->Element[2][2]};
  double   endOfRed[3] = {trackerXY->Element[0][3] + eor * trackerXY->Element[0][2],
                          trackerXY->Element[1][3] + eor * trackerXY->Element[1][2],
                          trackerXY->Element[2][3] + eor * trackerXY->Element[2][2]};

  //----------------------------------------------------------------
  // Define green part of the projected line.
  //----------------------------------------------------------------
  // start and end of the green part
  vtkPoints* pointsGreen = vtkPoints::New();
  pointsGreen->InsertNextPoint(start);
  pointsGreen->InsertNextPoint(endofGreen);

  // line representation
  vtkCellArray* cellsGreen = vtkCellArray::New();
  cellsGreen->InsertNextCell(2); // number of points to be added
  cellsGreen->InsertCellPoint(0);
  cellsGreen->InsertCellPoint(1);

  // use points and lines as polydata input
  vtkPolyData* polyGreen = vtkPolyData::New();
  polyGreen->SetPoints(pointsGreen);
  polyGreen->SetLines(cellsGreen);

  // map polydata to 2d
  // by default, the 3d data is transformed into 2d by ignoring the z-coordinate
  // of the 3d points in vtkPolyData; see the vtkPolyDataMapper2D documentation
  vtkPolyDataMapper2D* mapperGreen = vtkPolyDataMapper2D::New();
  mapperGreen->SetInput(polyGreen);

  if (this->Actor2DGreen == NULL)
    {
    // first time called --> create and define properties
    this->Actor2DGreen = vtkActor2D::New();
    this->Actor2DGreen->GetProperty()->SetColor(0, 1, 0); // green
    this->Actor2DGreen->GetProperty()->SetLineWidth(1);

    // add to renderer
    renderer->AddActor2D(this->Actor2DGreen);
    }

  //----------------------------------------------------------------
  // Define red part of the projected line.
  //----------------------------------------------------------------
  // start and end of the red part
  vtkPoints* pointsRed = vtkPoints::New();
  pointsRed->InsertNextPoint(endofGreen);
  pointsRed->InsertNextPoint(endOfRed);

  // line representation
  vtkCellArray* cellsRed = vtkCellArray::New();
  cellsRed->InsertNextCell(2); // number of points to be added
  cellsRed->InsertCellPoint(0);
  cellsRed->InsertCellPoint(1);

  // use points and lines as polydata input
  vtkPolyData* polyRed = vtkPolyData::New();
  polyRed->SetPoints(pointsRed);
  polyRed->SetLines(cellsRed);

  // map polydata to 2d
  // by default, the 3d data is transformed into 2d by ignoring the z-coordinate
  // of the 3d points in vtkPolyData; see the vtkPolyDataMapper2D documentation
  vtkPolyDataMapper2D* mapperRed = vtkPolyDataMapper2D::New();
  mapperRed->SetInput(polyRed);

  if (this->Actor2DRed == NULL)
    {
    // first time called --> create and define properties
    this->Actor2DRed = vtkActor2D::New();
    this->Actor2DRed->GetProperty()->SetColor(1, 0, 0); // red
    this->Actor2DRed->GetProperty()->SetLineWidth(1);

    // add to renderer
    renderer->AddActor2D(this->Actor2DRed);
    }

  //----------------------------------------------------------------
  // Update both actors.
  //----------------------------------------------------------------
  this->Actor2DGreen->SetMapper(mapperGreen);
  this->Actor2DGreen->SetVisibility(1);

  this->Actor2DRed->SetMapper(mapperRed);
  this->Actor2DRed->SetVisibility(1);

  // check if needle is in front of the slice plane or behind it; this is
  // determined by checking the sign of the respective component of the
  // needle direction vector
  if (registeredTracker->Element[2][2] <= 0)
    {
    // needle is in front of the slice plane --> draw solid line(s)
    this->Actor2DGreen->GetProperty()->SetLineStipplePattern(0xFFFF); // draw every pixel: 0xFFFF = 1111...
    this->Actor2DGreen->GetProperty()->SetLineStippleRepeatFactor(1);
    this->Actor2DRed->GetProperty()->SetLineStipplePattern(0xFFFF); // draw every pixel: 0xFFFF = 1111...
    this->Actor2DRed->GetProperty()->SetLineStippleRepeatFactor(1);
    }
  else
    {
    // needle is behind the slice plane --> draw dashed line(s)
    this->Actor2DGreen->GetProperty()->SetLineStipplePattern(0xAAAA); // draw every second pixel: 0xAAAA = 1010...
    this->Actor2DGreen->GetProperty()->SetLineStippleRepeatFactor(1);
    this->Actor2DRed->GetProperty()->SetLineStipplePattern(0xAAAA); // draw every second pixel: 0xAAAA = 1010...
    this->Actor2DRed->GetProperty()->SetLineStippleRepeatFactor(1);
    }

  // TODO: both Modified() calls seem to be unnecessary --> remove?
  this->Actor2DGreen->Modified();
  this->Actor2DRed->Modified();

  // clean up
  RASToXY->Delete();
  trackerXY->Delete();
  pointsGreen->Delete();
  cellsGreen->Delete();
  polyGreen->Delete();
  mapperGreen->Delete();
  pointsRed->Delete();
  cellsRed->Delete();
  polyRed->Delete();
  mapperRed->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::CheckSliceNodes()
{
  if (this->SliceNode[0] == NULL)
    {
    this->SliceNode[0] = this->GetApplicationLogic()->GetSliceLogic("Red")->GetSliceNode();
    }
  if (this->SliceNode[1] == NULL)
    {
    this->SliceNode[1] = this->GetApplicationLogic()->GetSliceLogic("Yellow")->GetSliceNode();
    }
  if (this->SliceNode[2] == NULL)
    {
    this->SliceNode[2] = this->GetApplicationLogic()->GetSliceLogic("Green")->GetSliceNode();
    }
}


//---------------------------------------------------------------------------
int vtkAbdoNavLogic::SetSliceDriver(int sliceNode, const char* sliceDriver)
{
  if (sliceNode < 0 || sliceNode >= 3)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::SetSliceDriver(...): "
                  "Slice node index out of range!");
    return EXIT_FAILURE;
    }
  else if (strcmp(sliceDriver, "User") != 0 && strcmp(sliceDriver, "Locator") != 0)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::SetSliceDriver(...): "
                  "Unknown slice driver specifier!");
    return EXIT_FAILURE;
    }
  else if (this->AbdoNavNode == NULL)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::SetSliceDriver(...): "
                  "Couldn't retrieve AbdoNavNode!");
    return EXIT_FAILURE;
    }

  // don't do anything unless the new slice driver is different
  // from the current one (GUI doesn't keep track of the slice
  // driver, i.e. the user might "change" the slice driver from
  // "User" to "User" or "Locator" to "Locator")
  if (strcmp(sliceDriver, this->SliceDriver[sliceNode].c_str()) != 0)
    {
    // set pointers to the three slice nodes (if not already set)
    this->CheckSliceNodes();

    // set new slice driver
    this->SliceDriver[sliceNode] = sliceDriver;

    if (strcmp(sliceDriver, "User") == 0)
      {
      if (this->ObliqueReslicing)
        {
        // switch from oblique reslicing to user mode;
        // restore the slice node's initial orientation
        // (see ResliceNode(...) for an explanation)
        if (strcmp(this->SliceOrientation[sliceNode].c_str(), "Axial") == 0)
          {
          this->SliceNode[sliceNode]->SetOrientationToAxial();
          }
        else if (strcmp(this->SliceOrientation[sliceNode].c_str(), "Sagittal") == 0)
          {
          this->SliceNode[sliceNode]->SetOrientationToSagittal();
          }
        else if (strcmp(this->SliceOrientation[sliceNode].c_str(), "Coronal") == 0)
          {
          this->SliceNode[sliceNode]->SetOrientationToCoronal();
          }

        // fit image data back into the slice viewers
        if (strcmp(this->SliceNode[0]->GetOrientationString(), "Reformat") != 0 &&
            strcmp(this->SliceNode[1]->GetOrientationString(), "Reformat") != 0 &&
            strcmp(this->SliceNode[2]->GetOrientationString(), "Reformat") != 0)
          {
          // initial orientation of all slice nodes was restored; thus, request
          // GUI to fit all of them at once into their respective slice viewer
          this->InvokeEvent(RequestFitToBackground);
          }
        }
      }
    else if (strcmp(sliceDriver, "Locator") == 0)
      {
      vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackingTransformID()));
      if (tnode)
        {
        vtkIntArray* nodeEvents = vtkIntArray::New();
        nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
        vtkSetAndObserveMRMLNodeEventsMacro(this->RelativeTrackingTransform, tnode, nodeEvents);
        nodeEvents->Delete();
        tnode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
        }
      else
        {
        vtkErrorMacro("in vtkAbdoNavLogic::SetSliceDriver(...): "
                      "Couldn't retrieve tracking transform node!");
        return EXIT_FAILURE;
        }
      }
    }

  return EXIT_SUCCESS;
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ResliceNode(int sliceNode, vtkMatrix4x4* registeredTracker)
{
  //----------------------------------------------------------------
  // NOTE: A slice node's orientation string is automatically set to
  //       "Reformat" whenever SetSliceToRASByNTP(...) is called, i.e.
  //       when oblique reslicing is performed.
  //       Thus, a slice node's orientation string needs to be stored
  //       in array SliceOrientation[3] in order to retrieve its "ini-
  //       tial" orientation (i.e. the one before oblique reslicing)
  //       once the user switches from oblique to non-oblique reslicing
  //       or switches from oblique reslicing to user mode (the latter
  //       of which is handled in SetSliceDriver(...)). In both cases,
  //       the image data also needs to be fitted back into the slice
  //       viewers once the initial orientation of all slice nodes was
  //       restored.
  //----------------------------------------------------------------

  float tx = registeredTracker->Element[0][0]; float nx = registeredTracker->Element[0][2]; float px = registeredTracker->Element[0][3];
  float ty = registeredTracker->Element[1][0]; float ny = registeredTracker->Element[1][2]; float py = registeredTracker->Element[1][3];
  float tz = registeredTracker->Element[2][0]; float nz = registeredTracker->Element[2][2]; float pz = registeredTracker->Element[2][3];

  if (strcmp(this->SliceNode[sliceNode]->GetOrientationString(), "Reformat") == 0)
    {
    if (this->ObliqueReslicing)
      {
      // oblique reslicing
      this->SliceNode[sliceNode]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNode);
      }
    else
      {
      // switch from oblique to non-oblique reslicing;
      // restore the slice node's initial orientation
      if (strcmp(this->SliceOrientation[sliceNode].c_str(), "Axial") == 0)
        {
        this->SliceNode[sliceNode]->SetOrientationToAxial();
        }
      else if (strcmp(this->SliceOrientation[sliceNode].c_str(), "Sagittal") == 0)
        {
        this->SliceNode[sliceNode]->SetOrientationToSagittal();
        }
      else if (strcmp(this->SliceOrientation[sliceNode].c_str(), "Coronal") == 0)
        {
        this->SliceNode[sliceNode]->SetOrientationToCoronal();
        }

      this->SliceNode[sliceNode]->JumpSlice(px, py, pz);

      // fit image data back into the slice viewers
      if (strcmp(this->SliceNode[0]->GetOrientationString(), "Reformat") != 0 &&
          strcmp(this->SliceNode[1]->GetOrientationString(), "Reformat") != 0 &&
          strcmp(this->SliceNode[2]->GetOrientationString(), "Reformat") != 0)
        {
        // initial orientation of all slice nodes was restored; thus, request
        // GUI to fit all of them at once into their respective slice viewer
        this->InvokeEvent(RequestFitToBackground);
        }
      }
    }
  else if (strcmp(this->SliceNode[sliceNode]->GetOrientationString(), "Axial")    == 0 ||
           strcmp(this->SliceNode[sliceNode]->GetOrientationString(), "Sagittal") == 0 ||
           strcmp(this->SliceNode[sliceNode]->GetOrientationString(), "Coronal")  == 0)
    {
    if (this->ObliqueReslicing)
      {
      // switch from non-oblique to oblique reslicing;
      // store the slice node's initial orientation
      this->SliceOrientation[sliceNode] = this->SliceNode[sliceNode]->GetOrientationString();
      this->SliceNode[sliceNode]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNode);
      }
    else
      {
      // non-oblique reslicing
      this->SliceNode[sliceNode]->JumpSlice(px, py, pz);
      }
    }
}
