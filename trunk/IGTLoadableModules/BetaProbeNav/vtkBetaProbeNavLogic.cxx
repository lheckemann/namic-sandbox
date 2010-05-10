/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkBetaProbeNavLogic.h"
#include "vtkMatrix4x4.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkPoints.h"
#include "vtkFloatArray.h"
#include "vtkCharArray.h"
#include "vtkMRMLUDPServerNode.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkAppendPolyData.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLHybridNavToolNode.h"
#include "vtkSphereSource.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkType.h"
#include "vtkImageData.h"

#include "vtkPointLocator.h"
#include "vtkCellLocator.h"
#include "vtkGenericCell.h"
#include "vtkPolyDataReader.h"

#include <math.h>

vtkCxxRevisionMacro(vtkBetaProbeNavLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkBetaProbeNavLogic);

//---------------------------------------------------------------------------
vtkBetaProbeNavLogic::vtkBetaProbeNavLogic()
{
  // Timer Handling
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBetaProbeNavLogic::DataCallback);
  //Variables
  this->Points = vtkPoints::New();
  this->SmoothScalars = vtkFloatArray::New();
  this->BetaScalars = vtkFloatArray::New();
  this->GammaScalars = vtkFloatArray::New();
  this->TimeStamp = vtkCharArray::New();
  this->TimeStamp->SetName("Time");
  this->nSmoothScalars = vtkFloatArray::New();
  this->nSmoothScalars->SetName("Smoothed");
  this->nBetaScalars = vtkFloatArray::New();
  this->nBetaScalars->SetName("Beta");
  this->nGammaScalars = vtkFloatArray::New();
  this->nGammaScalars->SetName("Gamma");
  this->CountMap = vtkPolyData::New();
  //this->kdTree = vtkKdTree::New();
  this->pointLocator = vtkPointLocator::New();
  this->cellLocator = vtkCellLocator::New();
  this->image = NULL;
  this->probeDiam = 9.0; //(mm) make sure it is an odd number
  //dispNode = vtkMRMLModelDisplayNode::New();
  this->interPoint[0] = 0;
  this->interPoint[1] = 0;
  this->interPoint[2] = 0;
}

