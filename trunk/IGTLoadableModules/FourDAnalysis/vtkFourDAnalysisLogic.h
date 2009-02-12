/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/IGTLoadableModules/FourDAnalysis/vtkFourDAnalysisLogic.h $
  Date:      $Date: 2009-02-03 12:05:00 -0500 (Tue, 03 Feb 2009) $
  Version:   $Revision: 3633 $

==========================================================================*/

// .NAME vtkFourDAnalysisLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkFourDAnalysisLogic_h
#define __vtkFourDAnalysisLogic_h

#include "vtkFourDAnalysisWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"
#include "vtkDoubleArray.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRML4DBundleNode.h"

#include "itkOrientedImage.h"
#include "itkImageSeriesReader.h"


#include "vtkIntensityCurves.h"

#include <string>
#include <map>


class vtkIGTLConnector;

class VTK_FourDAnalysis_EXPORT vtkFourDAnalysisLogic : public vtkSlicerModuleLogic 
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

  typedef std::vector<CoordType>                       IndexTableType;

  typedef struct {
    long             MaskModifiedTime;
    vtkDoubleArray*  Mean;
    vtkDoubleArray*  SD;
  } CurveDataType;
  
  typedef std::map<int, CurveDataType> CurveDataListType;
  typedef struct {
    std::string       MaskNodeID;
    CurveDataListType CurveList;
  } CurveDataSetType;

  typedef std::map<std::string, CurveDataSetType> CurveCacheType;

  typedef std::map<std::string, std::string> RegistrationParametersType;
  //ETX

 public:
  
  static vtkFourDAnalysisLogic *New();
  
  vtkTypeRevisionMacro(vtkFourDAnalysisLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  // Description:
  // Loads series of volumes from the directory that contains the file
  // specified by 'path' argument.
  // Returns number of volumes in the series.

  //BTX
  //const int SpaceDim = 3;
  typedef short PixelValueType;
  typedef itk::OrientedImage< PixelValueType, 3 > VolumeType;
  typedef itk::ImageSeriesReader< VolumeType > ReaderType;
  int CreateFileListFromDir(const char* path,
                            std::vector<ReaderType::FileNamesContainer>& fileNamesContainerList);
  //ETX
  vtkMRML4DBundleNode* LoadImagesFromDir(const char* path, const char* bundleNodeName,
                                         double& rangeLower, double& rangeUpper);
  int SaveImagesToDir(const char* path, const char* bundleID, const char* prefix, const char* suffix);

  vtkMRMLScalarVolumeNode* AddDisplayBufferNode(vtkMRML4DBundleNode* bundleNode, 
                                                int index);
  int         GetNumberOfFrames();
  const char* GetFrameNodeID(int index);
  const char* GetRegisteredFrameNodeID(int index);

  vtkDoubleArray* GetIntensityCurve(const char* bundleID, const char* maskID, int label, int type);
  int  SaveIntensityCurve(const char* bundleID, const char* maskID, int label, const char* filename);

  void SetApplication(vtkSlicerApplication *app) { this->Application = app; };
  vtkSlicerApplication* GetApplication() { return this->Application; };
  //int  RunSeriesRegistration(int sIndex, int eIndex, int kIndex, RegistrationParametersType& param);

  int GenerateBundleFrames(vtkMRML4DBundleNode* inputBundleNode,
                           vtkMRML4DBundleNode* outputBundleNode);

  int RunSeriesRegistration(int sIndex, int eIndex, int kIndex, 
                            const char* inputBundleNodeID,
                            const char* outputBundleNodeID,
                            RegistrationParametersType& param);

 protected:
  
  vtkFourDAnalysisLogic();
  ~vtkFourDAnalysisLogic();

  void operator=(const vtkFourDAnalysisLogic&);
  vtkFourDAnalysisLogic(const vtkFourDAnalysisLogic&);

  virtual void ProcessLogicEvents ( vtkObject * /*caller*/, 
                                  unsigned long /*event*/, 
                                  void * /*callData*/ );  

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  double GetMeanIntensity(vtkImageData* image, IndexTableType& indexTable);
  double GetSDIntensity(vtkImageData* image, double mean, IndexTableType& indexTable);
  void   GenerateIndexTable(vtkImageData* mask, int label, IndexTableType& indexTable);


  //----------------------------------------------------------------
  // Registration
  //----------------------------------------------------------------
  //BTX
  int  RunRegistration(vtkMRML4DBundleNode* bundleNode,
                       vtkMRMLScalarVolumeNode* fixedNode,
                       vtkMRMLScalarVolumeNode* movingNode,
                       vtkMRMLScalarVolumeNode* outputNode,
                       RegistrationParametersType& param);
  //ETX

  vtkSlicerApplication *Application;
  vtkCallbackCommand *DataCallbackCommand;

 private:

  vtkMRMLScene* MRMLScene;

  //BTX
  FrameNodeVectorType FrameNodeVector;
  FrameNodeVectorType RegisteredFrameNodeVector;
  std::string VolumeBundleID;
  std::string RegisteredVolumeBundleID;

  CurveCacheType CurveCache;  // CurveCache[<4d bundle name>][<label number>].<member of CurveDataType>
  //ETX

};

#endif


  
