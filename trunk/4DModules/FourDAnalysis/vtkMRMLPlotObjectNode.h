/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCurveAnalysisNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLPlotObjectNode_h
#define __vtkMRMLPlotObjectNode_h

#include <string>
#include <vector>

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkObject.h"
#include "vtkFourDAnalysisWin32Header.h"

#include "vtkDataObject.h"

class VTK_FourDAnalysis_EXPORT vtkMRMLPlotObjectNode : public vtkMRMLNode
{

  //----------------------------------------------------------------
  // Friend classes
  //----------------------------------------------------------------
  // Description:
  // These clases are defined as a friend calls of vtkMRMLPlotObjectNode
  // to call protected the function: GetDrawObject();
  friend vtkSlicerXYPlotWidget;
  friend vtkMRMLXYPlotNode;

 public:
  //----------------------------------------------------------------
  // Constants
  //----------------------------------------------------------------

  // Interpolation method
  //BTX
  enum {
    INTERP_LINEAR = 0,
  };
  //ETX

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLPlotObjectNode *New();
  vtkTypeMacro(vtkMRMLPlotObjectNode,vtkMRMLNode);
  
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
    {return "PlotObject";};

  // Description:
  // Method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  //----------------------------------------------------------------
  // Get and Set Macros
  //----------------------------------------------------------------
  vtkSetMacro ( Visible, int );
  vtkGetMacro ( Visible, int );

  // Description:
  // Set line color by SetColor(double, double, double) or SetColor(Double* c[3])
  vtkSetVector3Macro( Color, double );
  vtkGetVector3Macro( Color, double );
  
  //----------------------------------------------------------------
  // Access methods
  //----------------------------------------------------------------

  // Description:
  // Get Y range for given X range.
  // Returns 0 if it cannot determin the x range.
  virtual int GetYRange(const double* xrange, double* yrange) { return 0; };

  // Description:
  // Set line color
  void SetColor(double r, double g, double b)
  {
    this->Color[0] = r;
    this->Color[1] = g;
    this->Color[2] = b;
    this->Modified();
  }
  
  // Description:
  // Get line color
  void GetColor(double* r, double* g, double* b);
  {
    *r = this->Color[0];
    *g = this->Color[1];
    *b = this->Color[2];
  }
  
  // Description:
  // Set legend of the object for the graph.
  void SetLegend(const char* legend)
  {
    this->Legend() = legend;
  }


  // Description:
  // Get legend of the object for the graph.
  const char* GetLegend()
  {
    return this->Legend().c_str();
  }


  //----------------------------------------------------------------
  // Methods for Plotting (called from friend classes)
  //----------------------------------------------------------------
 protected:
  // Description:
  // Get minimum and muximum X values.
  // Returns 0 if the Y range cannot be determined.
  virtual int GetXRange(double* xrange);

  // Description:
  // Get minimum and muximum Y values.
  // Returns 0 if the Y range cannot be determined.
  virtual int GetYRange(double* yrange);

  // Description:
  // Get draw object (this funciton is called by vtkMRMLXYPlotNode)
  virtual vtkDataObject* GetDrawObject(double* xrange, double* yrange);


  //----------------------------------------------------------------
  // Constructor and destroctor
  //----------------------------------------------------------------
 protected:
  vtkMRMLPlotObjectNode();
  ~vtkMRMLPlotObjectNode();
  vtkMRMLPlotObjectNode(const vtkMRMLPlotObjectNode&);
  void operator=(const vtkMRMLPlotObjectNode&);


 protected:
  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------
  int Visible;
  int Color[3];
  //BTX
  std::string Legend;
  //ETX

};



#endif

