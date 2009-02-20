/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCurveAnalysisNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLCurveAnalysisNode_h
#define __vtkMRMLCurveAnalysisNode_h

#include "vtkFourDAnalysisWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_FourDAnalysis_EXPORT vtkMRMLCurveAnalysisNode : public vtkMRMLNode
{
  public:
  static vtkMRMLCurveAnalysisNode *New();
  vtkTypeMacro(vtkMRMLCurveAnalysisNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    {return "CurveAnalysis";};

  // Description:
  // Get/Set a parameter for the module.

  void SetParameterAsDouble(const char *name, const double value)
  {};
  const char* GetParameterAsDouble(const char* name)
  {};

private:
  vtkMRMLCurveAnalysisNode();
  ~vtkMRMLCurveAnalysisNode();
  vtkMRMLCurveAnalysisNode(const vtkMRMLCurveAnalysisNode&);
  void operator=(const vtkMRMLCurveAnalysisNode&);

  vtkDoubleArray* SourceData;
  vtkDoubleArray* InterpolatedData;


};

#endif

