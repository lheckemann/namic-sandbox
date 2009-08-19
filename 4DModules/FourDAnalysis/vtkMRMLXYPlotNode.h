/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCurveAnalysisNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLXYPlotNode_h
#define __vtkMRMLXYPlotNode_h

#include <string>
#include <vector>

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"


#include "vtkObject.h"
#include "vtkFourDAnalysisWin32Header.h"

#include "vtkMRMLDoubleArrayNode.h"
#include "vtkMRMLPlotObjectNode.h"

class vtkIntArray;

class VTK_FourDAnalysis_EXPORT vtkMRMLXYPlotNode : public vtkMRMLNode
{

 public:

  //----------------------------------------------------------------
  // Constants
  //----------------------------------------------------------------

  // Events
  //BTX
  enum {
    UpdateGraphEvent = 11900,
  };
  //ETX

  // Interpolation method
  //BTX
  enum {
    INTERP_LINEAR = 0,
  };
  enum {
    TYPE_CURVE = 0,
    TYPE_VERTICAL_LINE,
    TYPE_HORIZONTAL_LINE,
  };
  //ETX

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLXYPlotNode *New();
  vtkTypeMacro(vtkMRMLXYPlotNode,vtkMRMLNode);
  
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes.
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object.
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() { return "XYPlot"; };

  // Description:
  // Method to propagate events generated in mrml.
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  //----------------------------------------------------------------
  // Get and Set Macros
  //----------------------------------------------------------------
  //vtkSetObjectMacro ( Array, vtkDoubleArray );
  //vtkGetObjectMacro ( Array, vtkDoubleArray );

  //----------------------------------------------------------------
  // PlotObjectNode management
  //----------------------------------------------------------------
  
  // Description:
  // Add DoiubleArrayNode to the plotting list. Returns array ID.
  int AddPlot(vtkMRMLPlotObjectNode* node);

  // Description:
  // Remove DoubleArrayNode from the plotting list.
  void RemovePlot(int id);

  // Description:
  // Remove DoubleArrayNode from the plotting list.
  void RemovePlotByNodeID(const char* nodeID);

  // Description:
  // Remove all DoubleArrayNode from the plotting list.
  void ClearPlots();

  // Description:
  // Get number of arrays on the list
  unsigned int GetNumberOfPlots();

  // Description:
  // Get list of IDs
  vtkIntArray* GetPlotIDList();

  // Description:
  // Get n-th vtkMRMLDoubleArrayNode on the list
  vtkMRMLPlotObjectNode* GetPlot(int id);

  //----------------------------------------------------------------
  // Methods to change property of plot objects
  //----------------------------------------------------------------

  // Description:
  // Set visibility of the all curves (i == 0 : off; i == 1 : on)
  void SetVisibilityAll(int i);

  // Description:
  // Set visibility of the all curves (i == 0 : off; i == 1 : on)
  void SetErrorBarAll(int i);


  //----------------------------------------------------------------
  // Plot graph
  //----------------------------------------------------------------

  // Description:  
  // Invoke UpdateGraphEvent to force refreshing the graph
  void Refresh();

  //----------------------------------------------------------------
  // Graph properties
  //----------------------------------------------------------------

  // Description:
  // Set title of graph.
  void SetTitle(const char* str)
  {
    this->Title = str;
    this->Modified();
  };

  // Description:
  // Get title of graph.
  const char* GetTitle()
  {
    return this->Title.c_str();
  };

  // Description:
  // Set label for X-axis
  void SetXLabel(const char* str)
  {
    this->XLabel = str;
    this->Modified();
  };

  // Description:
  // Get label for X-axis
  const char* GetXLabel()
  {
    return this->XLabel.c_str();
  };

  // Description:
  // Set label for Y-axis
  void SetYLabel(const char* str)
  {
    this->YLabel = str;
    this->Modified();
  };
  
  // Description:
  // Get label for Y-axis
  const char* GetYLabel()
  {
    return this->YLabel.c_str();
  };

  // Description:
  // Set/Get automatic range setting flag
  vtkSetMacro ( AutoXRange, int );
  vtkGetMacro ( AutoXRange, int );
  vtkSetMacro ( AutoYRange, int );
  vtkGetMacro ( AutoYRange, int );

  // Description:
  // Set X range
  void SetXRange(double* range)
  {
    if (range[0] <= range[1])
      {
        this->XRange[0] = range[0];
        this->XRange[1] = range[1];
        this->Modified();
      }
  };

  // Description:
  // Set Y range
  void SetYRange(double* range)
  {
    if (range[0] <= range[1])
      {
        this->YRange[0] = range[0];
        this->YRange[1] = range[1];
        this->Modified();
      }
  };

  // Description:
  // Get X range
  void GetXRange(double* range)
  {
    range[0] = this->YRange[0];
    range[1] = this->YRange[1];
  };
 
  // Description:
  // Get Y range
  void GetYRange(double* range)
  {
    range[0] = this->YRange[0];
    range[1] = this->YRange[1];
  };

  // Description:
  // Set color of axes
  void SetAxesColor(double r, double g, double b)
  {
    this->AxesColor[0] = r;
    this->AxesColor[1] = g;
    this->AxesColor[2] = b;
    this->Modified();
  };
  
  // Description:
  // Get color of axes
  void GetAxesColor(double* r, double* g, double* b)
  {
    *r = this->AxesColor[0];
    *g = this->AxesColor[1];
    *b = this->AxesColor[2];
  };

  // Description:
  // Set color of background
  void SetBackgroundColor(double r, double g, double b)
  {
    this->BackgroundColor[0] = r;
    this->BackgroundColor[1] = g;
    this->BackgroundColor[2] = b;
    this->Modified();
  };

  // Description:
  // Get color of background
  void GetBackgroundColor(double* r, double* g, double* b)
  {
    *r = this->BackgroundColor[0];
    *g = this->BackgroundColor[1];
    *b = this->BackgroundColor[2];
  };

  
 protected:
  //----------------------------------------------------------------
  // Constructor and destroctor
  //----------------------------------------------------------------
  
  vtkMRMLXYPlotNode();
  ~vtkMRMLXYPlotNode();
  vtkMRMLXYPlotNode(const vtkMRMLXYPlotNode&);
  void operator=(const vtkMRMLXYPlotNode&);


 protected:
  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------

  //BTX
  std::string Title;             // Plotting graph title
  std::string XLabel;            // Label for x-axis
  std::string YLabel;            // Label for y-axis
  std::map< int, vtkMRMLPlotObjectNode* > Data;  // map for plotting data
  //ETX

  int LastArrayID;

  int    AutoXRange;  // Flag for automatic range adjust (default: 0 = off)
  int    AutoYRange;  // Flag for automatic range adjust (default: 0 = off)
  double XRange[2];
  double YRange[2];

  double AxesColor[3];
  double BackgroundColor[3];
  
};

#endif