//---------------------------------------------------------------------------
vtkBetaProbeNavLogic::~vtkBetaProbeNavLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
  
  //Clean up
  if (this->Points)
   this->Points->Delete();
  if (this->SmoothScalars)
    this->SmoothScalars->Delete();
  if (this->BetaScalars)
    this->BetaScalars->Delete();
  if (this->GammaScalars)
    this->GammaScalars->Delete();
  if (this->TimeStamp)
    this->TimeStamp->Delete();
  if (this->nSmoothScalars)
    this->nSmoothScalars->Delete();
  if (this->nBetaScalars)
    this->nBetaScalars->Delete();
  if (this->nGammaScalars)
    this->nGammaScalars->Delete();
  if (this->CountMap)
    this->CountMap->Delete();
  if (this->pointLocator)
    this->pointLocator->Delete();
  if (this->cellLocator)
    this->cellLocator->Delete();
  if (this->image)
    this->image->Delete();
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkBetaProbeNavLogic:             " << this->GetClassName() << "\n";

}

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkBetaProbeNavLogic *self = reinterpret_cast<vtkBetaProbeNavLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBetaProbeNavLogic DataCallback");
  self->UpdateAll();
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::UpdateAll()
{

}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkBetaProbeNavLogic::BuildLocators(vtkMRMLModelNode* mnode)
{
  //Check integrity of model node
  if (!mnode)
    {
    std::cerr << "Model Node not valid" << std::endl;
    return mnode;
    }

  //Extract polydata from the model node
  vtkPolyData* polydata = mnode->GetPolyData();

  //Create Scalar Arrays to add to polydata
  vtkFloatArray* SmoothCounts = vtkFloatArray::New();
  SmoothCounts->SetName("SmoothCounts");
  vtkFloatArray* BetaCounts = vtkFloatArray::New();
  BetaCounts->SetName("BetaCounts");
  vtkFloatArray* GammaCounts = vtkFloatArray::New();
  GammaCounts->SetName("GammaCounts");

  //Create Normalized Scalar Arrays to add to polydata
  vtkFloatArray* normSmoothCounts = vtkFloatArray::New();
  normSmoothCounts->SetName("NormSmoothCounts");
  vtkFloatArray* normBetaCounts = vtkFloatArray::New();
  normBetaCounts->SetName("NormBetaCounts");
  vtkFloatArray* normGammaCounts = vtkFloatArray::New();
  normGammaCounts->SetName("NormGammaCounts");

  /*//Create TimeStamp data to add to polydata
  vtkStringArray* TimeStampData = vtkStringArray::New();
  TimeStampData->SetName("TimeStamp");*/

  //Fill in values of these Arrays
  for (int i = 0; i < polydata->GetPoints()->GetNumberOfPoints(); i++)
    {
    SmoothCounts->InsertNextTuple1(0.0);
    BetaCounts->InsertNextTuple1(0.0);
    GammaCounts->InsertNextTuple1(0.0);
    normSmoothCounts->InsertNextTuple1(0.0);
    normBetaCounts->InsertNextTuple1(0.0);
    normGammaCounts->InsertNextTuple1(0.0);
    }
  /*for (int i = 0; i < polydata->GetPoints()->GetNumberOfPoints(); i++)
    {
    TimeStampData->InsertNextValue("00:00:00");
    }*/

  //Assign arrays to polydata
  polydata->GetPointData()->AddArray(SmoothCounts);
  polydata->GetPointData()->AddArray(BetaCounts);
  polydata->GetPointData()->AddArray(GammaCounts);
  polydata->GetPointData()->AddArray(normSmoothCounts);
  polydata->GetPointData()->AddArray(normBetaCounts);
  polydata->GetPointData()->AddArray(normGammaCounts);
  //polydata->GetPointData()->AddArray(TimeStampData);

  //Assign active Array
  switch (this->ActiveDataType)
    {
    case 0:
      polydata->GetPointData()->SetActiveScalars("NormSmoothCounts");
      break;
    case 1:
      polydata->GetPointData()->SetActiveScalars("NormBetaCounts");
      break;
    default:
      polydata->GetPointData()->SetActiveScalars("NormGammaCounts");
    }

  //Build Point Locator for later point search
  if (!this->pointLocator)
    {
    this->pointLocator = vtkPointLocator::New();
    }
  this->pointLocator->SetDataSet(polydata);
  this->pointLocator->AutomaticOn();
  this->pointLocator->SetNumberOfPointsPerBucket(2);
  this->pointLocator->BuildLocator();

  //Build Cell Locator for later point search
  if (!this->cellLocator)
    {
    this->cellLocator = vtkCellLocator::New();
    }
  cellLocator->SetDataSet(polydata);
  cellLocator->AutomaticOn();
  cellLocator->SetNumberOfCellsPerBucket(10);
  cellLocator->BuildLocator();

  //Append polydata back to model data
  mnode->SetAndObservePolyData(polydata);
  std::cerr << "Point and Cell Locators have been built" << std::endl;

  //Clean up and return
  SmoothCounts->Delete();
  BetaCounts->Delete();
  GammaCounts->Delete();
  normSmoothCounts->Delete();
  normBetaCounts->Delete();
  normGammaCounts->Delete();
  return mnode;
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::CollectData(vtkMRMLNode* tn, vtkMRMLNode* cn)
{
  std::cerr << "Collect Data started" << std::endl;

  //----------------------------
  //Position Data
  //----------------------------

  vtkMRMLHybridNavToolNode* ToolNode = vtkMRMLHybridNavToolNode::SafeDownCast(tn);
  if (!ToolNode)
    {
    return;
    }
    //Define tool tip position vector
    double pos[3];
    //Extract tool tip position from ToolNode
    vtkMatrix4x4* ToolTransform = vtkMatrix4x4::New();
    ToolNode->GetParentTransformNode()->GetMatrixTransformToWorld(ToolTransform);
    vtkMatrix4x4* ToolTipTransform = vtkMatrix4x4::New();
    vtkMatrix4x4* CalibrationMatrix = vtkMatrix4x4::New();
    ToolNode->GetCalibrationMatrix(CalibrationMatrix);
    ToolTipTransform->Multiply4x4(ToolTransform, CalibrationMatrix, ToolTipTransform);
    //Print out values onto screen
    std::cerr << "ToolTransform:" << std::endl;
    ToolTransform->Print(std::cerr);
    std::cerr << "ToolNode:" << std::endl;
    CalibrationMatrix->Print(std::cerr);
    std::cerr << "ToolTipTransform" << std::endl;
    ToolTipTransform->Print(std::cerr);
    //Extract point from ToolTipTransform matrix
    pos[0] = ToolTipTransform->GetElement(0, 3);
    pos[1] = ToolTipTransform->GetElement(1, 3);
    pos[2] = ToolTipTransform->GetElement(2, 3);
    //Add point to Points Array
    std::cerr << "Added point to array: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
    this->Points->InsertNextPoint(pos);

  //-------------------------------------------------------------
  //Calculate intersection point between probe axis and polydata
  //-------------------------------------------------------------
  //Define point arrays
  double p1[3];
  double intPoint[3] = {0, 0, 0};

  //Obtain the position p1 of the tip from the Points array
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, p1);

  //Calculate probe axis using two points, the first the tip of the probe p1 and the second
  //point at k units away from the tip in the direction of the probe axis
  double dir[3];
  double p2[3];

  //Direction vector of the probe axis
  dir[0] = ToolTipTransform->GetElement(0,2);
  dir[1] = ToolTipTransform->GetElement(1,2);
  dir[2] = ToolTipTransform->GetElement(2,2);
  double k = 50.0; //units from the probe tip
  for (int i = 0; i < 3; i++)
    p2[i] = p1[i] + k*dir[i];

  //Calculate intersection point between probe and polydata and save point in intPoint
  double t = 0.0;
  double pcoords[3] = {0, 0, 0};
  int subId = 0;
  vtkIdType cellid = -1;
  vtkGenericCell *cell = vtkGenericCell::New();
  if (this->cellLocator->IntersectWithLine(p1, p2, 0.001, t, intPoint, pcoords, subId, cellid, cell) != 0)
    {
    //Print out intersection point
    std::cerr << "Intersection Coordinates: " << intPoint[0] << " " << intPoint[1] << " " << intPoint[2] << std::endl;
    //Calculate height of probe in relation to surface of model
    this->ProbeHeight = sqrt(pow((p1[0]-intPoint[0]),2)+pow((p1[1]-intPoint[1]),2)+pow((p1[2]-intPoint[2]),2));
    //Store intersection point to send to GUI
    this->interPoint[0] = intPoint[0];
    this->interPoint[1] = intPoint[1];
    this->interPoint[2] = intPoint[2];
    }
  else
    {
    //No intersection made
    std::cerr << "No intersection with the model" << std::endl;
    this->ProbeHeight = -1;
    }

  //---------------------------------------
  // Counts Data
  //---------------------------------------
  //Correct values for height
  double cf; //correction factor
  cf = CorrectionFactorHeight(this->ProbeHeight);
  std::cerr << "Correction Factor" << cf << std::endl;

  //Extract counts from CountNode and append to Float Arrays
  vtkMRMLUDPServerNode* cnode = vtkMRMLUDPServerNode::SafeDownCast(cn);
  this->SmoothScalars->InsertNextTuple1(cf*(cnode->GetSmoothedCounts()));
  this->BetaScalars->InsertNextTuple1(cf*(cnode->GetBetaCounts()));
  this->GammaScalars->InsertNextTuple1(cf*(cnode->GetGammaCounts()));
  this->TimeStamp->InsertNextTupleValue((cnode->GetTime()).c_str());

  //Normalize value of Scalars for Color mapping purposes
  //Smooth Scalars
  double normScalar;
  normScalar = this->SmoothScalars->GetTuple1(this->SmoothScalars->GetNumberOfTuples()-1);
  normScalar = (int) (255*normScalar/(cf*this->maxRange));
  this->nSmoothScalars->InsertNextTuple1(normScalar);

  //BetaScalars
  normScalar = this->BetaScalars->GetTuple1(this->BetaScalars->GetNumberOfTuples()-1);
  normScalar = (int) (255*normScalar/(cf*this->maxRange));
  this->nBetaScalars->InsertNextTuple1(normScalar);

  //GammaScalars
  normScalar = this->GammaScalars->GetTuple1(this->GammaScalars->GetNumberOfTuples()-1);
  normScalar = (int) (255*normScalar/(cf*this->maxRange));
  this->nGammaScalars->InsertNextTuple1(normScalar);

  //Clean Up
  cell->Delete();
  ToolTransform->Delete();
  ToolTipTransform->Delete();
  CalibrationMatrix->Delete();
}

