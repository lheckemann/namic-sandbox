/*=========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH)
  All rights reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================*/
// .NAME vtkKWPlotGraph -  multi column list box to display/edit matrix4x4
// .SECTION Description
// vtkKWPlotGraph is a widget containing widgets that help view and
// edit a matrix
//

#ifndef __vtkKWPlotGraph_h
#define __vtkKWPlotGraph_h

#include "vtkFourDAnalysisWin32Header.h"
#include "vtkKWRenderWidget.h"

#include <string>
#include <vector>

class vtkDoubleArray;

class vtkXYPlotActor;

class VTK_FourDAnalysis_EXPORT vtkKWPlotGraph : public vtkKWRenderWidget
{
 public:
  //BTX
  typedef struct {
    int             visible;  // 0: invisible   1: visible
    vtkDoubleArray* data;
    std::string     label;
    double          color[3];
  } PlotDataType;
  //ETX
  
 public:
  static vtkKWPlotGraph* New();
  vtkTypeRevisionMacro(vtkKWPlotGraph,vtkKWRenderWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  //void SetAndObserveMatrix4x4(vtkMatrix4x4 *matrix);

  // Description:
  // Update the widget with the current values of the Matrix
  void UpdateWidget();
  // Description:
  // Update the matrix with the current values of the widget
  void UpdateGraph();

  int  SetNumberOfPlots(int n);
  void SetData(int id, vtkDoubleArray* data, const char* label);
  void SetColor(int id, double r, double g, double b);
  void AutoRangeOn();
  void AutoRangeOff();
  void SetXrange(double min, double max);
  void SetYrange(double min, double max);

  // Description:
  // Command to call when the User manipulates the widget
  virtual void SetCommand(vtkObject *object, const char *method) {};

  // TODO: have special commands for start/end events
  //virtual void SetStartCommand(vtkObject *object, const char *method);
  //virtual void SetEndCommand(vtkObject *object, const char *method);

  // Description:
  // TODO: access internal widgets
  //vtkKWRange* GetXRange() { return this->Range[0]; };

 protected:
  vtkKWPlotGraph();
  virtual ~vtkKWPlotGraph();

  // Description:
  // Create the widget.
  virtual void CreateWidget();

  char *Command;
  //char *StartCommand;
  //char *EndCommand;

 private:
  vtkKWPlotGraph(const vtkKWPlotGraph&); // Not implemented
  void operator=(const vtkKWPlotGraph&); // Not implemented

  vtkXYPlotActor* PlotActor;

  int Updating;

  //BTX
  typedef std::vector<PlotDataType> PlotDataVectorType;
  //ETX

  PlotDataVectorType PlotDataVector;
  int AutoRangeX;
  int AutoRangeY;
  double RangeX[2];
  double RangeY[2];

};

#endif

