/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageBrainThumbSegmentation.cxx,v $
  Date:      $Date: 2006/01/06 17:56:39 $
  Version:   $Revision: 1.10 $

=========================================================================auto=*/
#include "vtkImageBrainThumbSegmentation.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPriorityQueue.h"
#include "vtkVersion.h"
#include <math.h>
#include <stdlib.h>


//----------------------------------------------------------------------------
vtkImageBrainThumbSegmentation* vtkImageBrainThumbSegmentation::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageBrainThumbSegmentation");
  if(ret)
    {
    return (vtkImageBrainThumbSegmentation*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageBrainThumbSegmentation;
}


//----------------------------------------------------------------------------
// Constructor sets default values
vtkImageBrainThumbSegmentation::vtkImageBrainThumbSegmentation()
{
  //printf("initialize");
  this->SourceID = 0;
  this->SinkID = 0;
  this->NumberOfInputPoints = 0;
  this->PathPointer = -1;
  this->StopWhenEndReached = 1;
 
  this->ShortestPathIdList = NULL;
  this->Parent        = NULL;
  this->Visited          = NULL;
  this->PQ = NULL;
  this->BoundaryScalars = NULL;

  this->UseInverseDistance = 0;
  this->UseInverseSquaredDistance = 0;
  this->UseInverseExponentialDistance = 1;
  this->UseSquaredDistance = 0;
}


vtkImageBrainThumbSegmentation::~vtkImageBrainThumbSegmentation()
{

  //printf("in delete");
  if (this->ShortestPathIdList)
    this->ShortestPathIdList->Delete();
  if (this->Parent)
    this->Parent->Delete();
  if(this->BoundaryScalars) 
    this->BoundaryScalars->Delete();
}

void vtkImageBrainThumbSegmentation::init(vtkImageData *inData)
{
  if (this->ShortestPathIdList)
    this->ShortestPathIdList->Delete();
  if (this->Parent)
    this->Parent->Delete();
  if (this->Visited)
    this->Visited->Delete();
  if (this->PQ)
    this->PQ->Delete();

  this->ShortestPathIdList = vtkIdList::New();
  this->Parent             = vtkIntArray::New();
  this->Visited            = vtkIntArray::New();
  this->PQ                 = vtkPriorityQueue::New();
  
  int numPoints = inData->GetNumberOfPoints();

  this->Parent->SetNumberOfComponents(1);
  this->Parent->SetNumberOfTuples(numPoints);
  this->Visited->SetNumberOfComponents(1);
  this->Visited->SetNumberOfTuples(numPoints);
}

void vtkImageBrainThumbSegmentation::RunBrainThumbSegmentation(vtkDataArray *scalars,int startv, int endv)
{
  
  int i, u, v;
  
  InitSingleSource(startv);
  
  this->Visited->SetValue(startv, 1);

  int initialSize = PQ->GetNumberOfItems();
  int size = initialSize;
  int stop = 0;

  while ((PQ->GetNumberOfItems() > 0) && !stop)
    {

    this->UpdateProgress (0.5 + (float) (initialSize - size) / ((float) 2 * initialSize));
      
    if(PQ->GetNumberOfItems() != size) {
    //printf("*************** PROBLEM IN PQ SIZE ******************");
    }
    // printf("visiting node %i \n",u);
    // u is now in s since the shortest path to u is determined
    // remove from the front set >1
    vtkFloatingPointType u_weight;
#if (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION == 0)      
    u = PQ->Pop(u_weight,0);
#else
    u = PQ->Pop(0, u_weight);
#endif
    //printf("looking at id %i that has priority %f",u,u_weight);
     
    this->Visited->SetValue(u, 1);
      
    if (u == endv && StopWhenEndReached)
      stop = 1;
      
    // Update all vertices v neighbors to u
    // find the neighbors of u
    vtkIdList *list = vtkIdList::New();
    this->FindNeighbors(list,u,scalars);
      
    //printf("the list has %i ids",list->GetNumberOfIds());
    for (i = 0; i < list->GetNumberOfIds(); i++) {
    
    v = list->GetId(i);
    
    // s is the set of vertices with determined shortest path...do not 
    // use them again
    if (this->Visited->GetValue(v) != 1)
      {
      // Only relax edges where the end is not in s and edge 
      // is in the front set
      float w = EdgeCost(scalars, u, v);
      float v_weight = this->PQ->GetPriority(v);
      //if(v == endv) 
      //printf("we have found endv %i, its weight is %f, neighbor is %i , weight is %f, edge weight is %f",v,v_weight,u,u_weight,w);
        
      // Relax step
      if (v_weight > (u_weight + w))
        {
        this->PQ->DeleteId(v);
        this->PQ->Insert((u_weight + w),v);
        this->Parent->SetValue(v, u);
        //printf("setting parent of %i to be %i",v,u);
        }
      }
    }
      
    // now delete the array
    list->Delete();
    size--;
    }
  this->PQ->Delete();
  this->Visited->Delete();
}

void vtkImageBrainThumbSegmentation::InitSingleSource(int startv)
{
  for (int v = 0; v < this->GetNumberOfInputPoints(); v++)
    {
    // d will be updated with first visit of vertex
    //this->CumulativeWeightFromSource->SetValue(v, -1);
    this->Parent->SetValue(v, -1);
    this->Visited->SetValue(v, 0);
    }
  PQ->DeleteId(startv);
  // re-insert the source with priority 0
  PQ->Insert(0,startv);
  //printf("priority of startv %f",PQ->GetPriority(startv));
  //this->CumulativeWeightFromSource->SetValue(startv, 0);
}


void vtkImageBrainThumbSegmentation::FindNeighbors(vtkIdList *list,int id, vtkDataArray *scalars) {
  
  // find i, j, k for that node
  int *dim = GetInput()->GetDimensions();
  int numPts = dim[0] * dim[1] * dim[2];
  
  
  for(int vk = -1; vk<2; vk++) {
  for(int vj = -1; vj<2; vj++) {
  for(int vi = -1; vi<2; vi++) {         
    
  int tmpID = id + (vk * dim[1]*dim[0]) + (vj * dim[0]) + vi;
  // check we are in bounds (for volume faces)     
  if( tmpID >= 0 && tmpID < numPts && tmpID != 0) {
  float mask = scalars->GetTuple1(tmpID);
  // only add neighbor if it is in the graph
  if(mask > 0) {        
  list->InsertUniqueId(tmpID);
  }
  }
  }
  }
  }
}


// The edge cost function should be implemented as a callback function to
// allow more advanced weighting
float vtkImageBrainThumbSegmentation::EdgeCost(vtkDataArray *scalars, int u, int v)
{
  
  float w;
  // if it is a boundary voxel, give it a very high edge value to 
  // keep the path from going through it
  if(this->BoundaryScalars->GetTuple1(v) == 1) {
  w = 10000;
  } else {
  float dist2 = scalars->GetTuple1(v) ;
  float dist = sqrt(scalars->GetTuple1(v));
  if(this->UseInverseDistance)
    w = (1.0/dist);
  else if(this->UseInverseSquaredDistance)
    w = (1.0/(dist*dist));
  else if(this->UseInverseExponentialDistance)
    w = (1.0/exp(dist));
  else if(this->UseSquaredDistance)
    w = dist2;
  }
  return w;
}

void vtkImageBrainThumbSegmentation::BuildShortestPath(int start,int end)
{
  
  int v = end;
  while (v != start && v > 0)
    {
    this->ShortestPathIdList->InsertNextId(v);
    v = this->Parent->GetValue(v);
    }
  this->ShortestPathIdList->InsertNextId(v);
  
}

// ITERATOR PART 

void vtkImageBrainThumbSegmentation::InitTraversePath(){
  this->PathPointer = -1;
}

int vtkImageBrainThumbSegmentation::GetNumberOfPathNodes(){
  return this->ShortestPathIdList->GetNumberOfIds();
}

int vtkImageBrainThumbSegmentation::GetNextPathNode(){
  this->PathPointer = this->PathPointer + 1;
  
  if(this->PathPointer < this->GetNumberOfPathNodes()) {
  //printf("this->GetPathNode(this->PathPointer) %i",this->GetPathNode(this->PathPointer));
  return this->ShortestPathIdList->GetId(this->PathPointer);
  } else {
  return -1;
  }
}

// find closest scalar to id that is non-zero
int vtkImageBrainThumbSegmentation::findClosestPointInGraph(vtkDataArray *scalars,int id,int dim0,int dim1, int dim2) {

  
  int kFactor = dim0 * dim1;
  int jFactor = dim0;
  
  int numPoints = kFactor * dim2;
  vtkIdList* Q = vtkIdList::New();
  Q->InsertNextId(id);
  
  int pointer = 0;
  int foundID = -1;
  
  while(Q->GetNumberOfIds() != 0) {
    
  int current = Q->GetId(pointer);
  pointer = pointer + 1;
  //printf("we are looking at id %i \n",current);
    
  // check to see if we found something in the graph
  if(scalars->GetTuple1(current) >0) {
  //printf("before return");
  return current;
  } else {
  // set it to -1 to show that we already looked at it
  scalars->SetTuple1(current,-1);
  // put the neighbors on the stack
  // top
  if (current + kFactor <numPoints) {
  if(scalars->GetTuple1(current + kFactor) != -1){
  //printf("expand k+1 %i",current + kFactor);
  Q->InsertNextId(current+kFactor);
  }
  }
  // bottom
  if (current - kFactor >= 0){
  if(scalars->GetTuple1(current - kFactor) != -1){
  //printf("expand k-1 %i", current - kFactor);
  Q->InsertNextId(current-kFactor);
  }
  }
  // front
  if (current + jFactor < numPoints) {
  if(scalars->GetTuple1(current + jFactor) != -1){
  //printf("expand j+1 %i",current + jFactor);
  Q->InsertNextId(current + jFactor);
  }
  }
  // back
  if (current - jFactor >= 0) {
  if(scalars->GetTuple1(current - jFactor) != -1){
  //printf("expand j-1 %i",current - jFactor);
  Q->InsertNextId(current - jFactor);    
  }
  }
  // left
  if (current+1 <numPoints){
  if(scalars->GetTuple1(current + 1) != -1){
  //printf("expand i+1 %i",current+1);
  Q->InsertNextId(current + 1);    
  }
  }
  // right
  if (current -1 >= 0) {
  if(scalars->GetTuple1(current - 1) != -1){
  //printf("expand i-1 %i",current - 1);
  Q->InsertNextId(current - 1);    
  }
  }
  }
  }
  Q->Delete();
  return foundID;
}




//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageBrainThumbSegmentationExecute(vtkImageBrainThumbSegmentation *self,
                                                  vtkImageData *inData, T *inPtr,
                                                  vtkImageData *outData, int *outPtr)
{

  //printf("*************** vtkImageBrainThumbSegmentationExecute() **************");
  self->init(inData);
  //printf("*************** after init ****************");
  
  int *dim = inData->GetDimensions();
  vtkDataArray *scalars = inData->GetPointData()->GetScalars();
  
  // find closest point in graph to source and sink if their value is not 0
  //printf("source ID %i value is %f",self->GetSourceID(),scalars->GetScalar(self->GetSourceID()));
  if(scalars->GetTuple1(self->GetSourceID()) == 0) {
    
  vtkFloatArray *copyScalars = vtkFloatArray::New();
  copyScalars->DeepCopy(inData->GetPointData()->GetScalars());
    
  self->SetSourceID(self->findClosestPointInGraph(copyScalars,self->GetSourceID(),dim[0],dim[1],dim[2]));
    
  copyScalars->Delete();
  //printf("NEW source ID %i value is %f",self->GetSourceID(),scalars->GetScalar(self->GetSourceID()));

  }

  //printf("sink ID %i value is %f",self->GetSinkID(),scalars->GetScalar(self->GetSinkID()));
  if(scalars->GetTuple1(self->GetSinkID()) == 0) {
  vtkFloatArray *copyScalars2 = vtkFloatArray::New();
  copyScalars2->DeepCopy(inData->GetPointData()->GetScalars());
  self->SetSinkID(self->findClosestPointInGraph(copyScalars2,self->GetSinkID(),dim[0],dim[1],dim[2]));
  copyScalars2->Delete();
    
  //printf("NEW sink ID %i value is %f",self->GetSinkID(),scalars->GetScalar(self->GetSinkID()));
  }
      
  //printf("************* before run dijkstra ");
  self->RunBrainThumbSegmentation(scalars,self->GetSourceID(),self->GetSinkID());
  //printf("************* before build shortest path");
  self->BuildShortestPath(self->GetSourceID(),self->GetSinkID());
  //printf("************** DONE ***************");
}