//----------------------------------------------------------------------------
void vtkBetaProbeNavLogic::ClearArrays()
{
  //Clear All variables
  if (this->CountMap)
    this->CountMap->Reset();
  if (this->Points)
    this->Points->Reset();
  if (this->SmoothScalars)
    this->SmoothScalars->Reset();
  if (this->BetaScalars)
    this->BetaScalars->Reset();
  if (this->GammaScalars)
    this->GammaScalars->Reset();
  if (this->nSmoothScalars)
    this->nSmoothScalars->Reset();
  if (this->nBetaScalars)
    this->nBetaScalars->Reset();
  if (this->nGammaScalars)
    this->nGammaScalars->Reset();
  if (this->TimeStamp)
    this->TimeStamp->Reset();
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkBetaProbeNavLogic::PaintImage(vtkMRMLScalarVolumeNode* inode,
               vtkMRMLScalarVolumeNode* snode, vtkMRMLModelNode* mnode, vtkMRMLHybridNavToolNode* tnode)
{
  if (!(inode && snode && mnode && tnode))
    {
    return snode;
    }
  
  // Check to see if image has been created
  if (!this->image)
    {
    std::cerr << "Creating a new image" << std::endl;
    //Create image node
    this->image = vtkImageData::New();
    
    //Get image characteristics from vnode
    vtkImageData* im = inode->GetImageData();
    int dim[3];
    im->GetDimensions(dim);
    this->image->SetDimensions(dim[0], dim[1], dim[2]);
    int ext[6];
    im->GetExtent(ext);
    this->image->SetExtent(ext);
    double sp[3];
    im->GetSpacing(sp);
    this->image->SetSpacing(sp);
    im->GetOrigin(sp);
    this->image->SetOrigin(sp);
    this->image->SetNumberOfScalarComponents(1);
    this->image->SetScalarTypeToShort();
    this->image->AllocateScalars();
    
    //Assign properties to snode
    snode->SetOrigin(inode->GetOrigin());
    snode->SetAndObserveImageData(this->image);
    }
  
  //Extract IJK to RAS Matrix
  vtkMatrix4x4* matrixIJKtoRAS = vtkMatrix4x4::New();
  inode->GetIJKToRASMatrix(matrixIJKtoRAS);
  matrixIJKtoRAS->Print(std::cerr);
  
  //Set this matrix to snode
  snode->SetIJKToRASMatrix(matrixIJKtoRAS);
  
  //-------------------------------------------------------------
  //Calculate intersection point between probe axis and polydata
  //-------------------------------------------------------------
  //Define point arrays
  double p1[3];
  double intPoint[3] = {0, 0, 0};

  //Obtain the position p1 of the tip from the Points array
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, p1);

  //Calculate probe axis using two points, the first the tip of the probe p1 and the second
  //point at k units away from the tip in the direction of the probe axis
  double dir[3];
  double p2[3];

  //Calculate probe axis from tool
  vtkMatrix4x4* SensorAxisMatrix = vtkMatrix4x4::New();
  tnode->GetParentTransformNode()->GetMatrixTransformToWorld(SensorAxisMatrix);
  vtkMatrix4x4* ToolTipMatrix = vtkMatrix4x4::New();
  tnode->GetCalibrationMatrix(ToolTipMatrix);
  vtkMatrix4x4* ToolTipAxis = vtkMatrix4x4::New();
  ToolTipAxis->Multiply4x4(SensorAxisMatrix, ToolTipMatrix, ToolTipAxis);
  //Direction vector of the probe axis
  dir[0] = ToolTipAxis->GetElement(0,2);
  dir[1] = ToolTipAxis->GetElement(1,2);
  dir[2] = ToolTipAxis->GetElement(2,2);
  double k = 50.0; //units from the probe tip
  for (int i = 0; i < 3; i++)
    p2[i] = p1[i] + k*dir[i];

  //Calculate intersection point between probe and polydata and save point in intPoint
  double t = 0.0;
  double pcoords[3] = {0, 0, 0};
  int subId = 0;
  vtkIdType cellid = -1;
  vtkGenericCell *cell = vtkGenericCell::New();
  if (this->cellLocator->IntersectWithLine(p1, p2, 0.001, t, intPoint, pcoords, subId, cellid, cell) == 0)
    {
    //No intersection made
    std::cerr << "No intersection with the model" << std::endl;
    return snode;
    }

  //Print out intersection point
  std::cerr << "Intersection Coordinates: " << intPoint[0] << " " << intPoint[1] << " " << intPoint[2] << std::endl;


  //-------------------------------------------------------------------------
  // Find other polydata points and pixels surrounding the intersection point
  //--------------------------------------------------------------------------
  vtkIdList* result = vtkIdList::New();
  this->pointLocator->FindPointsWithinRadius(this->probeDiam/2, intPoint, result);
  std::cerr << "Number of points: " << result->GetNumberOfIds () << std::endl;
  
  //Extract polydata from the model node
  vtkPolyData* polydata = mnode->GetPolyData();
  
  //We need Matrix from RAS to IJK, so we invert matrixIJKtoRAS
  vtkMatrix4x4* matrixRAStoIJK = vtkMatrix4x4::New();
  matrixIJKtoRAS->Invert(matrixIJKtoRAS, matrixRAStoIJK);
  
  //-----------------------------------------------------------------------------------
  //Convert intersection and surrounding points into ijk pixel and append scalar value
  //-----------------------------------------------------------------------------------
  //define point vectors
  double pthom[4];
  double pt[3];
  pthom[3] = 1;
  vtkIdType id;
  double ScalarValue;

  //Assign scalar to each point
  for (int i = 0; i < result->GetNumberOfIds(); i++)
    {
    //Convert each point into homogenous format
    id = result->GetId(i);
    polydata->GetPoint(id, pt);
    for (int j = 0; j < 3; j++)
      {
      pthom[j] = pt[j];
      }
    //Convert each point into ijk pixel
    matrixRAStoIJK->MultiplyPoint(pthom, pthom);
    //Print homogenous coordinate
    std::cerr << "Intersection Coordinates ijk: " << pthom[0] << " " << pthom[1] << " " << pthom[2] << std::endl;
    //Add the polydata scalar value to that pixel
    ScalarValue = polydata->GetPointData()->GetScalars()->GetTuple1(id);
    short* scalPointer = (short*) image->GetScalarPointer(pthom[0], pthom[1], pthom[2]);
    if (scalPointer)
      {
      //Only change scalar value at pixel if new value is higher
      if (ScalarValue > *(scalPointer))
        *(scalPointer) = (short)ScalarValue;
      }
    }
  //Update image
  image->Update();
  image->Modified();  
  
  /*//Calculate how many pixels need to be colored
  double sp[3];
  this->image->GetSpacing(sp);
  int numPix_i = this->probeDiam/sp[0];
  int numPix_j = this->probeDiam/sp[1];
  int numPix_k = this->probeDiam/sp[2];
  //Fill in the pixels
  if (scalPointer)
    {
    //Calculate intensity value to associate to pixel
    double scal = this->nGammaScalars->GetTuple1(this->nGammaScalars->GetNumberOfTuples()-1); 
    for (int i = -floor(numPix_i/2); i <= floor(numPix_i/2); i++)
      {
      for (int j = -floor(numPix_j/2); j <= floor(numPix_j/2); j++)
        {
        for (int k = -floor(numPix_k/2); k <= floor(numPix_k/2); k++)
          {
          int ext[6];
          image->GetExtent(ext);
          //Check to see that indices are within the extent of the image
          if ( (npt[0]+i <= ext[1]) && (npt[1]+j <= ext[3]) && (npt[2]+k <= ext[5])
             && (npt[0]+i >= ext[0]) && (npt[1]+j >= ext[2]) && (npt[2]+k >= ext[4]) )
            {
            short* scalPointer = (short*) image->GetScalarPointer(npt[0] + i, npt[1] + j, npt[2] + k);
            *(scalPointer) = (short)scal;
            }
          }
        }
      }
    image->Update();
    }*/
  
  //Clean up
  matrixIJKtoRAS->Delete();
  matrixRAStoIJK->Delete();
  cell->Delete();
  result->Delete();
  SensorAxisMatrix->Delete();
  ToolTipMatrix->Delete();
  ToolTipAxis->Delete();
  
  return snode; 
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkBetaProbeNavLogic::PaintModel(vtkMRMLModelNode* mnode, vtkMRMLHybridNavToolNode* tnode)
{
  if (!(mnode && tnode))
    {
    std::cerr << "The model node and/or were not valid" << std::endl;
    return mnode;
    }
 
  //-------------------------------------------------------------
  //Calculate intersection point between probe axis and polydata
  //-------------------------------------------------------------
  //Define point arrays
  double p1[3];
  double intPoint[3] = {0, 0, 0};

  //Obtain the position p1 of the tip from the Points array
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, p1);

  //Calculate probe axis using two points, the first the tip of the probe p1 and the second
  //point at k units away from the tip in the direction of the probe axis
  double dir[3];
  double p2[3];

  //Calculate probe axis from tool
  vtkMatrix4x4* SensorAxisMatrix = vtkMatrix4x4::New();
  tnode->GetParentTransformNode()->GetMatrixTransformToWorld(SensorAxisMatrix);
  vtkMatrix4x4* ToolTipMatrix = vtkMatrix4x4::New();
  tnode->GetCalibrationMatrix(ToolTipMatrix);
  vtkMatrix4x4* ToolTipAxis = vtkMatrix4x4::New();
  ToolTipAxis->Multiply4x4(SensorAxisMatrix, ToolTipMatrix, ToolTipAxis);
  //Direction vector of the probe axis
  dir[0] = ToolTipAxis->GetElement(0,2);
  dir[1] = ToolTipAxis->GetElement(1,2);
  dir[2] = ToolTipAxis->GetElement(2,2);
  double k = 50.0; //units from the probe tip
  for (int i = 0; i < 3; i++)
    p2[i] = p1[i] + k*dir[i];
  
  //Calculate intersection point between probe and polydata and save point in intPoint
  double t = 0.0;
  double pcoords[3] = {0, 0, 0};
  int subId = 0;
  vtkIdType cellid = -1;
  vtkGenericCell *cell = vtkGenericCell::New();
  if (this->cellLocator->IntersectWithLine(p1, p2, 0.001, t, intPoint, pcoords, subId, cellid, cell) == 0)
    {
    //No intersection made
    std::cerr << "No intersection with the model" << std::endl;
    return mnode;
    }

  //Print out intersection point
  std::cerr << "Intersection Coordinates: " << intPoint[0] << " " << intPoint[1] << " " << intPoint[2] << std::endl;

  //Send to GUI

  //---------------------------------------------------------------
  // Associate a scalar (radiation counts) to intersection points
  //---------------------------------------------------------------

  // Find other polydata points at ProbeDiam diameter of the intersection point
  vtkIdList* result = vtkIdList::New();
  this->pointLocator->FindPointsWithinRadius(this->probeDiam/2, intPoint, result);
  std::cerr << "Number of points: " << result->GetNumberOfIds () << std::endl;

  //Get radiation value and assume a uniform distribution
  double Scalars[6];
  //Extract the scalar values in each array
  Scalars[0] = this->SmoothScalars->GetTuple1(this->SmoothScalars->GetNumberOfTuples()-1);
  Scalars[1] = this->BetaScalars->GetTuple1(this->BetaScalars->GetNumberOfTuples()-1);
  Scalars[2] = this->GammaScalars->GetTuple1(this->GammaScalars->GetNumberOfTuples()-1);
  Scalars[3] = this->nSmoothScalars->GetTuple1(this->nSmoothScalars->GetNumberOfTuples()-1);
  Scalars[4] = this->nBetaScalars->GetTuple1(this->nBetaScalars->GetNumberOfTuples()-1);
  Scalars[5] = this->nGammaScalars->GetTuple1(this->nGammaScalars->GetNumberOfTuples()-1);
  /*//Get TimeStamp value
  std::string tstamp;
  this->TimeStamp->GetTupleValue(this->TimeStamp->GetNumberOfTuples()-1, tstamp);
  std::cerr << "Inserted TimeStamp value" << std::endl;*/

  //Extract polydata
  vtkPolyData* polydata = mnode->GetPolyData();
 
  //Assign scalar value to each array over all calculated points (overwrite current value on point)
  unsigned int id;
  for(unsigned int i = 0; i < result->GetNumberOfIds (); i++)
    {
    id = result->GetId(i);
    //Assign scalar values to all arrays
    polydata->GetPointData()->GetArray("SmoothCounts")->SetTuple1(id, Scalars[0]);
    polydata->GetPointData()->GetArray("BetaCounts")->SetTuple1(id, Scalars[1]);
    polydata->GetPointData()->GetArray("GammaCounts")->SetTuple1(id, Scalars[2]);
    polydata->GetPointData()->GetArray("NormSmoothCounts")->SetTuple1(id, Scalars[3]);
    polydata->GetPointData()->GetArray("NormBetaCounts")->SetTuple1(id, Scalars[4]);
    polydata->GetPointData()->GetArray("NormGammaCounts")->SetTuple1(id, Scalars[5]);
    /*//Assign Timestamp value
    vtkCharArray* ts = vtkCharArray::SafeDownCast(polydata->GetPointData()->GetArray("TimeStamp"));
    if (ts)
      {
      std::cerr << "Inserted TimeStamp value into array" << std::endl;
      ts->SetTupleValue(id, tstamp);
      }*/
    }
  
  //Update the model elements
  polydata->Update();
  polydata->Modified();
  mnode->Modified();
  
  //Clean up and return
  result->Delete();
  cell->Delete();
  SensorAxisMatrix->Delete();
  ToolTipMatrix->Delete();
  ToolTipAxis->Delete();
  return mnode;
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkBetaProbeNavLogic::PaintModelGaussian(vtkMRMLModelNode* mnode, vtkMRMLHybridNavToolNode* tnode)
{
  if (!(mnode && tnode))
    {
    std::cerr << "The model node and/or were not valid" << std::endl;
    return mnode;
    }
  
  //-------------------------------------------------------------
  //Calculate intersection point between probe axis and polydata
  //-------------------------------------------------------------
  //Define point arrays
  double p1[3];
  double intPoint[3] = {0, 0, 0};

  //Obtain the position p1 of the tip from the Points array
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, p1);

  //Calculate probe axis using two points, the first the tip of the probe p1 and the second
  //point at k units away from the tip in the direction of the probe axis
  double dir[3];
  double p2[3];

  //Calculate probe axis from tool
  vtkMatrix4x4* SensorAxisMatrix = vtkMatrix4x4::New();
  tnode->GetParentTransformNode()->GetMatrixTransformToWorld(SensorAxisMatrix);
  vtkMatrix4x4* ToolTipMatrix = vtkMatrix4x4::New();
  tnode->GetCalibrationMatrix(ToolTipMatrix);
  vtkMatrix4x4* ToolTipAxis = vtkMatrix4x4::New();
  ToolTipAxis->Multiply4x4(SensorAxisMatrix, ToolTipMatrix, ToolTipAxis);
  //Direction vector of the probe axis
  dir[0] = ToolTipAxis->GetElement(0,2);
  dir[1] = ToolTipAxis->GetElement(1,2);
  dir[2] = ToolTipAxis->GetElement(2,2);
  double k = 50.0; //units from the probe tip
  for (int i = 0; i < 3; i++)
    p2[i] = p1[i] + k*dir[i];
  
  //Calculate intersection point between probe and polydata and save point in intPoint
  double t = 0.0;
  double pcoords[3] = {0, 0, 0};
  int subId = 0;
  vtkIdType cellid = -1;
  vtkGenericCell *cell = vtkGenericCell::New();
  if (this->cellLocator->IntersectWithLine(p1, p2, 0.001, t, intPoint, pcoords, subId, cellid, cell) == 0)
    {
    //No intersection made
    std::cerr << "No intersection with the model" << std::endl;
    return mnode;
    }

  //Print out intersection point
  std::cerr << "Intersection Coordinates: " << intPoint[0] << " " << intPoint[1] << " " << intPoint[2] << std::endl;

  //----------------------------------------------------------------
  // Check to see if the measured counts are useful
  //----------------------------------------------------------------
  //Measured Counts
  double cts;
  switch (this->ActiveDataType)
    {
    case 0:
      cts = this->nSmoothScalars->GetTuple1(this->nSmoothScalars->GetNumberOfTuples()-1);
      break;
    case 1:
      cts = this->nBetaScalars->GetTuple1(this->nBetaScalars->GetNumberOfTuples()-1);
      break;
    default:
      cts = this->nGammaScalars->GetTuple1(this->nGammaScalars->GetNumberOfTuples()-1);
    }
  std::cerr << "Count values: " << cts << std::endl;

  //-------------------------------------------------
  // Create a Gaussian Splat onto the PolyData
  //-------------------------------------------------
  //The Gaussian splat will affect all points located at two standard deviations from the splat point
  //Calculate the standard deviation of the Gaussian which depends on distance h of probe from splat point
  double h = sqrt(pow(intPoint[0] - p1[0],2) + pow(intPoint[1] - p1[1],2) + pow(intPoint[2] - p1[2],2));
  //Calculate the standard deviation from the probe model
  double std = 2.897 + 0.4348*h;
  
  //Make the upper limit of the std, the radius of the probe diameter
  if (std > (this->probeDiam/2))
    {
    std = (this->probeDiam/2);
    }

  // Find other polydata points surrounding the intersection point
  vtkIdList* result = vtkIdList::New();
  this->pointLocator->FindPointsWithinRadius(std, intPoint, result);
  std::cerr << "Number of points: " << result->GetNumberOfIds () << std::endl;
  vtkPolyData* polydata = mnode->GetPolyData();
  
  //-------------------------------------------------
  // Associate a scalar (radiation counts) to all these points using a Gaussian
  //-------------------------------------------------
  //Gaussian function f(x)= Counts * exp( (x-x0)/2std^2) )
  //where x0 is the intersetion point and std the standard deviation
  //These values depend on the probe characteristics
  double sc;
  for(unsigned int i = 0; i < result->GetNumberOfIds (); i++)
    {
    unsigned int id = result->GetId(i);
    //Calculate Gaussian contribution to each point
    double pnt[3];
    polydata->GetPoint(id, pnt);
    double rnorm = pow(pnt[0] - intPoint[0],2) + pow(pnt[1] - intPoint[1],2) + pow(pnt[2] - intPoint[2],2); 
    double fx = cts*exp(-rnorm/(2*pow(std,2)));
    std::cerr << "Gaussian contribution: " << fx << std::endl;
    //Check to see if current value is greater than the stored value at this point
    sc = polydata->GetPointData()->GetScalars()->GetTuple1(id);
    if (fx > sc)
      polydata->GetPointData()->GetScalars()->SetTuple1(id, fx);
    }

  //Update the model elements  
  polydata->Update();
  polydata->Modified();
  mnode->Modified();
  
  //Clean up
  result->Delete();
  cell->Delete();
  SensorAxisMatrix->Delete();
  ToolTipMatrix->Delete();
  ToolTipAxis->Delete();
  return mnode;
}

