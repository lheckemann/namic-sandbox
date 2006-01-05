/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlTree.h,v $
  Date:      $Date: 2005/12/20 22:44:32 $
  Version:   $Revision: 1.17.12.1 $

=========================================================================auto=*/
#ifndef __vtkMrmlTree_h
#define __vtkMrmlTree_h

#include "vtkCollection.h"
#include "vtkMrmlNode.h"
#include "vtkMrmlVolumeNode.h"
#include "vtkMrmlModelNode.h"
#include "vtkMrmlTransformNode.h"
#include "vtkMrmlMatrixNode.h"
#include "vtkMrmlColorNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlTree : public vtkCollection
{
public:
  static vtkMrmlTree *New();
  vtkTypeMacro(vtkMrmlTree,vtkCollection);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  void Write(char *filename);

  // Description:
  // Add a path to the list.
  void AddItem(vtkMrmlNode *n);

  // Description:
  // Remove a path from the list.
  void RemoveItem(vtkMrmlNode *n);

  // Description:
  // Determine whether a particular node is present. Returns its position
  // in the list.
  int IsItemPresent(vtkMrmlNode *n); 

  // Description:
  // Get the next path in the list.
  vtkMrmlNode *GetNextItem();

  vtkMrmlVolumeNode *GetNextVolume() ;
  vtkMrmlModelNode *GetNextModel() ;
  vtkMrmlTransformNode *GetNextTransform() ;
  vtkMrmlMatrixNode *GetNextMatrix() ;
  vtkMrmlColorNode *GetNextColor() ;

  vtkMrmlVolumeNode *InitVolumeTraversal() ;
  vtkMrmlModelNode *InitModelTraversal() ;
  vtkMrmlTransformNode *InitTransformTraversal() ;
  vtkMrmlMatrixNode *InitMatrixTraversal() ;
  vtkMrmlColorNode *InitColorTraversal() ;

  vtkMrmlNode* GetNthItem(int n);
  vtkMrmlVolumeNode *GetNthVolume(int n) ;
  vtkMrmlModelNode *GetNthModel(int n) ;
  vtkMrmlTransformNode *GetNthTransform(int n) ;
  vtkMrmlMatrixNode *GetNthMatrix(int n) ;
  vtkMrmlColorNode *GetNthColor(int n) ;

  int GetNumberOfVolumes() ;
  int GetNumberOfModels() ;
  int GetNumberOfTransforms() ;
  int GetNumberOfMatrices() ;
  int GetNumberOfColors() ;

  void ComputeTransforms();

  void InsertAfterItem( vtkMrmlNode *item, vtkMrmlNode *newItem);
  void InsertBeforeItem( vtkMrmlNode *item, vtkMrmlNode *newItem);

  // Description:
  // Compute the Transform for one node
  // The second just gets the matrix
  // There is no check that the node actually exists. The function
  // sometimes crash without it.
  void ComputeNodeTransform( vtkMrmlNode *node, vtkTransform *xform );
  void ComputeNodeTransform( vtkMrmlNode *node, vtkMatrix4x4 *xform );

  // Description:
  // The reader should set this code at the end of the update.
  // The error code contains a possible error that occured while
  // writing the file.
  vtkGetMacro(ErrorCode, unsigned long );
    
};

#endif
