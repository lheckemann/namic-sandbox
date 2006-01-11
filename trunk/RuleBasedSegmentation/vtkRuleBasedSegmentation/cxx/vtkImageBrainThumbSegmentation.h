/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageBrainThumbSegmentation.h,v $
  Date:      $Date: 2006/01/06 17:56:40 $
  Version:   $Revision: 1.5 $

=========================================================================auto=*/

// .NAME vtkImageBrainThumbSegmentation - Segment brain "Thumbs" from imagery using the Fast Marching algorithm.
// .SECTION Description
// vtkImageBrainThumbSegmentation - 


#ifndef __vtkImageBrainThumbSegmentation_h
#define __vtkImageBrainThumbSegmentation_h

#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkPriorityQueue.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkImageBrainThumbSegmentation : public vtkImageToImageFilter
{
  public:
  static vtkImageBrainThumbSegmentation *New();
  vtkTypeMacro(vtkImageBrainThumbSegmentation,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Seed points and input data do not need anything here???

  // what is this?
  vtkGetObjectMacro(Parent,vtkIntArray);

  // TODO: add Set/Get Macro for SeedPointList

  // is this the input data??
  void init(vtkImageData *inData);
  

  // ALGORITHM
  
  void InitSingleSource(int startv);

  // Calculate shortest path from vertex startv to vertex endv
  void RunBrainThumbSegmentation(vtkDataArray *scalars,int source, int sink);

  // edge cost
  float EdgeCost(vtkDataArray *scalars, int u, int v);
  void BuildShortestPath(int start,int end);
  
  // VARIABLES
  
  // The vertex at the start of the shortest path
  int SourceID;
  
  // The vertex at the end of the shortest path
  int SinkID;  
  
  // boundary scalars
  vtkDataArray *BoundaryScalars;

  // the number of points if the graph
  int NumberOfInputPoints;

  int UseInverseDistance;
  int UseInverseSquaredDistance;
  int UseInverseExponentialDistance;
  int UseSquaredDistance;

  // list of all the graph nodes
  //vtkIntArray *GraphNodes;
  // Adjacency representation
  //vtkIdList **Neighbors;

  // the pq
  vtkPriorityQueue *PQ;

  // The vertex ids on the shortest path
  vtkIdList *ShortestPathIdList;
  // ShortestPathWeight current summed weight for path to vertex v
  //vtkFloatArray *CumulativeWeightFromSource;
  // pre(v) predecessor of v
  vtkIntArray *Parent;
  // Visited is the set of vertices with allready determined shortest path
  // Visited(v) == 1 means that vertex v is in s
  vtkIntArray *Visited;  
  // f is the set of vertices wich has not a shortest path yet but has a path
  // ie. the front set (f(v) == 1 means that vertex v is in f)
  //vtkIntArray *f;
 
  int PathPointer;
  //vtkImageData* CachedImage;
  int StopWhenEndReached;

protected:
 vtkImageBrainThumbSegmentation();
 ~vtkImageBrainThumbSegmentation();
 vtkImageBrainThumbSegmentation(const vtkImageBrainThumbSegmentation&) {};
 void operator=(const vtkImageBrainThumbSegmentation&) {};
 
 //void Execute(vtkImageData *inData, vtkImageData *outData);
 void ExecuteData(vtkDataObject *);
 void Execute() { this->vtkImageToImageFilter::Execute(); };
 void Execute(vtkImageData *outData) { this->vtkImageToImageFilter::Execute(outData); };
  
  
};

#endif
