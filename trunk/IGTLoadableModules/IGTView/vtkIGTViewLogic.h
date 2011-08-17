/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

// .NAME vtkIGTViewLogic - slicer logic class for Locator module
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT.


#ifndef __vtkIGTViewLogic_h
#define __vtkIGTViewLogic_h

#include "vtkIGTViewWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLCrosshairNode.h"
#include "vtkMRMLTrajectoryNode.h"

class vtkIGTLConnector;
class vtkMRMLLinearTransformNode;
class vtkMRMLCrosshairNode;
class vtkMRMLSliceNode;
class vtkMRMLTrajectoryNode;

class VTK_IGTView_EXPORT vtkIGTViewLogic : public vtkSlicerModuleLogic
{
 public:
  //BTX
  enum {
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:

  static vtkIGTViewLogic *New();

  vtkTypeRevisionMacro(vtkIGTViewLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  void  Reslice(vtkMRMLSliceNode* RedSlice, vtkMRMLSliceNode* YellowSlice, vtkMRMLSliceNode* GreenSlice);
  void  ObliqueOrientation(vtkMRMLSliceNode* slice, const char* ReslicingType);
  void  UpdateCrosshair();
  void  DisplayTrajectory();

  vtkSetMacro(TransformNodeSelected, bool);
  vtkGetMacro(TransformNodeSelected, bool);
  vtkSetObjectMacro(locatorTransformNode, vtkMRMLLinearTransformNode);
  vtkGetObjectMacro(locatorTransformNode, vtkMRMLLinearTransformNode);
  vtkSetObjectMacro(Crosshair, vtkMRMLCrosshairNode);
  vtkGetObjectMacro(Crosshair, vtkMRMLCrosshairNode);
  vtkSetMacro(CrosshairEnabled, bool);
  vtkGetMacro(CrosshairEnabled, bool);
  vtkGetObjectMacro(TrajectoryNode, vtkMRMLTrajectoryNode);
  vtkSetObjectMacro(TrajectoryNode, vtkMRMLTrajectoryNode);
  vtkGetMacro(TrajectoryEnabled, bool);
  vtkSetMacro(TrajectoryEnabled, bool);

 protected:

  vtkIGTViewLogic();
  ~vtkIGTViewLogic();

  void operator=(const vtkIGTViewLogic&);
  vtkIGTViewLogic(const vtkIGTViewLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

  bool                          TransformNodeSelected;
  vtkMRMLLinearTransformNode*   locatorTransformNode;
  double                        locatorPosition[3];
  vtkMRMLCrosshairNode*         Crosshair;
  bool                          CrosshairEnabled;
  vtkMRMLTrajectoryNode*        TrajectoryNode;
  bool                          TrajectoryEnabled;

 private:


};

#endif



