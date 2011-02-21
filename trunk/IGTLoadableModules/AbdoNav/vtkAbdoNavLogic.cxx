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
  this->OriginalTrackerTransform = NULL;
  this->LocatorFreezePosition = NULL;
  for (int i = 0; i < 3; i++)
      {
      this->SliceNode[i] = NULL;
      }
  this->Crosshair = NULL;
  this->AppGUI = NULL;
  this->Actor2DGreen = NULL;
  this->Actor2DRed = NULL;
  // initialize each slice orientation (Red == 0, Yellow == 1, Green == 2) to be driven by the user (== 0)
  for (int i = 0; i < 3; i++)
    {
    this->SliceDriver[i] = 0;
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
  // Cleanup logic values.
  //----------------------------------------------------------------
  vtkSetMRMLNodeMacro(this->AbdoNavNode, NULL);

  if (this->RegistrationTransform)
    {
    this->RegistrationTransform->Delete();
    }
  if (this->OriginalTrackerTransform)
    {
    this->OriginalTrackerTransform->Delete();
    }
  if (this->LocatorFreezePosition)
    {
    this->LocatorFreezePosition->Delete();
    }
  if (this->Crosshair)
    {
    this->Crosshair->Delete();
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
      this->AbdoNavNode->GetOriginalTrackerTransformID() != NULL &&
      strcmp(node->GetID(), this->AbdoNavNode->GetOriginalTrackerTransformID()) == 0)
    {

    if (this->RegistrationPerformed == true)
      {
      this->UpdateAll();
      }

    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::PerformRegistration()
{
  //----------------------------------------------------------------
  // Needle-based registration between tracking system coordinates and
  // image coordinates:
  //
  // It is assumed that the guidance needle visible in the CT scan acts
  // as a reference for the cryoprobe. That is, tracking information of
  // the cryoprobe is always given relative to the reference (i.e. if
  // the cryoprobe and the reference were perfectly aligned, the tracking
  // data received from the cryoprobe would equal a 4x4 identity matrix).
  //
  // Under this assumption, registration is simply the process of applying
  // the position and orientation of the guidance needle tip in the CT scan
  // to the cryoprobe. Hence, the registration matrix can be derived from a
  // set of two points given in RAS coordinates of the CT scan: the tip of
  // the guidance needle and a second point on the guidance needle.
  //
  // Calculation is performed as follows:
  // - First, the direction vector V1 of the needle is calculated by sub-
  //   tracting the second point on the guidance needle from its tip and
  //   then normalizing it.
  // - Second, a vector V2 perpendicular to the first is calculated. Since
  //   there's an infinite number of vectors perpendicular to V1, an arbi-
  //   trary choice must be made.
  // - Third, the vector V3 perpendicular to vectors V1 and V2 is obtained
  //   by calculating the cross product of these vectors.
  //
  // The second vector V2 must be chosen arbitrarily since the needle is
  // rotation symmetric and thus, the orientation around its long axis
  // cannot be reconstructed from a CT scan. For the same reason, however,
  // orientation around the long axis is negligible.
  //
  // The registration is then composed as follows:
  // - The rotational components R1, R2 and R3 of the registration matrix
  //   are given by the three vectors V1, V2 and V3 calculated as described
  //   above. The correspondence between V1, V2, V3 and R1, R2, R3 is de-
  //   fined by the coordinate definition of the sensor being used to track
  //   the guidance needle. In case of the NDI Polaris Vicra Probe (NDI,
  //   part number 8700340) for example, the Probe's +z-axis is given by
  //   the inverted direction vector V1, the +x-axis corresponds to the vec-
  //   tor V2 perpendicular to direction vector and the +y-axis corresponds
  //   to V3. In case of the NDI Polaris Vicra Probe, vectors R1, R2 and R3
  //   of the registration matrix hence correspond to V2, V3, -V1 respectively.
  // - Finally, the translational component T of the registration matrix is
  //   directly given by the RAS coordinates of the guidance needle tip.
  //
  //   | |R1| |R2| |R3| |T| |
  //   | |R1| |R2| |R3| |T| |
  //   | |R1| |R2| |R3| |T| |
  //   |  0    0    0    1  |
  //
  // Implementation specifics:
  // vtkMath::Perpendiculars(const double x[3], double y[3], double z[3],
  // double theta) is used to conveniently calculate vectors V2 and V3
  // from vector V1.
  //
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

  //----------------------------------------------------------------
  // the translational component T of the registration matrix is
  // directly given by the RAS coordinates of the guidance needle tip
  //----------------------------------------------------------------
  registrationMatrix->SetElement(0, 3, guidanceNeedleTip[0]);
  registrationMatrix->SetElement(1, 3, guidanceNeedleTip[1]);
  registrationMatrix->SetElement(2, 3, guidanceNeedleTip[2]);

  //----------------------------------------------------------------
  // calculation of vectors V1, V2 and V3 as described in the comment
  // above
  //----------------------------------------------------------------
  double v1[3], v2[3], v3[3];

  // calculate and normalize the direction vector of the guidance needle
  v1[0] = guidanceNeedleTip[0] - guidanceNeedleSecond[0];
  v1[1] = guidanceNeedleTip[1] - guidanceNeedleSecond[1];
  v1[2] = guidanceNeedleTip[2] - guidanceNeedleSecond[2];
  vtkMath::Normalize(v1);

  // since the second vector must be chosen arbitrarily anyway, use
  // VTK to conveniently calculate the two missing normalized vectors
  // perpendicular to V1
  vtkMath::Perpendiculars(v1, v3, v2, 0.0);

  // set rotational components R1, R2 and R3 of the registration matrix
  // according to the coordinate definition of the NDI Polaris Vicra Probe
  registrationMatrix->SetElement(0, 0, v2[0]); registrationMatrix->SetElement(0, 1, v3[0]); registrationMatrix->SetElement(0, 2, -v1[0]);
  registrationMatrix->SetElement(1, 0, v2[1]); registrationMatrix->SetElement(1, 1, v3[1]); registrationMatrix->SetElement(1, 2, -v1[1]);
  registrationMatrix->SetElement(2, 0, v2[2]); registrationMatrix->SetElement(2, 1, v3[2]); registrationMatrix->SetElement(2, 2, -v1[2]);

  // copy registration matrix into registration transform node
  this->RegistrationTransform->GetMatrixTransformToParent()->DeepCopy(registrationMatrix);
  this->RegistrationTransform->Modified();
  // indicate that registration has been performed
  this->RegistrationPerformed = 1;

  // cleanup
  registrationMatrix->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::SetSliceDriver(int sliceIndex, const char* driver)
{
  if (sliceIndex < 0 || sliceIndex >= 3)
    {
    vtkErrorMacro("in vtkAbdoNavLogic::SetSliceDriver(...): "
                  "Index out of range!");
    return;
    }

  this->CheckSliceNode();

  if (strcmp(driver, "User") == 0)
    {
    switch (sliceIndex) {
      case 0:
        this->SliceNode[sliceIndex]->SetOrientationToAxial();
        break;
      case 1:
        this->SliceNode[sliceIndex]->SetOrientationToSagittal();
        break;
      case 2:
        this->SliceNode[sliceIndex]->SetOrientationToCoronal();
        break;
      default:
        break;
    }
    this->SliceDriver[sliceIndex] = 0;
    }
  else if (strcmp(driver, "Locator") == 0)
    {
    this->SliceDriver[sliceIndex] = 1;
    vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetOriginalTrackerTransformID()));
    if (tnode)
      {
      vtkIntArray* nodeEvents = vtkIntArray::New();
      nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
      vtkSetAndObserveMRMLNodeEventsMacro(this->OriginalTrackerTransform, tnode, nodeEvents);
      nodeEvents->Delete();
      tnode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
      }
    }
  else
    {
    vtkErrorMacro("in vtkAbdoNavLogic::SetSliceDriver(...): "
                  "Unknown slice driver specifier!");
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateAll()
{
  if (this->FreezeReslicing == false)
    {
    // get registered tracking data for reslicing
    vtkMatrix4x4* registeredTracker = vtkMatrix4x4::New();
    vtkMRMLLinearTransformNode* originalTracker = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetOriginalTrackerTransformID()));
    if (originalTracker != NULL)
      {
      originalTracker->GetMatrixTransformToWorld(registeredTracker);
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
      if (this->SliceDriver[i] == 1)
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

    // cleanup
    registeredTracker->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateSliceNode(int sliceNodeIndex, vtkMatrix4x4* registeredTracker)
{
  float tx = registeredTracker->Element[0][0]; float nx = registeredTracker->Element[0][2]; float px = registeredTracker->Element[0][3];
  float ty = registeredTracker->Element[1][0]; float ny = registeredTracker->Element[1][2]; float py = registeredTracker->Element[1][3];
  float tz = registeredTracker->Element[2][0]; float nz = registeredTracker->Element[2][2]; float pz = registeredTracker->Element[2][3];

  if (strcmp(this->SliceNode[sliceNodeIndex]->GetOrientationString(), "Axial") == 0)
    {
    if (this->ObliqueReslicing)
      {
      // store orientation of this slice view
      this->SliceOrientation[sliceNodeIndex] = SLICE_ORIENT_AXIAL;
      this->SliceNode[sliceNodeIndex]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeIndex);
      }
    else
      {
      this->SliceNode[sliceNodeIndex]->SetOrientationToAxial();
      this->SliceNode[sliceNodeIndex]->JumpSlice(px, py, pz);
      }
    }
  else if (strcmp(this->SliceNode[sliceNodeIndex]->GetOrientationString(), "Sagittal") == 0)
    {
    if (this->ObliqueReslicing)
      {
      // store orientation of this slice view
      this->SliceOrientation[sliceNodeIndex] = SLICE_ORIENT_SAGITTAL;
      this->SliceNode[sliceNodeIndex]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeIndex);
      }
    else
      {
      this->SliceNode[sliceNodeIndex]->SetOrientationToSagittal();
      this->SliceNode[sliceNodeIndex]->JumpSlice(px, py, pz);
      }
    }
  else if (strcmp(this->SliceNode[sliceNodeIndex]->GetOrientationString(), "Coronal") == 0)
    {
    if (this->ObliqueReslicing)
      {
      // store orientation of this slice view
      this->SliceOrientation[sliceNodeIndex] = SLICE_ORIENT_CORONAL;
      this->SliceNode[sliceNodeIndex]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeIndex);
      }
    else
      {
      this->SliceNode[sliceNodeIndex]->SetOrientationToCoronal();
      this->SliceNode[sliceNodeIndex]->JumpSlice(px, py, pz);
      }
    }
  else if (strcmp(this->SliceNode[sliceNodeIndex]->GetOrientationString(), "Reformat") == 0)
    {
    if (this->ObliqueReslicing)
      {
      this->SliceNode[sliceNodeIndex]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, sliceNodeIndex);
      }
    else
      {
      if (this->SliceOrientation[sliceNodeIndex] == SLICE_ORIENT_AXIAL)
        {
        this->SliceNode[sliceNodeIndex]->SetOrientationToAxial();
        }
      else if (this->SliceOrientation[sliceNodeIndex] == SLICE_ORIENT_SAGITTAL)
        {
        this->SliceNode[sliceNodeIndex]->SetOrientationToSagittal();
        }
      else if (this->SliceOrientation[sliceNodeIndex] == SLICE_ORIENT_CORONAL)
        {
        this->SliceNode[sliceNodeIndex]->SetOrientationToCoronal();
        }

      this->SliceNode[sliceNodeIndex]->JumpSlice(px, py, pz);
      }
    }

  this->SliceNode[sliceNodeIndex]->UpdateMatrices();
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

    // cleanup
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

  // cleanup
  RASToXY->Delete();
  trackerXY->Delete();
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