//-------------------------------------------------------------------------------------------------
double* vtkBetaProbeNavLogic::CalculateIntersectionPoint(double p1[3], vtkMRMLLinearTransformNode* tnode, double* intPoint)
{
  //Calculate probe axis using two points, the first the tip of the probe and the second
  //point at k units away from the tip
  double dir[3];
  double p2[3];
  vtkMatrix4x4* mat = vtkMatrix4x4::New();
  tnode->GetMatrixTransformToWorld(mat);
  dir[0] = mat->GetElement(0,2);
  dir[1] = mat->GetElement(1,2);
  dir[2] = mat->GetElement(2,2);  //direction of the probe axis
  double k = 50.0; //units from the probe tip
  for (int i = 0; i < 3; i++)
    p2[i] = p1[i] + k*dir[i];
  
  //Calculate intersection point between probe and polydata and save point in intPoint
  double t = 0.0;
  //double iPoint[3] = {0, 0, 0};  //intersection point
  double pcoords[3] = {0, 0, 0};
  int subId = 0;
  vtkIdType cellid = -1;
  vtkGenericCell *cell = vtkGenericCell::New();
  if (this->cellLocator->IntersectWithLine(p1, p2, 0.001, t, intPoint, pcoords, subId, cellid, cell) == 0)
    {
    //No intersection made
    std::cerr << "No intersection with the model" << std::endl;
    return NULL;
    }
  //Print out intersection point
  std::cerr << "Coordinates: " << intPoint[0] << " " << intPoint[1] << " " << intPoint[2] << std::endl;
 
  //clean up and return
  mat->Delete();
  cell->Delete();
  return intPoint;
}

//-------------------------------------------------------------------------------------------------
vtkIdList* vtkBetaProbeNavLogic::CalculateSurroundingPoints(double p1[3], double radius, vtkIdList* result)
{
  //Extrate points within a certain radius of the intersection point
  std::cerr << p1[0] << ", " << p1[1] << ", " << p1[2] << std::endl;
  this->pointLocator->FindPointsWithinRadius(radius, p1, result);
  std::cerr << "Number of points: " << result->GetNumberOfIds () << std::endl;
  return result;
}

//-------------------------------------------------------------------------------------------------
double vtkBetaProbeNavLogic::CorrectionFactorHeight(double h)
{
  //Correction factor defined from probe model
  double cf;
  /*if (h >= 1)
    cf = exp(0.1949*(h-1));
  else
    cf = 1;*/
  cf = 1;
  return cf;
}
