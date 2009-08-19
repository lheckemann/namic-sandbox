/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCurveAnalysisNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLPlotObjectOrthogonalLine2DNode_h
#define __vtkMRMLPlotObjectOrthogonalLine2DNode_h

#include <string>
#include <vector>

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkObject.h"
#include "vtkFourDAnalysisWin32Header.h"

#include "vtkDataObject.h"

class VTK_FourDAnalysis_EXPORT vtkMRMLPlotObjectOrthogonalLine2DNode : public vtkMRMLNode
{

 public:

  //----------------------------------------------------------------
  // Constants
  //----------------------------------------------------------------

  // Line direction
  //BTX
  enum {
    VERTICAL = 0,
    HORIZONTAL
  };
  //ETX


  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLPlotObjectOrthogonalLine2DNode *New();
  vtkTypeMacro(vtkMRMLPlotObjectOrthogonalLine2DNode,vtkMRMLNode);
  
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
    {return "PlotObjectLine2D";};

  // Description:
  // Method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );


  //----------------------------------------------------------------
  // Get and Set Macros
  //----------------------------------------------------------------
 public:
  vtkSetVector2Macro ( Point, double );
  vtkGetVector2Macro ( Point, double );

  vtkSetMacro ( Direction, int );
  vtkGetMacro ( Direction, int );
  

  //----------------------------------------------------------------
  // Access methods
  //----------------------------------------------------------------
 public:

  //----------------------------------------------------------------
  // Methods for plotting (called from friend classes)
  //----------------------------------------------------------------
 protected:
  // Description:
  // Get minimum and muximum X values.
  // Returns 0 if the Y range cannot be determined.
  virtual int GetXange(double* xrange);

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
  vtkMRMLPlotObjectOrthogonalLine2DNode();
  ~vtkMRMLPlotObjectOrthogonalLine2DNode();
  vtkMRMLPlotObjectOrthogonalLine2DNode(const vtkMRMLPlotObjectOrthogonalLine2DNode&);
  void operator=(const vtkMRMLPlotObjectOrthogonalLine2DNode&);


 protected:
  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------
  
  double Point[2];   // A point on the line
  int Direction;

};



#endif

