/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

// .NAME vtkBetaProbeLogic - slicer logic class for Locator module
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT.


#ifndef __vtkBetaProbeLogic_h
#define __vtkBetaProbeLogic_h

#include "vtkBetaProbeWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLUDPServerNode.h"
#include "itkMultiThreader.h"
#include "vtkMRMLLinearTransformNode.h"

class vtkIGTLConnector;
class vtkMatrix3x3;
class vtkMatrix4x4;
class vtkCollection;
class vtkVector3d;
class vtkMRMLUDPServerNode;
class vtkMRMLLinearTransformNode;

class VTK_BetaProbe_EXPORT vtkBetaProbeLogic : public vtkSlicerModuleLogic
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:

  static vtkBetaProbeLogic *New();

  vtkTypeRevisionMacro(vtkBetaProbeLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);


  // Detection
  vtkGetMacro(DetectionRunning, bool);
  vtkSetMacro(DetectionRunning, bool);

  void StartTumorDetection(int threshold);
  void StopTumorDetection();
  static ITK_THREAD_RETURN_TYPE TumorDetection(void *pInfoStruct);
  void BeepFunction(int interval_ms);

  // Mapping
  vtkSetObjectMacro(UDPServerNode, vtkMRMLUDPServerNode);
  vtkGetObjectMacro(UDPServerNode, vtkMRMLUDPServerNode);
  vtkSetObjectMacro(PositionTransform, vtkMRMLLinearTransformNode);
  vtkGetObjectMacro(PositionTransform, vtkMRMLLinearTransformNode);
  vtkSetObjectMacro(DataToMap, vtkMRMLScalarVolumeNode);
  vtkGetObjectMacro(DataToMap, vtkMRMLScalarVolumeNode);
  vtkSetObjectMacro(ColorNode, vtkMRMLColorTableNode);
  vtkGetObjectMacro(ColorNode, vtkMRMLColorTableNode);
  vtkSetObjectMacro(MappedVolume, vtkMRMLScalarVolumeNode);
  vtkGetObjectMacro(MappedVolume, vtkMRMLScalarVolumeNode);
  vtkSetObjectMacro(RASToIJKMatrix, vtkMatrix4x4);
  vtkGetObjectMacro(RASToIJKMatrix, vtkMatrix4x4);
  vtkSetObjectMacro(IJKToRASDirectionMatrix, vtkMatrix4x4);
  vtkGetObjectMacro(IJKToRASDirectionMatrix, vtkMatrix4x4);
  vtkSetMacro(CountsType, int);
  vtkGetMacro(CountsType, int);
  vtkSetMacro(MappingRunning, bool);
  vtkGetMacro(MappingRunning, bool);

  void StartMapping();
  void StopMapping();
  static ITK_THREAD_RETURN_TYPE MappingFunction(void *pInfoStruct);

  // Calibration
  void PivotCalibration(vtkCollection* PivotingMatrix, double AveragePcal[3]);

  vtkMatrix4x4* TmpMatrix;

 protected:

  vtkBetaProbeLogic();
  ~vtkBetaProbeLogic();

  void operator=(const vtkBetaProbeLogic&);
  vtkBetaProbeLogic(const vtkBetaProbeLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:

  vtkMRMLUDPServerNode *UDPServerNode;

  int CountsType;

  // Mapping
  vtkMRMLLinearTransformNode* PositionTransform;
  vtkMRMLScalarVolumeNode* DataToMap;
  vtkMRMLColorTableNode* ColorNode;
  vtkMRMLScalarVolumeNode* MappedVolume;
  vtkMatrix4x4* RASToIJKMatrix;
  vtkMatrix4x4* IJKToRASDirectionMatrix;
  bool MappingRunning;

  int MappingThreadID;
  // Detection
  int ThreadID;
  //BTX
  itk::MultiThreader::Pointer m_Threader;
  //ETX
  bool DetectionRunning;
  int ThresholdDetection;


};

#endif



