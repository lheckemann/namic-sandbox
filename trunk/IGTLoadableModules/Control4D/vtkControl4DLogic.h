/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/

// .NAME vtkControl4DLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkControl4DLogic_h
#define __vtkControl4DLogic_h

#include "vtkControl4DWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"
#include "vtkDoubleArray.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLScene.h"

class vtkIGTLConnector;

class VTK_Control4D_EXPORT vtkControl4DLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent    = 50000,
    StatusUpdateEvent       = 50001,
    ProgressDialogEvent     = 50002,
  };
  enum {
    TYPE_MEAN,
    TYPE_SD,
  };
  typedef struct {
    int    show;
    double progress;
    std::string message;
  } StatusMessageType;
  
  typedef std::vector<std::string> FrameNodeVectorType;
  typedef struct {
    int x;
    int y;
    int z;
  } CoordType;
  typedef std::vector<CoordType> IndexTableType;
  typedef std::map<int, vtkDoubleArray*> IntensityCurveSetType;
  typedef std::map<std::string, IntensityCurveSetType> IntensityCurveCacheType;
  //ETX

 public:
  
  static vtkControl4DLogic *New();
  
  vtkTypeRevisionMacro(vtkControl4DLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  // Description:
  // Loads series of volumes from the directory that contains the file
  // specified by 'path' argument.
  // Returns number of volumes in the series.
  int LoadImagesFromDir(const char* path);

  const char* SwitchNodeFG(int index);
  const char* SwitchNodeBG(int index);

  void  ClearIntensityCurveCache(const char* maskID);
  vtkDoubleArray* GetIntensityCurve(const char* maskID, int label, int type);
  int  SaveIntensityCurve(const char* maskID, int label, const char* filename);

 protected:
  
  vtkControl4DLogic();
  ~vtkControl4DLogic();

  void operator=(const vtkControl4DLogic&);
  vtkControl4DLogic(const vtkControl4DLogic&);

  virtual void ProcessLogicEvents ( vtkObject * /*caller*/, 
                                  unsigned long /*event*/, 
                                  void * /*callData*/ );  

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  double GetMeanIntencity(vtkImageData* image);
  double GetSDIntencity(vtkImageData* image, double mean);
  void   GenerateIndexTable(vtkImageData* mask, int label);

  vtkCallbackCommand *DataCallbackCommand;

 private:

  vtkMRMLScene* MRMLScene;

  //BTX
  FrameNodeVectorType FrameNodeVector;
  IndexTableType MaskIndexTable;
  IntensityCurveCacheType IntensityCurveCache;
  IntensityCurveCacheType IntensitySDCurveCache;
  //ETX

  const char* CurrentFrameFG;
  const char* CurrentFrameBG;

  
};

#endif


  
