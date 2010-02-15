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
vtkMRMLScalarVolumeNode* vtkBetaProbeNavLogic::PaintImage(vtkMRMLNode* inode, vtkMRMLScalarVolumeNode* snode)
{
  if (!inode)
    {
    return snode;
    }
  
  //Get current image from node
  vtkMRMLScalarVolumeNode* vnode = vtkMRMLScalarVolumeNode::SafeDownCast(inode);
  
  // Check to see if image has been created
  if (!image)
    {
    std::cerr << "Creating a new image" << std::endl;
    //Create image node
    this->image = vtkImageData::New();
    
    //Get image characteristics
    vtkImageData* im = vnode->GetImageData();
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
    snode->SetOrigin(vnode->GetOrigin());
    snode->SetAndObserveImageData(this->image);
    }
  
  //Extract IJK to RAS Matrix
  vtkMatrix4x4* mat = vtkMatrix4x4::New();
  vnode->GetIJKToRASMatrix(mat);
  
  //Set this matrix to snode
  snode->SetIJKToRASMatrix(mat);
  
  //Convert last point introduced into Points Array and convert to ijk
  double pt[3];
  this->Points->GetPoint(this->Points->GetNumberOfPoints()-1, pt);
  double ptt[4];   //Make pt[3] into homogenous form
  for (int i = 0; i < 3; i++)
    {
    ptt[i] = pt[i];
    std::cerr << ptt[i] << std::endl;
    }
  ptt[3] = 1;
  
  //We need Matrix from RAS to IJK, so we invert matrix
  mat->Invert(mat, mat);
  double npt[4];     //calculate new point in IJK
  mat->MultiplyPoint(ptt, npt);
 
  for (int i = 0; i < 3; i++)
    {
    std::cerr << npt[i] << std::endl;
    }
  
  //With the ijk point we colorate the pixel of the new image
  //Get Scalar Pointer of image
  /*short* scalPointer = (short*) image->GetScalarPointer(npt[0], npt[1], npt[2]);
  if (scalPointer)
    {
    double scal = this->nGammaScalars->GetTuple1(this->nGammaScalars->GetNumberOfTuples()-1);
    std::cerr << scal << std::endl;
    std::cerr << *(scalPointer) << std::endl;
    //memset(scalPointer, (short)scal, sizeof(short));
    *(scalPointer) = (short)scal;
    std::cerr << *(scalPointer) << std::endl;
    image->Update();
    }*/
    
  //Project value of radiation over a large amount of pixels
  //With the ijk point we colorate pixels on the new image
  //Get Scalar Pointer of image
  short* scalPointer = (short*) image->GetScalarPointer(npt[0], npt[1], npt[2]);
  //Calculate how many pixels need to be colored
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
          short* scalPointer = (short*) image->GetScalarPointer(npt[0] + i, npt[1] + j, npt[2] + k);
          *(scalPointer) = (short)scal;
          }
        }
      }
    image->Update();
    }
  
  //Clean up
  mat->Delete();
  
  return snode; 
}

