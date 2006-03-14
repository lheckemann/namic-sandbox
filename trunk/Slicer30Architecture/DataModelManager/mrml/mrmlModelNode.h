/*=auto=========================================================================
  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.
  Program:   3D Slicer
  Module:    $RCSfile: ModelNode.h,v $
  Date:      $Date: 2006/03/03 22:26:39 $
  Version:   $Revision: 1.4 $
=========================================================================auto=*/

// .NAME ModelNode - MRML node to represent a 3D surface model.
// .SECTION Description
// Model nodes describe polygonal data.  They indicate where the model is 
// stored on disk, and how to render it (color, opacity, etc).  Models 
// are assumed to have been constructed with the orientation and voxel 
// dimensions of the original segmented volume.

#ifndef __mrmlModelNode_h
#define __mrmlModelNode_h

#include "mrmlNode.h"

namespace mrml
{
class Model;
class MRMLCommon_EXPORT ModelNode : public Node
{
public:
  typedef ModelNode Self;
  typedef Node Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  // Description:
  mrmlTypeMacro(Node, Object);

  //--------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(std::ostream& of, Indent ind);

  // Description:
  // Copy the node's attributes to this object
  void Copy(ModelNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Model";};

  // Description:
  // Name of the model's color, which is defined by a Color node in a MRML file
  mrmlSetStringMacro(Color);
  mrmlGetStringMacro(Color);

  // Description:
  // Opacity of the surface expressed as a number from 0 to 1
  mrmlSetMacro(Opacity, float);
  mrmlGetMacro(Opacity, float);

  // Description:
  // Indicates if the surface is visible
  mrmlBooleanMacro(Visibility, int);
  mrmlGetMacro(Visibility, int);
  mrmlSetMacro(Visibility, int);

  // Description:
  // Specifies whether to clip the surface with the slice planes
  mrmlBooleanMacro(Clipping, int);
  mrmlGetMacro(Clipping, int);
  mrmlSetMacro(Clipping, int);

  // Description:
  // Indicates whether to cull (not render) the backface of the surface
  mrmlBooleanMacro(BackfaceCulling, int);
  mrmlGetMacro(BackfaceCulling, int);
  mrmlSetMacro(BackfaceCulling, int);

  // Description:
  // Indicates whether to render the scalar value associated with each polygon vertex
  mrmlBooleanMacro(ScalarVisibility, int);
  mrmlGetMacro(ScalarVisibility, int);
  mrmlSetMacro(ScalarVisibility, int);

  // Description:
  // Indicates whether to render the vector value associated with each polygon vertex
  mrmlBooleanMacro(VectorVisibility, int);
  mrmlGetMacro(VectorVisibility, int);
  mrmlSetMacro(VectorVisibility, int);

  // Description:
  // Indicates whether to render the tensor value associated with each polygon vertex
  mrmlBooleanMacro(TensorVisibility, int);
  mrmlGetMacro(TensorVisibility, int);
  mrmlSetMacro(TensorVisibility, int);

  // Description:
  // Range of scalar values to render rather than the single color designated by colorName
  mrmlSetVector2Macro(ScalarRange, double);
  mrmlGetVector2Macro(ScalarRange, double);


  // Description:
  // Numerical ID of the color lookup table to use for rendering the overlay
  // for this model
  mrmlGetMacro(LUTName,int);
  mrmlSetMacro(LUTName,int);

  // Used to be the vtkPolyData accessors:
  mrmlGetObjectMacro(PolyData, Model);
  virtual void SetPolyData(Model*);

protected:
  ModelNode();
  ~ModelNode();

  /** Print the object information in a stream. */
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  // Data
  Model *PolyData;

  // Strings
  char *Color;
  int LUTName;

  // Numbers
  float Opacity;

  // Booleans
  int Visibility;
  int Clipping;
  int BackfaceCulling;
  int ScalarVisibility;
  int VectorVisibility;
  int TensorVisibility;

  // Arrays
  double ScalarRange[2];

private:
  ModelNode(const ModelNode&);
  void operator=(const ModelNode&);
};
} // end namespace mrml
#endif
