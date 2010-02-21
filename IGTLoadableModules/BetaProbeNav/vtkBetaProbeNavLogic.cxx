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
#include "vtkMRMLUDPServerNode.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkAppendPolyData.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSphereSource.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkType.h"
#include "vtkGlyph3D.h"
#include "vtkImageData.h"

//#include "vtkKdTree.h"
#include "vtkPointLocator.h"
#include "vtkCellLocator.h"
#include "vtkGenericCell.h"

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

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::CollectData(vtkMRMLNode* tn, vtkMRMLNode* cn)
{
  std::cerr << "Collect Data started" << std::endl;
  
  //Pick up position data from TransformNode
  /*if (this->Points == NULL)
    {
    //Create objects
    Points = vtkPoints::New();
    }*/

  //Extract position from transform
  float pos[3];
  vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(tn);
  if (tnode)
    {
    pos[0] = tnode->GetMatrixTransformToParent()->GetElement(0, 3);
    pos[1] = tnode->GetMatrixTransformToParent()->GetElement(1, 3);
    pos[2] = tnode->GetMatrixTransformToParent()->GetElement(2, 3);
    //Add point to Points Array
    std::cerr << "About to add points to array" << std::endl;
    this->Points->InsertNextPoint(pos);
    std::cerr << "Added Points to Array" << std::endl;
    }
  
  //Extract counts from CountNode and append to Float Arrays
  vtkMRMLUDPServerNode* cnode = vtkMRMLUDPServerNode::SafeDownCast(cn);
  SmoothScalars->InsertNextTuple1(cnode->GetSmoothedCounts());
  BetaScalars->InsertNextTuple1(cnode->GetBetaCounts());
  GammaScalars->InsertNextTuple1(cnode->GetGammaCounts());
  
  //Normalize value of Scalars for Color mapping purposes
  //Smooth Scalars
  for (vtkIdType i = 0; i < (this->SmoothScalars->GetNumberOfTuples()); i++)
    {
    double nSC;
    nSC = this->SmoothScalars->GetTuple1(i);
    nSC = (int) (255*nSC/(this->maxRange)); 
    nSmoothScalars->InsertNextTuple1(nSC);
    }
  //BetaScalars
  for (vtkIdType i = 0; i < (this->BetaScalars->GetNumberOfTuples()); i++)
    {
    double nBC;
    nBC = this->BetaScalars->GetTuple1(i);
    nBC = (int) (255*nBC/(this->maxRange)); 
    nBetaScalars->InsertNextTuple1(nBC);
    }
  //GammaScalars
  for (vtkIdType i = 0; i < (this->GammaScalars->GetNumberOfTuples()); i++)
    {
    double nGC;
    nGC = this->GammaScalars->GetTuple1(i);
    nGC = (int) (255*nGC/(this->maxRange)); 
    nGammaScalars->InsertNextTuple1(nGC);
    }
}

