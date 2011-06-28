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

class vtkIGTLConnector;

class VTK_IGTView_EXPORT vtkIGTViewLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkIGTViewLogic *New();
  
  vtkTypeRevisionMacro(vtkIGTViewLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  vtkSetMacro(Link2DViewers, bool);
  vtkGetMacro(Link2DViewers, bool);

 protected:
  
  vtkIGTViewLogic();
  ~vtkIGTViewLogic();

  void operator=(const vtkIGTViewLogic&);
  vtkIGTViewLogic(const vtkIGTViewLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

  bool Link2DViewers;

 private:


};

#endif


  
