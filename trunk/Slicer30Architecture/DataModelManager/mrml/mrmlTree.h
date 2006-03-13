/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: Tree.h,v $
  Date:      $Date: 2006/02/14 20:40:15 $
  Version:   $Revision: 1.19 $

=========================================================================auto=*/
// .NAME Tree - a list of actors
// .SECTION Description
// Tree represents and provides methods to manipulate a list of
// MRML objects. The list is core and duplicate
// entries are not prevented.
//
// SECTION Transforms
//  The question is: in what order are transforms applied?
//  Twice in Tree.cxx, we set tran->PreMultiply();
//  The result is that if we traverse a MRML tree, and we
//  run into transforms M1 and then M2 as we traverse down
//  the tree, the resulting transform is M = M1*M2.
//
//  Just to be clear, given the following structure:
//  Transform
//    Matrix: M1
//    Transform
//      Matrix: M2
//      Matrix: M3
//      Volume: V1
//      Model:  Model1
//    End Transform
//  End Transform
//
// Then the effective transform applied to V1 is  M1*M2*M3.
// Note that the Transform is the RAS to World transform for the volume V1.
// It is the same for the Model, Model1. Though, the RasToWld matrix is
// only set explicitly for models and volumes.
//
// .SECTION see also
// vtkMrmlNode vtkCollection 

#ifndef __mrmlTree_h
#define __mrmlTree_h

#include "mrmlObject.h"
#include "mrmlObjectFactory.h" // ??

//#include "mrmlVolumeNode.h"
//#include "mrmlModelNode.h"
//#include "mrmlTransformNode.h"
//#include "mrmlMatrixNode.h"
//#include "mrmlColorNode.h"

namespace mrml
{

class TreeInternals;
class Node;
class VolumeNode;
class MRMLCommon_EXPORT Tree : public Object
{
public:
  typedef Tree Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for defining the name of the class */
  mrmlTypeMacro(Self,Superclass);

  /** Method for creation through the object factory */
  mrmlNewMacro(Self);

  // Description:
  void Write(const char *filename);

  // Description:
  // Add a path to the list.
  void AddNode(Node *n);

  // Description:
  // Remove a path from the list.
  void RemoveNode(Node *n);

  // Description:
  // Determine whether a particular node is present. Returns its position
  // in the list.
  unsigned long IsNodePresent(Node *n);

  // Description:
  // Get the next path in the list.
  Node *GetNextNode();

  VolumeNode *GetNextVolume();
  //ModelNode *GetNextModel();
  //TransformNode *GetNextTransform();
  //MatrixNode *GetNextMatrix();
  //ColorNode *GetNextColor();

  VolumeNode *InitVolumeTraversal(); 
  //ModelNode *InitModelTraversal();
  //TransformNode *InitTransformTraversal();
  //MatrixNode *InitMatrixTraversal();
  //ColorNode *InitColorTraversal();

  Node* GetNthNode(unsigned long n);
  VolumeNode *GetNthVolume(int n);
  //ModelNode *GetNthModel(int n);
  //TransformNode *GetNthTransform(int n);
  //MatrixNode *GetNthMatrix(int n);
  //ColorNode *GetNthColor(int n);

  unsigned long GetNumberOfVolumes();
  //int GetNumberOfModels() {
  //int GetNumberOfTransforms() {
  //int GetNumberOfMatrices() {
  //int GetNumberOfColors() {

  //void ComputeTransforms(); ??

  // Convenient functions
  void InsertAfterNode( Node *node, Node *newNode);
  void InsertBeforeNode( Node *node, Node *newNode);

  // Description:
  // Compute the Transform for one node
  // The second just gets the matrix
  // There is no check that the node actually exists. The function
  // sometimes crash without it.
  //void ComputeNodeTransform( Node *node, vtkTransform *xform );
  //void ComputeNodeTransform( Node *node, vtkMatrix4x4 *xform );

protected:
  Tree();
  ~Tree();

  /** Print the object information in a stream. */
  void PrintSelf(std::ostream& os, Indent indent) const;

  Node *InitTraversalByClass(const char *className);
  unsigned long GetNumberOfNodesByClass(const char *className);
  Node *GetNthNodeByClass(unsigned long n, const char *className);
  Node *GetNextNodeByClass(const char *className);

private:
  // Private implementation details.
  TreeInternals* Internal;

  Tree(const Tree&);
  void operator=(const Tree&);
};

} // namespace mrml

#endif
