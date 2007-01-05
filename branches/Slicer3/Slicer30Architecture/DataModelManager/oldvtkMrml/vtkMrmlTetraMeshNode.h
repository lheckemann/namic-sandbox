/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlTetraMeshNode.h,v $
  Date:      $Date: 2005/12/20 22:44:31 $
  Version:   $Revision: 1.8.16.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlTetraMeshNode_h
#define __vtkMrmlTetraMeshNode_h

#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlTetraMeshNode : public vtkMrmlNode
{
  public:
  static vtkMrmlTetraMeshNode *New();
  vtkTypeMacro(vtkMrmlTetraMeshNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  //--------------------------------------------------------------------------
  // Non-Header Information
  //--------------------------------------------------------------------------
  
  // Description:
  // TetraMesh ID
  vtkSetStringMacro(TetraMeshID);
  vtkGetStringMacro(TetraMeshID);
  
  // Description:
  // The FileName to save the data to
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  //--------------------------------------------------------------------------
  // Display Information
  //--------------------------------------------------------------------------
  
  // Description:
  // Numerical ID of the color lookup table to use for rendering the TetraMesh
  vtkSetStringMacro(LUTName);
  vtkGetStringMacro(LUTName);

  // Description:
  // Opacity of display, from 0 to 1
  vtkSetMacro(Opacity, float);
  vtkGetMacro(Opacity, float);

  // Description:
  // Opacity of display, from 0 to 1
  vtkSetMacro(Clipping, unsigned short);
  vtkGetMacro(Clipping, unsigned short);

  // Description:
  // Display these things?
  vtkSetMacro(DisplaySurfaces, unsigned short);
  vtkGetMacro(DisplaySurfaces, unsigned short);
  vtkSetMacro(DisplayEdges, unsigned short);
  vtkGetMacro(DisplayEdges, unsigned short);
  vtkSetMacro(DisplayNodes, unsigned short);
  vtkGetMacro(DisplayNodes, unsigned short);
  vtkSetMacro(DisplayScalars, unsigned short);
  vtkGetMacro(DisplayScalars, unsigned short);
  vtkSetMacro(DisplayVectors, unsigned short);
  vtkGetMacro(DisplayVectors, unsigned short);

  // Description
  // Surfaces Use Cell Data or Point Data?
  vtkSetMacro(SurfacesUseCellData, unsigned short);
  vtkGetMacro(SurfacesUseCellData, unsigned short);

  // Description
  // Surfaces Smooth Normals to look good? Or, don't bother?
  vtkSetMacro(SurfacesSmoothNormals, unsigned short);
  vtkGetMacro(SurfacesSmoothNormals, unsigned short);

  // Description:
  // Parameters to display
  vtkSetMacro(NodeScaling, float);
  vtkGetMacro(NodeScaling, float);
  vtkSetMacro(NodeSkip, unsigned short);
  vtkGetMacro(NodeSkip, unsigned short);

  vtkSetMacro(ScalarScaling, float);
  vtkGetMacro(ScalarScaling, float);
  vtkSetMacro(ScalarSkip, unsigned short);
  vtkGetMacro(ScalarSkip, unsigned short);

  vtkSetMacro(VectorScaling, float);
  vtkGetMacro(VectorScaling, float);
  vtkSetMacro(VectorSkip, unsigned short);
  vtkGetMacro(VectorSkip, unsigned short);

};

#endif

