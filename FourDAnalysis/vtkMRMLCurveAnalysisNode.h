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

#include "vtkDoubleArray.h"
#include "vtkStringArray.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_FourDAnalysis_EXPORT vtkMRMLCurveAnalysisNode : public vtkMRMLNode
{
 public:
  //BTX
  typedef std::map<std::string, double> OutputParameterMapType;
  typedef std::map<std::string, vtkDoubleArray*> InputDataMapType;
  typedef std::map<std::string, double> InitialOptimParameterMapType;
  //ETX

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

  vtkSetObjectMacro( SourceData, vtkDoubleArray );
  vtkGetObjectMacro( SourceData, vtkDoubleArray );
  vtkSetObjectMacro( FittedData, vtkDoubleArray );
  vtkGetObjectMacro( FittedData, vtkDoubleArray );

  void SetFunctionName(const char* name)
  {
    this->FunctionName = name;
  }

  const char* GetFunctionName()
  {
    return this->FunctionName.c_str();
  }

  // Input curve data
  void SetInputData(const char* name, vtkDoubleArray* data=NULL);
  vtkDoubleArray* GetInputData(const char* name);
  vtkStringArray* GetInputDataNameArray();
  void ClearInputData();

  // Initial optimization parameters
  void SetInitialOptimParameter(const char* name, double value);
  double GetInitialOptimParameter(const char* name);
  vtkStringArray* GetInitialOptimParameterNameArray();
  void ClearInitialOptimParameters();

  // Output parameters
  void SetParameter(const char *name, const double value);
  double GetParameter(const char* namoe);
  vtkStringArray* GetParameterNameArray();
  void ClearParameters();


private:
  vtkMRMLCurveAnalysisNode();
  ~vtkMRMLCurveAnalysisNode();
  vtkMRMLCurveAnalysisNode(const vtkMRMLCurveAnalysisNode&);
  void operator=(const vtkMRMLCurveAnalysisNode&);

  vtkDoubleArray* SourceData;
  vtkDoubleArray* FittedData;

  OutputParameterMapType       OutputParameters;
  InputDataMapType             InputData;
  InitialOptimParameterMapType InitialOptimParameters;

  //BTX
  std::string FunctionName;
  //ETX

};

#endif

