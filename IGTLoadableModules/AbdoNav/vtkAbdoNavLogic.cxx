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
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLLinearTransformNode.h"

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
  this->RegistrationTransform = NULL;
  this->RelativeTrackingTransform = NULL;
  this->LocatorFreezePosition = NULL;
  for (int i = 0; i < 3; i++)
      {
      this->SliceNode[i] = NULL;
      }
  this->Crosshair = NULL;
  this->AppGUI = NULL;
  this->Actor2DGreen = NULL;
  this->Actor2DRed = NULL;
  // initialize each slice node (Red == 0, Yellow == 1, Green == 2) to be driven by the user
  for (int i = 0; i < 3; i++)
    {
    this->SliceDriver[i] = "User";
    }
  this->RegistrationPerformed = 0;
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
  //----------------------------------------------------------------
  // remove AbdoNavNode observers
  vtkSetMRMLNodeMacro(this->AbdoNavNode, NULL);

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
  //----------------------------------------------------------------
  // Needle-based registration between tracking system coordinates and image
  // coordinates:
  //
  // Registration is performed under the assumption that the guidance needle
  // (whose needle artifact is visible in the MR image) acts as a reference
  // for the actual procedure needle (e.g. a cryoprobe). Thus, tracking data
  // of the procedure needle is always given relative to this reference. In
  // other words: if guidance and procedure needle were perfectly aligned in
  // physical space, the 4x4 transformation matrix describing the procedure
  // needle's orientation relative to the guidance needle would equal a 4x4
  // identity matrix.
  // Further, it is assumed that the translatory offset between a needle's
  // tip and its local coordinate system were determined. These offsets are
  // determined by pivoting each needle and, figuratively speaking, translate
  // a needle's local coordinate system from its original location to the
  // needle tip.
  //
  // Under these assumptions, registration between tracking coordinate system
  // and image coordinate system is performed by reconstructing the guidance
  // needle's coordinate system in the RAS image coordinate system.
  //
  //   | |x| |y| |z| |t| |
  //   | |x| |y| |z| |t| |
  //   | |x| |y| |z| |t| |
  //   |  0   0   0   1  |
  //
  // t:
  // ==
  // Since the guidance needle's local coordinate system is to be found at
  // its tip (see description above), t is given by identifying the tip of
  // the guidance needle artifact in the MR image.
  //
  // IMPORTANT NOTE:
  // ===============
  // The following description of reconstructing the x-, y- and z-axis of the
  // guidance needle's local coordinate system refers to the specific optical
  // marker attachment that was used during this project! In case your setup
  // differs from the one used during this project, registration is likely
  // to fail!
  //
  // z:
  // ==
  // The z-axis of the guidance needle's local coordinate system and the
  // guidance needle itself point in opposite directions. The z-axis can thus
  // be reconstructed by selecting a second point on the guidance needle
  // artifact, subtracting the previously selected tip from this second point
  // (order is important! otherwise one would obtain -z instead of +z!) and
  // normalizing the result.
  //
  // x:
  // ==
  // Due to rotational symmetry, a third point is required to determine the
  // needle's x-axis (equivalent to determining the needle's roll angle). In
  // the current setup, this point is given by the center of a marker which
  // is located in the xz-plane of the guidance needle's local coordinate
  // system. The x-axis is thus obtained by calculating the normalized vector
  // perpendicular to the z-axis and passing through the marker center.
  //
  // Since x and z are perpendicular, the scalar product equals 0, i.e. one
  // has to solve |x| * |z| = 0 for x. Let m denote the coordinates of the
  // marker center and let further i denote the coordinates of the intersecting
  // point of x and z. Then |x| = |m| - |i| (again, order is important!). Since
  // i is a point on z, it can be rewritten as |i| = |tip| + |lambda * z| where
  // tip denotes the known image coordinates of the previously selected guidance
  // needle tip. Thus, one has to solve
  //
  //      |mR - (tipR + lambda * zR)|   |zR|
  //      |mA - (tipA + lambda * zA)| * |zA| = 0
  //      |mS - (tipS + lambda * zS)|   |zS|
  //
  // for lambda, which yields
  //
  //      lambda = [zR * (mR - tipR) + zA * (mA -tipA) + zS * (mS - tipS)] /
  //               (zR^2 + zA^2 + zS^2)
  //
  // y:
  // ==
  // Calculating the normalized cross-product of the z- and x-axis (again,
  // order is important!) finally yields the missing y-axis.
  //
  //----------------------------------------------------------------

  // retrieve input parameters from the registration fiducial list
  float* guidanceNeedleTip = NULL;
  float* guidanceNeedleSecond = NULL;
  float* markerCenter = NULL;

  vtkMRMLFiducialListNode* fnode = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetRegistrationFiducialListID()));
  if (fnode != NULL)
    {
    for (int i = 0; i < fnode->GetNumberOfFiducials(); i++)
      {
      if (!strcmp(tipRAS, fnode->GetNthFiducialLabelText(i)))
        {
        guidanceNeedleTip = fnode->GetNthFiducialXYZ(i);
        }
      else if (!strcmp(sndRAS, fnode->GetNthFiducialLabelText(i)))
        {
        guidanceNeedleSecond = fnode->GetNthFiducialXYZ(i);
        }
      else if (!strcmp(markerRAS, fnode->GetNthFiducialLabelText(i)))
        {
        markerCenter = fnode->GetNthFiducialXYZ(i);
        }
      }
    }
  else
    {
    return EXIT_FAILURE;
    }

  // validate registration input parameters
  if (guidanceNeedleTip == NULL || guidanceNeedleSecond == NULL || markerCenter == NULL)
    {
    return EXIT_FAILURE;
    }


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

  vtkMatrix4x4* registrationMatrix = vtkMatrix4x4::New();
  registrationMatrix->Identity();

  //----------------------------------------------------------------
  // set translation t
  //----------------------------------------------------------------
  registrationMatrix->SetElement(0, 3, guidanceNeedleTip[0]);
  registrationMatrix->SetElement(1, 3, guidanceNeedleTip[1]);
  registrationMatrix->SetElement(2, 3, guidanceNeedleTip[2]);

  double x[3], y[3], z[3];

  //----------------------------------------------------------------
  // calculate z-axis
  //----------------------------------------------------------------
  // calculate the inverted direction vector of the guidance needle
  // artifact (order is important!)
  z[0] = guidanceNeedleSecond[0] - guidanceNeedleTip[0];
  z[1] = guidanceNeedleSecond[1] - guidanceNeedleTip[1];
  z[2] = guidanceNeedleSecond[2] - guidanceNeedleTip[2];
  // normalize the inverted direction vector
  vtkMath::Normalize(z);

  //----------------------------------------------------------------
  // calculate x-axis
  //----------------------------------------------------------------
  // calculate lambda
  double lambda = (z[0] * (markerCenter[0] - guidanceNeedleTip[0])  +
                   z[1] * (markerCenter[1] - guidanceNeedleTip[1])  +
                   z[2] * (markerCenter[2] - guidanceNeedleTip[2])) /
                  (pow(z[0], 2) + pow(z[1], 2) + pow(z[2], 2));
  // calculate intersecting point of x and z using lambda
  double i[3];
  i[0] = guidanceNeedleTip[0] + lambda * z[0];
  i[1] = guidanceNeedleTip[1] + lambda * z[1];
  i[2] = guidanceNeedleTip[2] + lambda * z[2];
  // calculate direction vector of x (order is important!)
  x[0] = markerCenter[0] - i[0];
  x[1] = markerCenter[1] - i[1];
  x[2] = markerCenter[2] - i[2];
  // normalize x
  vtkMath::Normalize(x);

  //----------------------------------------------------------------
  // calculate y-axis
  //----------------------------------------------------------------
  // calculate cross-product of z and x (order is important!)
  vtkMath::Cross(z, x, y);
  // normalize the result
  vtkMath::Normalize(y);

  //----------------------------------------------------------------
  // set x-, y- and z-axis
  //----------------------------------------------------------------
  registrationMatrix->SetElement(0, 0, x[0]); registrationMatrix->SetElement(0, 1, y[0]); registrationMatrix->SetElement(0, 2, z[0]);
  registrationMatrix->SetElement(1, 0, x[1]); registrationMatrix->SetElement(1, 1, y[1]); registrationMatrix->SetElement(1, 2, z[1]);
  registrationMatrix->SetElement(2, 0, x[2]); registrationMatrix->SetElement(2, 1, y[2]); registrationMatrix->SetElement(2, 2, z[2]);

  // copy registration matrix into registration transform node
  this->RegistrationTransform->GetMatrixTransformToParent()->DeepCopy(registrationMatrix);
  // indicate that the Scene contains unsaved changes; make Slicer's save dialog list this transform as modified
  this->RegistrationTransform->SetModifiedSinceRead(1);
  // indicate that registration has been performed
  this->RegistrationPerformed = 1;

  // clean up
  registrationMatrix->Delete();

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

    this->CheckSliceNode();

    // update each slice node that is driven by the locator
    for (int i = 0; i < 3; i++)
      {
      // driven by locator
      if (this->SliceDriver[i] == "Locator")
        {
        if (registeredTracker != NULL)
          {
          this->UpdateSliceNode(i, registeredTracker);
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
    if (this->AppGUI != NULL)
      {
      this->UpdateNeedleProjection(registeredTracker);
      }

    // clean up
    registeredTracker->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateNeedleProjection(vtkMatrix4x4* registeredTracker)
{
  // calculate angle alpha between needle and slice plane according to the formula
  // below where u denotes the direction vector of the needle and n denotes the
  // normal vector of the slice plane:
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
  std::cout << "off-plane-angle = " << angle << std::endl;

  // get matrix that converts XY coordinates to RAS
  vtkMatrix4x4* RASToXY = vtkMatrix4x4::New();
  RASToXY->DeepCopy(this->GetApplicationLogic()->GetSliceLogic("Red")->GetSliceNode()->GetXYToRAS());
  // invert it and convert registered tracking coordinates from RAS to XY
  RASToXY->Invert();
  vtkMatrix4x4* trackerXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(RASToXY, registeredTracker, trackerXY);

  vtkRenderer* renderer = this->AppGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderer();

  if (this->DrawNeedleProjection)
    {
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
    pointsGreen->Delete();
    cellsGreen->Delete();
    polyGreen->Delete();
    mapperGreen->Delete();
    pointsRed->Delete();
    cellsRed->Delete();
    polyRed->Delete();
    mapperRed->Delete();
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
  RASToXY->Delete();
  trackerXY->Delete();
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
vtkMRMLModelNode* vtkAbdoNavLogic::EnableLocatorDriver(const char* locatorName)
{
  // TODO: all TransformModifiedEvents within the scope of this function seem to be unnecessary --> remove?

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
void vtkAbdoNavLogic::CheckSliceNode()
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
int vtkAbdoNavLogic::SetSliceDriver(int sliceIndex, const char* driver)
{
  if (sliceIndex < 0 || sliceIndex >= 3)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::SetSliceDriver(...): "
                  "Index out of range!");
    return EXIT_FAILURE;
    }
  else if (strcmp(driver, "User") != 0 && strcmp(driver, "Locator") != 0)
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
  // User to User or Locator to Locator)
  if (strcmp(driver, this->SliceDriver[sliceIndex].c_str()) != 0)
    {
    // if necessary, set pointers to access the three slice nodes
    this->CheckSliceNode();

    // set new slice driver
    this->SliceDriver[sliceIndex] = driver;

    if (strcmp(driver, "User") == 0)
      {
      if (this->ObliqueReslicing)
        {
        // switch from oblique reslicing to user mode;
        // restore the slice node's initial orientation
        // (see UpdateSliceNode(...) for an explanation)
        if (strcmp(this->SliceOrientation[sliceIndex].c_str(), "Axial") == 0)
          {
          this->SliceNode[sliceIndex]->SetOrientationToAxial();
          }
        else if (strcmp(this->SliceOrientation[sliceIndex].c_str(), "Sagittal") == 0)
          {
          this->SliceNode[sliceIndex]->SetOrientationToSagittal();
          }
        else if (strcmp(this->SliceOrientation[sliceIndex].c_str(), "Coronal") == 0)
          {
          this->SliceNode[sliceIndex]->SetOrientationToCoronal();
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
    else if (strcmp(driver, "Locator") == 0)
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
void vtkAbdoNavLogic::UpdateSliceNode(int sliceNodeIndex, vtkMatrix4x4* registeredTracker)
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

  if (strcmp(this->SliceNode[sliceNodeIndex]->GetOrientationString(), "Reformat") == 0)
    {
    if (this->ObliqueReslicing)
      {
      // oblique reslicing
      this->SliceNode[sliceNodeIndex]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeIndex);
      }
    else
      {
      // switch from oblique to non-oblique reslicing;
      // restore the slice node's initial orientation
      if (strcmp(this->SliceOrientation[sliceNodeIndex].c_str(), "Axial") == 0)
        {
        this->SliceNode[sliceNodeIndex]->SetOrientationToAxial();
        }
      else if (strcmp(this->SliceOrientation[sliceNodeIndex].c_str(), "Sagittal") == 0)
        {
        this->SliceNode[sliceNodeIndex]->SetOrientationToSagittal();
        }
      else if (strcmp(this->SliceOrientation[sliceNodeIndex].c_str(), "Coronal") == 0)
        {
        this->SliceNode[sliceNodeIndex]->SetOrientationToCoronal();
        }

      this->SliceNode[sliceNodeIndex]->JumpSlice(px, py, pz);

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
  else if (strcmp(this->SliceNode[sliceNodeIndex]->GetOrientationString(), "Axial")    == 0 ||
           strcmp(this->SliceNode[sliceNodeIndex]->GetOrientationString(), "Sagittal") == 0 ||
           strcmp(this->SliceNode[sliceNodeIndex]->GetOrientationString(), "Coronal")  == 0)
    {
    if (this->ObliqueReslicing)
      {
      // switch from non-oblique to oblique reslicing;
      // store the slice node's initial orientation
      this->SliceOrientation[sliceNodeIndex] = this->SliceNode[sliceNodeIndex]->GetOrientationString();
      this->SliceNode[sliceNodeIndex]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeIndex);
      }
    else
      {
      // non-oblique reslicing
      this->SliceNode[sliceNodeIndex]->JumpSlice(px, py, pz);
      }
    }

  this->SliceNode[sliceNodeIndex]->UpdateMatrices();
}
