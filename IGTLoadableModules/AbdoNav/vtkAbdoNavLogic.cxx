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

/* MRML includes */
#include "vtkMRMLLinearTransformNode.h"

/* VTK includes */
#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkMath.h"
#include "vtkRegularPolygonSource.h"
#include "vtkRotationalExtrusionFilter.h"
#include "vtkSmartPointer.h"
#include "vtkTransformPolyDataFilter.h"

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
  this->LocatorFreezePosition = NULL;
  this->RegistrationTransform = NULL;
}


//---------------------------------------------------------------------------
vtkAbdoNavLogic::~vtkAbdoNavLogic()
{
  //----------------------------------------------------------------
  // Cleanup logic values.
  //----------------------------------------------------------------
  vtkSetMRMLNodeMacro(this->AbdoNavNode, NULL);

  if (this->RegistrationTransform)
    {
    this->RegistrationTransform->Delete();
    }
  if (this->LocatorFreezePosition)
    {
    this->LocatorFreezePosition->Delete();
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
  if (caller != NULL)
    {
    // vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

    // TODO: fill in or delete!
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::PerformRegistration()
{
  //----------------------------------------------------------------
  // Given the RAS coordinates of the guidance needle tip and a second
  // point on the guidance needle, the registration matrix is calculated
  // as follows:
  //
  // - The translational component of the registration matrix is given
  //   by the tip of the guidance needle.
  // - The rotational component of the registration matrix is given by:
  //    - The first vector, which is the direction vector of the guidance
  //      needle (i.e. subtracting the second point on the guidance needle
  //      from its tip).
  //    - The second vector, which is perpendicular to the first vector.
  //      Since there's an infinite number of vectors perpendicular to
  //      the first, the second vector must be chosen arbitrarily.
  //    - The third vector, which is perpendicular to the first and
  //      second vector and thus given by the cross product of these
  //      vectors.
  //
  // NOTE: The second vector must be chosen arbitrarily since we can't
  //       retrieve the orientation of the guidance needle around its
  //       long axis. However, since the needles are rotation symmetric,
  //       loosing this information is considered negligible.
  //----------------------------------------------------------------

  if (this->RegistrationTransform == NULL)
    {
    // create a new registration transform node and add it to the scene
    this->RegistrationTransform = vtkMRMLLinearTransformNode::New(); // deleted in destructor
    this->RegistrationTransform->SetName("AbdoNav-RegistrationTransform");
    this->RegistrationTransform->SetDescription("Created by AbdoNav");
    this->GetMRMLScene()->AddNode(this->RegistrationTransform);
    // set registration transform node ID in AbdoNavNode
    this->AbdoNavNode->SetRegistrationTransformID(this->RegistrationTransform->GetID());
    // get original tracker transform node and make it observe the registration transform node
    if (this->AbdoNavNode->GetOriginalTrackerTransformID() != NULL)
      {
      vtkMRMLLinearTransformNode* originalTrackerTransform = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetOriginalTrackerTransformID()));
      originalTrackerTransform->SetAndObserveTransformNodeID(this->RegistrationTransform->GetID());
      }
    else
      {
      vtkErrorMacro("in vtkAbdoNavLogic::PerformRegistration(): "
                    "Couldn't move original tracker transform node below registration transform node "
                    "because vtkMRMLAbdoNavNode::GetOriginalTrackerTransformID() returned NULL!");
      }
    }

  vtkMatrix4x4* registrationMatrix = vtkMatrix4x4::New();
  registrationMatrix->Identity();

  double* guidanceNeedleTip = this->AbdoNavNode->GetGuidanceNeedleTipRAS();
  double* guidanceNeedleSecond = this->AbdoNavNode->GetGuidanceNeedleSecondRAS();

  // translational component of the registration matrix is given by the tip of the guidance needle
  registrationMatrix->SetElement(0, 3, guidanceNeedleTip[0]);
  registrationMatrix->SetElement(1, 3, guidanceNeedleTip[1]);
  registrationMatrix->SetElement(2, 3, guidanceNeedleTip[2]);

  // calculation of the rotational component of the registration matrix
  double firstRotVec[3], secondRotVec[3], thirdRotVec[3];

  // calculate and normalize first vector, which is given by the direction vector of the guidance needle
  firstRotVec[0] = guidanceNeedleTip[0] - guidanceNeedleSecond[0];
  firstRotVec[1] = guidanceNeedleTip[1] - guidanceNeedleSecond[1];
  firstRotVec[2] = guidanceNeedleTip[2] - guidanceNeedleSecond[2];
  vtkMath::Normalize(firstRotVec);

  // since the second vector must be chosen arbitrarily anyway, use VTK to calculate the two missing normalized vectors
  vtkMath::Perpendiculars(firstRotVec, secondRotVec, thirdRotVec, NULL);

  // set rotational components of the registration matrix
  registrationMatrix->SetElement(0, 0, firstRotVec[0]);
  registrationMatrix->SetElement(1, 0, firstRotVec[1]);
  registrationMatrix->SetElement(2, 0, firstRotVec[2]);
  registrationMatrix->SetElement(0, 1, secondRotVec[0]);
  registrationMatrix->SetElement(1, 1, secondRotVec[1]);
  registrationMatrix->SetElement(2, 1, secondRotVec[2]);
  registrationMatrix->SetElement(0, 2, thirdRotVec[0]);
  registrationMatrix->SetElement(1, 2, thirdRotVec[1]);
  registrationMatrix->SetElement(2, 2, thirdRotVec[2]);

  // copy registration matrix into registration transform node
  this->RegistrationTransform->GetMatrixTransformToParent()->DeepCopy(registrationMatrix);
  this->RegistrationTransform->Modified();

  // cleanup
  registrationMatrix->Delete();
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
  // cleanup
  collection->Delete();

  return locatorModel;
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkAbdoNavLogic::EnableLocatorDriver(const char* locatorName)
{
  // TODO: all TransformModifiedEvents within the scope of this function seem to be unnecessary --> remove?

  // create the locator model
  vtkMRMLModelNode* locatorModel = this->AddLocatorModel(locatorName, 0.0, 1.0, 1.0);
  // get the tracker transform node
  vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetOriginalTrackerTransformID()));
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
    }

  return locatorModel;
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

  // nothing to do if locatorModel == NULL && vis == 0
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
    if (LocatorFreezePosition == NULL)
      {
      // LocatorFreezePosition is deleted in the destructor
      LocatorFreezePosition = vtkMatrix4x4::New();
      }
    // retrieve and store transform to world (transform to parent isn't sufficient since
    // the original tracker transform node observes AbdoNav's registration transform node)
    vtkMatrix4x4* tfmToWorld = vtkMatrix4x4::New();
    vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetOriginalTrackerTransformID()))->GetMatrixTransformToWorld(tfmToWorld);
    LocatorFreezePosition->DeepCopy(tfmToWorld);
    tfmToWorld->Delete();
    // apply current transformation matrix
    locatorModel->ApplyTransform(LocatorFreezePosition);
    locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    }
  else if (locatorModel != NULL && freeze == 0)
    {
    // need to apply the inverse transformation matrix so as to obtain the identity matrix
    vtkMatrix4x4::Invert(LocatorFreezePosition, LocatorFreezePosition);
    locatorModel->ApplyTransform(LocatorFreezePosition);
    // unfreeze locator model
    locatorModel->SetAndObserveTransformNodeID(vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetOriginalTrackerTransformID()))->GetID());
    locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    }

  // nothing to do if locatorModel == NULL
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
  // vendor (Galil Medical) brochure states: needle diameter = 17 gauge, 1.47 mm --> radius = 0.735 mm (weird because e.g. Wikipedia states that: 17 gauge ~ 1.150 mm  --> radius ~ 0.575 mm)
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
  // Add locator model to the scene.
  //----------------------------------------------------------------
  // prepare model node
  locatorPolyData->Update();
  locatorModel->SetAndObservePolyData(locatorPolyData->GetOutput());
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