//----------------------------------------------------------------------------
// This method is passed a input and output Data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the Datas data types.
// -- sp 2002-09-05 updated for vtk4
void vtkImageBrainThumbSegmentation::ExecuteData(vtkDataObject *)
{
  void *inPtr;
  void *outPtr;
  
  vtkImageData *inData = this->GetInput();
  vtkImageData *outData = this->GetOutput();
  outData->SetExtent(outData->GetWholeExtent());
  outData->AllocateScalars();

  int inExt[6], outExt[6];

  outData->GetExtent(outExt);
  this->ComputeInputUpdateExtent(inExt, outExt);
  inPtr = inData->GetScalarPointerForExtent(inExt);
  outPtr = outData->GetScalarPointerForExtent(outExt);
  

  // Components turned into x, y and z
  if (this->GetInput()->GetNumberOfScalarComponents() > 3)
    {
    vtkErrorMacro("This filter can handle upto 3 components");
    return;
    }
  
  
  switch (inData->GetScalarType())
    {
    vtkTemplateMacro5(vtkImageBrainThumbSegmentationExecute, this, 
                      inData, (VTK_TT *)(inPtr), 
                      outData, (int *)(outPtr));
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
}


void vtkImageBrainThumbSegmentation::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

  os << indent << "Source ID: ( "
     << this->GetSourceID() << " )\n";

  os << indent << "Sink ID: ( "
     << this->GetSinkID() << " )\n";
}
