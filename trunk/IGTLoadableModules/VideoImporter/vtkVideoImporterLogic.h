/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkVideoImporterLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkVideoImporterLogic_h
#define __vtkVideoImporterLogic_h

#include "vtkVideoImporterWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;

class VTK_VideoImporter_EXPORT vtkVideoImporterLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkVideoImporterLogic *New();
  
  vtkTypeRevisionMacro(vtkVideoImporterLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

 protected:
  
  vtkVideoImporterLogic();
  ~vtkVideoImporterLogic();

  void operator=(const vtkVideoImporterLogic&);
  vtkVideoImporterLogic(const vtkVideoImporterLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
