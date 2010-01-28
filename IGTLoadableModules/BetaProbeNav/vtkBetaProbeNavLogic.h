/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkBetaProbeNavLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkBetaProbeNavLogic_h
#define __vtkBetaProbeNavLogic_h

#include "vtkBetaProbeNavWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLNode.h"

class vtkPoints;
class vtkFloatArray;
class vtkPloyData;
class vtkMRMLModelNode;

class VTK_BetaProbeNav_EXPORT vtkBetaProbeNavLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkBetaProbeNavLogic *New();
  
  vtkTypeRevisionMacro(vtkBetaProbeNavLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);
  void CollectData(vtkMRMLNode*, vtkMRMLNode*);
  vtkMRMLModelNode* RepresentData(vtkMRMLModelNode* mnode);

 protected:
  
  vtkBetaProbeNavLogic();
  ~vtkBetaProbeNavLogic();

  void operator=(const vtkBetaProbeNavLogic&);
  vtkBetaProbeNavLogic(const vtkBetaProbeNavLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();
  
  vtkCallbackCommand *DataCallbackCommand;

 private:
   vtkPoints* Points;
   vtkFloatArray* Scalars;
   vtkPolyData* CountMap;
};

#endif


  