//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkBetaProbeNavLogic::RepresentData(vtkMRMLModelNode* mnode)
{
  if (this->CountMap == NULL)
    {
    CountMap = vtkPolyData::New();
    }

  //Append points to PolyData
  CountMap->SetPoints(this->Points);
  CountMap->Update();

  //Append scalars to points  
  CountMap->GetPointData()->AddArray(this->nSmoothScalars);
  CountMap->GetPointData()->AddArray(this->nBetaScalars);
  CountMap->GetPointData()->AddArray(this->nGammaScalars);
  CountMap->GetPointData()->SetActiveScalars("Gamma");
  CountMap->Update();
  
  //--------------------------------
  //Represent Polydata using glyphs
  
  //Create display node and add to scene
  if ((this->dispNode == NULL) || (mnode->GetDisplayNode() == NULL))
    {
    std::cerr << "About to create display node display node" << std::endl;
    dispNode = vtkMRMLModelDisplayNode::New();
    GetMRMLScene()->SaveStateForUndo();
    GetMRMLScene()->AddNode(dispNode);
    dispNode->SetScene(this->GetMRMLScene());
  
    //Associate display node to model node
    mnode->SetScene(this->GetMRMLScene());
    mnode->SetAndObserveDisplayNodeID(dispNode->GetID());
    std::cerr << "Display node assigned" << std::endl;
    }
    
  // Each glyph is represented by a sphere
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(2.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  //Create Glyphs
  vtkGlyph3D* gly = vtkGlyph3D::New();
  gly->SetSourceConnection(sphere->GetOutputPort());
  gly->SetInput(CountMap);
  gly->SetColorModeToColorByScalar();
  //gly->ScalingOff();
  gly->SetScaleModeToDataScalingOff();
  //gly->SetScaleFactor(0.25);
  //gly->ClampingOn();
  //gly->SetRange(0,40);
  gly->Update();
  
  std::cerr << "Glyphs created" << std::endl;
  mnode->SetAndObservePolyData(gly->GetOutput());
  std::cerr << "dispNode Properties" << std::endl;
  //Properties of dataDisp node
  dispNode->SetActiveScalarName("Gamma");
  dispNode->SetScalarVisibility(1);
  //dataDisp->SetColorNode
  std::cerr << "dispNode Properties setting polydata" << std::endl;
  dispNode->SetPolyData(mnode->GetPolyData());
  std::cerr << "dispNode Properties setting visibility" << std::endl;
  dispNode->SetVisibility(1);
  
  //Clean up
  sphere->Delete();
  gly->Delete();
  std::cerr << "About to return model" << std::endl;
  return mnode;
}

//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkBetaProbeNavLogic::RepresentDataRT(vtkMRMLModelNode* mnode)
{
  if (this->CountMap == NULL)
    {
    CountMap = vtkPolyData::New();
    }
  
  //Access last point appended
  vtkPolyData* newPoly = vtkPolyData::New(); 
  double pt[3];
  vtkPoints* pts = vtkPoints::New(); 
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, pt);
  pts->InsertNextPoint(pt);
  newPoly->SetPoints(pts);

  //Access latest Scalars
  vtkFloatArray* nSC = vtkFloatArray::New();
  nSC->SetName("Smoothed");
  double ns = this->nSmoothScalars->GetTuple1(this->nSmoothScalars->GetNumberOfTuples()-1);
  nSC->InsertNextTuple1(ns);
  vtkFloatArray* nBC = vtkFloatArray::New();
  nSC->SetName("Beta");
  double nb = this->nBetaScalars->GetTuple1(this->nBetaScalars->GetNumberOfTuples()-1);
  nBC->InsertNextTuple1(nb);
  vtkFloatArray* nGC = vtkFloatArray::New();
  nSC->SetName("Gamma");
  double ng = this->nGammaScalars->GetTuple1(this->nGammaScalars->GetNumberOfTuples()-1);
  nGC->InsertNextTuple1(ng);

  //Add Scalars to latest point
  newPoly->GetPointData()->AddArray(nSC);
  newPoly->GetPointData()->AddArray(nBC);
  newPoly->GetPointData()->AddArray(nGC);
  newPoly->GetPointData()->SetActiveScalars("Gamma");
  
  
  //--------------------------------
  //Represent Polydata using glyphs
  
  //Create display node and add to scene
  if ((this->dispNode == NULL) || (mnode->GetDisplayNode() == NULL))
    {
    std::cerr << "About to create display node display node" << std::endl;
    dispNode = vtkMRMLModelDisplayNode::New();
    GetMRMLScene()->SaveStateForUndo();
    GetMRMLScene()->AddNode(dispNode);
    dispNode->SetScene(this->GetMRMLScene());
  
    //Associate display node to model node
    mnode->SetScene(this->GetMRMLScene());
    mnode->SetAndObserveDisplayNodeID(dispNode->GetID());
    std::cerr << "Display node assigned" << std::endl;
    mnode->SetAndObserveDisplayNodeID(dispNode->GetID());
    }
    
  // Each glyph is represented by a sphere
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(2.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  //Create Glyphs
  vtkGlyph3D* gly = vtkGlyph3D::New();
  gly->SetSourceConnection(sphere->GetOutputPort());
  gly->SetInput(newPoly);
  gly->SetColorModeToColorByScalar();
  gly->SetScaleModeToDataScalingOff();
  gly->Update();
  
  //Append to previous polydata
  vtkAppendPolyData* apd = vtkAppendPolyData::New();
  apd->AddInput(CountMap);
  apd->AddInput(gly->GetOutput());
  apd->Update();
  
  mnode->SetAndObservePolyData(apd->GetOutput());
  
  //Properties of dataDisp node
  dispNode->SetActiveScalarName("Gamma");
  dispNode->SetScalarVisibility(1);
  dispNode->SetPolyData(mnode->GetPolyData());

  //Save the current polydata until next time
  CountMap->DeepCopy(apd->GetOutput());

  //Clean up
  sphere->Delete();
  gly->Delete();
  apd->Delete();
  pts->Delete();
  newPoly->Delete();
  nSC->Delete();
  nGC->Delete();
  nBC->Delete();
  
  return mnode;
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
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkBetaProbeNavLogic::PaintImage(vtkMRMLScalarVolumeNode* inode, vtkMRMLScalarVolumeNode* snode, vtkMRMLModelNode* mnode, vtkMRMLLinearTransformNode* tnode)
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
  vtkMatrix4x4* mat = vtkMatrix4x4::New();
  inode->GetIJKToRASMatrix(mat);
  
  //Set this matrix to snode
  snode->SetIJKToRASMatrix(mat);
  
  //----------------------------------------------------------------
  //Calculate intersection point between probe axis and polydata
  //---------------------------------------------------------------
  double p1[3];
  double intPoint[3] = {0, 0, 0};
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, p1); //Point p1 is the probe tip
  //Calculate probe axis using two points, the first the tip of the probe and the second
  //point at k units away from the tip
  double dir[3];
  double p2[3];
  vtkMatrix4x4* mat1 = vtkMatrix4x4::New();
  tnode->GetMatrixTransformToWorld(mat1);
  dir[0] = mat1->GetElement(0,2);
  dir[1] = mat1->GetElement(1,2);
  dir[2] = mat1->GetElement(2,2);  //direction of the probe axis
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
  mat->Invert(mat, mat);
  
  //Convert surrounding points into ijk
  for (int i = 0; i < result->GetNumberOfIds(); i++)
    {
    //Convert each point into homogenous format
    double pthom[4];
    double pt[3];
    unsigned int id = result->GetId(i);
    polydata->GetPoint(id, pt);
    for (int j = 0; j < 4; j++)
      {
      if (j < 3)
        pthom[j] = pt[j];
      else
        pthom[j] = 1;
      }
    //Convert each point into ijk pixel
    mat->MultiplyPoint(pthom, pthom);
    //Print homogenous coordinate
    std::cerr << "Intersection Coordinates ijk: " << pthom[0] << " " << pthom[1] << " " << pthom[2] << std::endl;
    //Add the polydata scalar value to that pixel
    double sc = polydata->GetPointData()->GetScalars()->GetTuple1(id);
    short* scalPointer = (short*) image->GetScalarPointer(pthom[0], pthom[1], pthom[2]);
    if (scalPointer)
      *(scalPointer) = (short)sc;
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
  mat->Delete();
  mat1->Delete();
  cell->Delete();
  
  
  return snode; 
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
  
  /*if (!this->kdTree)
    {
    this->kdTree = vtkKdTree::New();
    }
  this->kdTree->SetDataSet(polydata);
  this->kdTree->BuildLocatorFromPoints(polydata->GetPoints());*/
  
  //Create Scalar Array, fill its values and add to polydata
  vtkFloatArray* scals = vtkFloatArray::New();
  scals->SetName("Counts");
  for (int i = 0; i < polydata->GetPoints()->GetNumberOfPoints(); i++)
    {
    scals->InsertNextTuple1(0.0);
    }
  polydata->GetPointData()->SetScalars(scals);

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
  scals->Delete();
  return mnode;
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkBetaProbeNavLogic::PaintModel(vtkMRMLModelNode* mnode, vtkMRMLLinearTransformNode* tnode)
{
  if (!(mnode && tnode))
    {
    std::cerr << "The model node and/or were not valid" << std::endl;
    return mnode;
    }
 
  /*//In order to paint the model, we need to find the point on the model that is closest to
  //the position of the probe in space
  double pt[3];
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, pt);
  double dist; //distance from probe position to closest point on model
  vtkIdType id;
  id = this->kdTree->FindClosestPoint(pt, dist);
  std::cerr << "Squared Distance: " << dist << "mm" << std::endl; 
  
  //Get the coordinates of the closest point
  double closestPoint[3];
  kdTree->GetDataSet()->GetPoint(id, closestPoint);
  std::cerr << "Coordinates: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;
  */
  
  //Calculate intersection point between probe axis and polydata
  double p1[3];
  double intPoint[3] = {0, 0, 0};
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, p1); //Point p1 is the probe tip
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
  /*intPoint = CalculateIntersectionPoint(p1, tnode, intPoint);
  if (!intPoint)
    {
    std::cerr << "No intersection found" << std::endl;
    return mnode;
    }*/
  //Print out intersection point
  std::cerr << "Intersection Coordinates: " << intPoint[0] << " " << intPoint[1] << " " << intPoint[2] << std::endl;

  // Find other polydata points surrounding the intersection point
  vtkIdList* result = vtkIdList::New();
  this->pointLocator->FindPointsWithinRadius(this->probeDiam/2, intPoint, result);
  std::cerr << "Number of points: " << result->GetNumberOfIds () << std::endl;
 
  //-------------------------------------------------
  // Associate a scalar (radiation counts) to all these points
  //-------------------------------------------------
  //Get radiation value and assume a uniform distribution
  double scal = this->nGammaScalars->GetTuple1(this->nGammaScalars->GetNumberOfTuples()-1); 
  std::cerr << "Scalar: " << scal << std::endl;
  vtkPolyData* polydata = mnode->GetPolyData();
 
 //Assign to all calculated points
  for(unsigned int i = 0; i < result->GetNumberOfIds (); i++)
    {
    unsigned int id = result->GetId(i);
    //Average with current scalar value on point
    //double sc = polydata->GetPointData()->GetScalars()->GetTuple1(id);
    polydata->GetPointData()->GetScalars()->SetTuple1(id, scal);
    }
  
  //Update the model elements
  polydata->Update();
  polydata->Modified();
  mnode->Modified();
  
  //Clean up and return
  result->Delete();
  mat->Delete();
  cell->Delete();
  return mnode;
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkBetaProbeNavLogic::PaintModelGaussian(vtkMRMLModelNode* mnode, vtkMRMLLinearTransformNode* tnode)
{
  if (!(mnode && tnode))
    {
    std::cerr << "The model node and/or were not valid" << std::endl;
    return mnode;
    }
  
  //Calculate interesection point between probe axis and polydata
  double p1[3];
  double intPoint[3] = {0, 0, 0};
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, p1); //Point p1 is the probe tip
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

  // Find other polydata points surrounding the intersection point
  vtkIdList* result = vtkIdList::New();
  this->pointLocator->FindPointsWithinRadius(this->probeDiam/2, intPoint, result);
  std::cerr << "Number of points: " << result->GetNumberOfIds () << std::endl;
  
  //-------------------------------------------------
  // Associate a scalar (radiation counts) to all these points using a Gaussian
  //-------------------------------------------------
  //Gaussian function f(x)= ScaleFactor * exp( ExponentFactor*((r/Radius)**2) )
  //These values depend on the probe characteristics
  double sfact, efact, rad;
  efact = -5.0;
  rad = 10.0;
  sfact = this->nGammaScalars->GetTuple1(this->nGammaScalars->GetNumberOfTuples()-1);
  vtkPolyData* polydata = mnode->GetPolyData();
 
  for(unsigned int i = 0; i < result->GetNumberOfIds (); i++)
    {
    unsigned int id = result->GetId(i);
    //Calculate Gaussian contribution to each point
    double pnt[3];
    polydata->GetPoint(id, pnt);
    double r[3];
    for (int j = 0; j < 3; j++)
      r[j] = intPoint[j] - pnt[j];
    std::cerr << "r vector: " << r[0] << ", " << r[1] << ", " << r[2]<< std::endl;
    double rnorm = sqrt(pow(r[0],2) + pow(r[1],2) + pow(r[2],2)); 
    double fx = sfact*exp(efact*pow(rnorm/rad,2));
    std::cerr << "Gaussian contribution: " << fx << std::endl;
    polydata->GetPointData()->GetScalars()->SetTuple1(id, fx);
    }

  //Update the model elements  
  polydata->Update();
  polydata->Modified();
  mnode->Modified();
  result->Delete();
  
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

