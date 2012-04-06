/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

// .NAME vtkMTIPlanLogic - slicer logic class for Locator module
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT.


#ifndef __vtkMTIPlanLogic_h
#define __vtkMTIPlanLogic_h

#include "vtkMTIPlanWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"
#include "vtkCollection.h"
#include "expat.h"

class vtkIGTLConnector;
class vtkCollection;

class VTK_MTIPlan_EXPORT vtkMTIPlanLogic : public vtkSlicerModuleLogic
{
 public:
  //BTX
  enum {  // Events
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:

  static vtkMTIPlanLogic *New();

  vtkTypeRevisionMacro(vtkMTIPlanLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);
  vtkSetMacro(ReadyToDraw, bool);
  vtkGetMacro(ReadyToDraw, bool);

  void ParseMTIPlanFile(const char* filename);
  static void startElement(void *userData, const XML_Char *name, const XML_Char **atts);
  static void endElement(void *userData, const XML_Char *name);

  //BTX
  typedef struct{
    double EntryPoint[3];
    double Target[3];
  }Trajectory;

  std::vector<std::string> vTargets;
  std::vector<std::string> vEntryPoint;
  std::vector<Trajectory> vTrajectories;

  std::string CaseName;
  //ETX


 protected:

  vtkMTIPlanLogic();
  ~vtkMTIPlanLogic();

  void operator=(const vtkMTIPlanLogic&);
  vtkMTIPlanLogic(const vtkMTIPlanLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:
  bool ReadyToDraw;

};

#endif



