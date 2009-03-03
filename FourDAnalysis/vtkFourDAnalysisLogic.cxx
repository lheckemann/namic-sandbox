/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/IGTLoadableModules/FourDAnalysis/vtkFourDAnalysisLogic.cxx $
  Date:      $Date: 2009-02-03 12:05:00 -0500 (Tue, 03 Feb 2009) $
  Version:   $Revision: 3633 $

==========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkSlicerColorLogic.h"
#include "vtkFourDAnalysisLogic.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"
#include "vtkCommandLineModuleLogic.h"
#include "vtkCommandLineModuleGUI.h"

#include "itkMetaDataDictionary.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbesCollectorBase.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLBSplineTransformNode.h"
#include "itksys/DynamicLoader.hxx"

#include "vtkMRML4DBundleNode.h"

#include "vtkGlobFileNames.h"

#include "vtkSlicerVolumesGUI.h"
#include "vtkSlicerVolumesLogic.h"

#include <cmath>

#ifdef Slicer3_USE_PYTHON
//// If debug, Python wants pythonxx_d.lib, so fake it out
//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
#include <Python.h>
//#endif

//extern "C" {
//  void init_mytkinter( Tcl_Interp* );
//  void init_slicer(void );
//}
//#include "vtkTclUtil.h"
//
#endif

vtkCxxRevisionMacro(vtkFourDAnalysisLogic, "$Revision: 3633 $");
vtkStandardNewMacro(vtkFourDAnalysisLogic);

//---------------------------------------------------------------------------
vtkFourDAnalysisLogic::vtkFourDAnalysisLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkFourDAnalysisLogic::DataCallback);

  this->FrameNodeVector.clear();

  this->CurveCache.clear();

  this->VolumeBundleID = "";
  this->RegisteredVolumeBundleID = "";

}


//---------------------------------------------------------------------------
vtkFourDAnalysisLogic::~vtkFourDAnalysisLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkFourDAnalysisLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkFourDAnalysisLogic:             " << this->GetClassName() << "\n";

}

//----------------------------------------------------------------------------
void vtkFourDAnalysisLogic::ProcessLogicEvents(vtkObject *caller, 
                                            unsigned long event, 
                                            void *callData)
{
  if (event ==  vtkCommand::ProgressEvent) 
    {
    this->InvokeEvent ( vtkCommand::ProgressEvent,callData );
    }
}

//---------------------------------------------------------------------------
void vtkFourDAnalysisLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkFourDAnalysisLogic *self = reinterpret_cast<vtkFourDAnalysisLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkFourDAnalysisLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkFourDAnalysisLogic::UpdateAll()
{
}


//---------------------------------------------------------------------------
int vtkFourDAnalysisLogic::CreateFileListFromDir(const char* path,
                                             std::vector<ReaderType::FileNamesContainer>& fileNamesContainerList)
{

  typedef itk::OrientedImage< PixelValueType, 2 > SliceType;
  typedef itk::ImageFileReader< SliceType > SliceReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
  typedef itk::ImageFileWriter< VolumeType >  WriterType;
  //typedef itk::VectorImage< PixelValueType, SpaceDim > NRRDImageType;
  typedef itk::VectorImage< PixelValueType, 3 > NRRDImageType;
  
  fileNamesContainerList.clear();
  
  StatusMessageType statusMessage;
  
  //itk::TimeProbesCollectorBase collector;

  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Checking directory....";
  this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);
  
  int nVolumes;

  // Search files with compatible types (except DICOM)
  vtkGlobFileNames* gfn = vtkGlobFileNames::New();
  gfn->SetDirectory(path);
  gfn->AddFileNames("*.nhdr");
  gfn->AddFileNames("*.nrrd");
  gfn->AddFileNames("*.hdr");
  gfn->AddFileNames("*.img");
  gfn->AddFileNames("*.nii");
  gfn->AddFileNames("*.nia");

  int nFiles = gfn->GetNumberOfFileNames();
  if (nFiles > 0)
    {
    //std::cerr << "find non-dicom files" << std::endl;
    for (int i = 0; i < nFiles; i ++)
      {
      ReaderType::FileNamesContainer container;
      container.clear();
      //std::cerr << "FileName #" << i << " " << gfn->GetNthFileName(i) << std::endl;
      container.push_back(gfn->GetNthFileName(i));
      fileNamesContainerList.push_back(container);
      }
    nVolumes = nFiles;  // ??
    }
  else
    {
    // in case of dicom series
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    gdcmIO->LoadPrivateTagsOn();
    
    InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
    inputNames->SetUseSeriesDetails(true);
    inputNames->SetDirectory(path);
    
    itk::SerieUIDContainer seriesUIDs = inputNames->GetSeriesUIDs();
    if (seriesUIDs.size() == 1)  // if single series UID is used
      {
      statusMessage.message = "Splitting series....";
      this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);
      
      const ReaderType::FileNamesContainer & filenames = 
        inputNames->GetFileNames(seriesUIDs[0]);
      
      ReaderType::Pointer reader = ReaderType::New();
      reader->SetImageIO( gdcmIO );
      reader->SetFileNames( filenames );
      
      try
        {
        //collector.Start( "Checking series ...." );
        reader->Update();
        //collector.Stop( "Checking series ...." );
        }
      catch (itk::ExceptionObject &excp)
        {
        std::cerr << "Exception thrown while reading the series" << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
        }
      ReaderType::DictionaryArrayRawPointer inputDict = reader->GetMetaDataDictionaryArray();
      int nSlices = inputDict->size();
      
      // search "cycle" of slice location
      std::string tag;
      tag.clear();
      itk::ExposeMetaData<std::string> ( *(*inputDict)[0], "0020|1041",  tag);
      float firstSliceLocation = atof( tag.c_str() ); // first slice location
      int nSlicesInVolume;
      
      statusMessage.progress = 0.0;
      statusMessage.message = "Checking slice locations...";
      this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);
      
      for (int i = 1; i < nSlices; i ++)
        {
        statusMessage.progress = (double)i/(double)nSlices;
        this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);
        
        tag.clear();
        itk::ExposeMetaData<std::string> ( *(*inputDict)[i], "0020|1041",  tag);
        float sliceLocation = atof( tag.c_str() );
        //std::cerr << "location = " << tag.c_str() << std::endl;
        if (sliceLocation == firstSliceLocation)
          {
          nSlicesInVolume = i;
          break;
          }
        }
      /*
        std::cerr << "Number of slices in Volume is " << nSlicesInVolume << std::endl;
        std::cerr << "Number of slices " << nSlices << std::endl;
        std::cerr << "Number of slices " << filenames.size() << std::endl;
      */
      
      nVolumes = nSlices / nSlicesInVolume;
      fileNamesContainerList.resize(nVolumes);
      
      for (int i = 0; i < nVolumes; i ++)
        {
        fileNamesContainerList[i].resize(nSlicesInVolume);
        for (int j = 0; j < nSlicesInVolume; j ++)
          {
          //std::cerr << "fileNamesContainerList " << i << ", " << j << std::endl;
          fileNamesContainerList[i][j] = filenames[i*nSlicesInVolume + j];
          }
        }
      
      }
    else // if the directory contains multiple series UIDs
      {
      nVolumes = seriesUIDs.size();
      //fileNamesContainerList.resize(nVolumes);
      fileNamesContainerList.clear();
      
      itk::SerieUIDContainer::iterator iter;
      for (iter = seriesUIDs.begin(); iter != seriesUIDs.end(); iter ++)
        {
        //std::cerr << "UID = " << *iter << std::endl;
        fileNamesContainerList.push_back(inputNames->GetFileNames(*iter));
        }
      }
    }

  return nVolumes;
}


//---------------------------------------------------------------------------
vtkMRML4DBundleNode* vtkFourDAnalysisLogic::LoadImagesFromDir(const char* path, const char* bundleNodeName,
                                                          double& rangeLower,
                                                          double& rangeUpper)
{
  StatusMessageType statusMessage;
  
  std::cerr << "loading from " << path << std::endl;

  std::vector<ReaderType::FileNamesContainer> fileNamesContainerList;

  // Analyze the directory and create the file list
  if (CreateFileListFromDir(path, fileNamesContainerList) <= 0)
    {
    std::cerr << "Couldn't find files" << std::endl;
    return NULL;
    }

  int nVolumes = fileNamesContainerList.size();
  std::cerr << "nVolumes = " << nVolumes << std::endl;


  //std::vector<ReaderType::FileNamesContainer>::iterator fnciter;
  //for (fnciter = fileNamesContainerList.begin(); fnciter != fileNamesContainerList.end(); fnciter ++)

  //vtkMRMLScene* scene = vtkMRMLScene::New();
  vtkMRMLScene* scene = this->GetMRMLScene();
  scene->SaveStateForUndo();
  this->FrameNodeVector.clear();

  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Reading Volumes....";
  this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

  rangeLower = 0.0;
  rangeUpper = 0.0;

  // prepare 4D bundle
  vtkMRML4DBundleNode* bundleNode = vtkMRML4DBundleNode::New();
  //vtkMRMLLinearTransformNode* bundleNode = vtkMRMLLinearTransformNode::New();
  //bundleNode->SetName("4DBundle");
  bundleNode->SetName(bundleNodeName);
  bundleNode->SetDescription("Created by FourDAnalysis");

  vtkMatrix4x4* transform = vtkMatrix4x4::New();
  transform->Identity();
  bundleNode->ApplyTransform(transform);
  transform->Delete();
  scene->AddNode(bundleNode);

  for (int i = 0; i < nVolumes; i ++)
    {
    //std::cerr << "i = " << i << std::endl;

    statusMessage.progress = (double)i / (double)nVolumes;
    this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

    vtkMRMLScalarVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::New();
    vtkMRMLVolumeArchetypeStorageNode*storageNode = vtkMRMLVolumeArchetypeStorageNode::New();
    vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();

    //storageNode->SetFileName(filename);
    char nodeName[128];
    std::cerr << "filename = " << fileNamesContainerList[i][0].c_str() << std::endl;
    ReaderType::FileNamesContainer::iterator fnciter;
    storageNode->SetFileName(fileNamesContainerList[i][0].c_str());
    storageNode->ResetFileNameList();
    for (fnciter = fileNamesContainerList[i].begin(); fnciter != fileNamesContainerList[i].end(); fnciter ++)
      {
      storageNode->AddFileName(fnciter->c_str());
      }
    storageNode->SetSingleFile(0);
    //storageNode->AddObserver(vtkCommand::ProgressEvent,  this->LogicCallbackCommand);
    storageNode->ReadData(volumeNode);

    sprintf(nodeName, "Vol_%03d", i);
    volumeNode->SetName(nodeName);

    volumeNode->SetScene(scene);
    storageNode->SetScene(scene);
    displayNode->SetScene(scene);

    double range[2];
    volumeNode->GetImageData()->GetScalarRange(range);
    /*
      range[0] = 0.0;
      range[1] = 256.0;
    */
    if (range[0] < rangeLower) rangeLower = range[0];
    if (range[1] > rangeUpper) rangeUpper = range[1];
    displayNode->SetAutoWindowLevel(0);
    displayNode->SetAutoThreshold(0);
    displayNode->SetLowerThreshold(range[0]);
    displayNode->SetUpperThreshold(range[1]);
    displayNode->SetWindow(range[1] - range[0]);
    displayNode->SetLevel(0.5 * (range[1] + range[0]) );
    vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
    displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
    colorLogic->Delete();

    scene->AddNode(displayNode);  
    scene->AddNode(storageNode);  
    volumeNode->SetAndObserveStorageNodeID(storageNode->GetID());
    volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
    scene->AddNode(volumeNode);  
    this->FrameNodeVector.push_back(std::string(volumeNode->GetID()));

    // Add to 4D bundle
    volumeNode->SetAndObserveTransformNodeID(bundleNode->GetID());
    volumeNode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    bundleNode->AddFrame(volumeNode->GetID());

    volumeNode->Delete();
    storageNode->Delete();
    displayNode->Delete();
    }

  AddDisplayBufferNode(bundleNode, 0);
  AddDisplayBufferNode(bundleNode, 1);

  statusMessage.show = 0;
  this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);
  
  return bundleNode;
}


//---------------------------------------------------------------------------
int vtkFourDAnalysisLogic::SaveImagesToDir(const char* path,
                                       const char* bundleID,
                                       const char* prefix,
                                       const char* suffix)
{
  vtkMRML4DBundleNode* bundleNode 
    = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

  if (!bundleNode)
    {
    return 0;
    }

  StatusMessageType statusMessage;
  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Writing Volumes....";
  this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

  int nFrames = bundleNode->GetNumberOfFrames();
  for (int i = 0; i < nFrames; i ++)
    {
    statusMessage.progress = (double)i / (double) nFrames;
    this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

    vtkMRMLScalarVolumeNode* inode
      = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(i));
    if (inode)
      {
      vtkSlicerApplication* app = this->GetApplication();
      vtkSlicerVolumesGUI* volumesGUI = 
        vtkSlicerVolumesGUI::SafeDownCast(app->GetModuleGUIByName ("Volumes"));
      vtkSlicerVolumesLogic* volumesLogic = volumesGUI->GetLogic(); 

      char fileName[256];
      sprintf(fileName, "%s/%s_%03d.%s", path, prefix, i, suffix);
      std::cerr << "writing " << fileName << std::endl;
      volumesLogic->SaveArchetypeVolume(fileName, inode);
      }
    }

  statusMessage.show = 0;
  this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

  return 1;
}


//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkFourDAnalysisLogic::AddDisplayBufferNode(vtkMRML4DBundleNode* bundleNode, int index)
{
  vtkMRMLScene* scene = this->GetMRMLScene();

  // Create Display Buffer Node
  vtkMRMLScalarVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::New();
  //vtkMRMLVolumeArchetypeStorageNode*storageNode = vtkMRMLVolumeArchetypeStorageNode::New();
  vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();
  

  volumeNode->SetScene(scene);
  //storageNode->SetScene(scene);
  displayNode->SetScene(scene);

  vtkImageData* imageData = vtkImageData::New();
  vtkMRMLScalarVolumeNode *firstFrameNode 
    = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(0));

  if (firstFrameNode && firstFrameNode->GetImageData())
    {
    volumeNode->Copy(firstFrameNode);
    volumeNode->SetAndObserveTransformNodeID(NULL);
    vtkImageData* firstImageData = firstFrameNode->GetImageData();
    imageData->ShallowCopy(firstImageData);
    }
  
  // J. Tokuda -- Jan 26, 2009: The display buffer node is placed outside
  // the bundle to allow the users to make a label map with the buffer image.
  volumeNode->SetAndObserveImageData(imageData);


  char nodeName[128];
  sprintf(nodeName, "%s_Display%d", bundleNode->GetName(), index);
  volumeNode->SetName(nodeName);
  
  double rangeLower;
  double rangeUpper;

  double range[2];
  volumeNode->GetImageData()->GetScalarRange(range);
  if (range[0] < rangeLower) rangeLower = range[0];
  if (range[1] > rangeUpper) rangeUpper = range[1];
  displayNode->SetAutoWindowLevel(0);
  displayNode->SetAutoThreshold(0);
  displayNode->SetLowerThreshold(range[0]);
  displayNode->SetUpperThreshold(range[1]);
  displayNode->SetWindow(range[1] - range[0]);
  displayNode->SetLevel(0.5 * (range[1] - range[0]) );
  vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
  displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
  colorLogic->Delete();
  scene->AddNode(displayNode);  
  //scene->AddNode(storageNode);  
  //volumeNode->SetAndObserveStorageNodeID(storageNode->GetID());
  volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  scene->AddNode(volumeNode);  
  //this->FrameNodeVector.push_back(std::string(volumeNode->GetID()));

  bundleNode->SetDisplayBufferNodeID(index, volumeNode->GetID());
  
  //volumeNode->Delete();
  //storageNode->Delete();
  displayNode->Delete();
  return volumeNode;

}


//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkFourDAnalysisLogic::AddMapVolumeNode(vtkMRML4DBundleNode* bundleNode,
                                                                 const char* nodeName)
{
  vtkMRMLScene* scene = this->GetMRMLScene();

  vtkMRMLScalarVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::New();
  vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();

  volumeNode->SetScene(scene);

  vtkImageData* imageData = vtkImageData::New();
  vtkMRMLScalarVolumeNode *firstFrameNode 
    = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(0));

  if (firstFrameNode && firstFrameNode->GetImageData())
    {
    vtkImageData* firstImageData = firstFrameNode->GetImageData();

    imageData->SetDimensions(firstImageData->GetDimensions());
    imageData->SetExtent(firstImageData->GetExtent());
    imageData->SetSpacing(firstImageData->GetSpacing());
    imageData->SetOrigin(firstImageData->GetOrigin());
    imageData->SetNumberOfScalarComponents(1);
    imageData->SetScalarTypeToFloat();  // Set to float to store parameters
    imageData->AllocateScalars();
    }

  volumeNode->SetAndObserveImageData(imageData);
  volumeNode->SetName(nodeName);
  vtkMatrix4x4* mat = vtkMatrix4x4::New();
  firstFrameNode->GetIJKToRASMatrix(mat);
  volumeNode->SetIJKToRASMatrix(mat);
  mat->Delete();

  scene->SaveStateForUndo();
  volumeNode->SetScene(scene);
  volumeNode->SetDescription("Created by 4D Analysis Module");
  
  double range[2];
  vtkDebugMacro("Set basic display info");
  //volumeNode->GetImageData()->GetScalarRange(range);
  range[0] = 0.0;
  range[1] = 256.0;
  displayNode->SetAutoWindowLevel(0);
  displayNode->SetAutoThreshold(0);
  displayNode->SetLowerThreshold(range[0]);
  displayNode->SetUpperThreshold(range[1]);
  displayNode->SetWindow(range[1] - range[0]);
  displayNode->SetLevel(0.5 * (range[1] + range[0]) );
  
  vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
  displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
  displayNode->SetScene(scene);
  scene->AddNode(displayNode);  
  scene->AddNode(volumeNode);  

  volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());

  //volumeNode->Delete();
  //storageNode->Delete();
  //displayNode->Delete();
  //colorLogic->Delete();

  volumeNode->Modified();
  volumeNode->GetImageData()->Modified();

  return volumeNode;
}




//---------------------------------------------------------------------------
int vtkFourDAnalysisLogic::GenerateBundleFrames(vtkMRML4DBundleNode* inputBundleNode,
                                                vtkMRML4DBundleNode* outputBundleNode)
{
  int nfInput  = inputBundleNode->GetNumberOfFrames();
  int nfOutput = outputBundleNode->GetNumberOfFrames();

  if (nfInput == nfOutput)
    {
    // nothing to do
    return 0;
    }

  if (nfOutput > nfInput)
    {
    for (int i = nfOutput-1; i >= nfInput; i --)
      {
      outputBundleNode->RemoveFrame(i);
      }
    return -1;
    }

  // need to create new frames

  int nVolumes = nfInput - nfOutput;

  StatusMessageType statusMessage;
  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Allocating image nodes....";
  this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

  vtkMRMLScene* scene = this->GetMRMLScene();
  for (int i = 0; i < nVolumes; i ++)
    {
    statusMessage.progress = (double)i / (double)nVolumes;
    this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

    vtkMRMLScalarVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::New();
    vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();
    //vtkMRMLVolumeArchetypeStorageNode*storageNode = vtkMRMLVolumeArchetypeStorageNode::New();
    char nodeName[128];

    sprintf(nodeName, "RegVol_%03d", i);
    vtkMRMLScalarVolumeNode *inFrameNode 
      = vtkMRMLScalarVolumeNode::SafeDownCast(inputBundleNode->GetFrameNode(nfOutput+i));
    if (inFrameNode)
      {
      volumeNode->Copy(inFrameNode);
      vtkImageData* inImageData = inFrameNode->GetImageData();
      vtkImageData* imageData = vtkImageData::New();
      imageData->DeepCopy(inImageData);
      volumeNode->SetAndObserveImageData(imageData);
      }
    volumeNode->SetName(nodeName);
    volumeNode->SetScene(scene);
    displayNode->SetScene(scene);

    vtkImageData* imageData = vtkImageData::New();
    volumeNode->SetAndObserveImageData(imageData);
    
    vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
    displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
    
    //scene->AddNode(storageNode);  
    scene->AddNode(displayNode);  
    volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
    scene->AddNode(volumeNode);  
    //this->RegisteredFrameNodeVector.push_back(std::string(volumeNode->GetID()));

    volumeNode->SetAndObserveTransformNodeID(outputBundleNode->GetID());
    volumeNode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    outputBundleNode->AddFrame(volumeNode->GetID());
    
    volumeNode->Delete();
    colorLogic->Delete();
    displayNode->Delete();
    }

  if (!outputBundleNode->GetDisplayBufferNode(0))
    {
    AddDisplayBufferNode(outputBundleNode, 0);
    }
  if (!outputBundleNode->GetDisplayBufferNode(1))
    {
    AddDisplayBufferNode(outputBundleNode, 1);
    }

  statusMessage.show = 0;
  this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);
  
  return 1;
}


//---------------------------------------------------------------------------
int vtkFourDAnalysisLogic::GetNumberOfFrames()
{
  return this->FrameNodeVector.size();
}

//---------------------------------------------------------------------------
const char* vtkFourDAnalysisLogic::GetFrameNodeID(int index)
{
  if (index >= 0 && index < (int)this->FrameNodeVector.size())
    {
    return this->FrameNodeVector[index].c_str();
    }
  else
    {
    return NULL;
    }
}

//---------------------------------------------------------------------------
const char* vtkFourDAnalysisLogic::GetRegisteredFrameNodeID(int index)
{
  if (index >= 0 && index < (int)this->RegisteredFrameNodeVector.size())
    {
    return this->RegisteredFrameNodeVector[index].c_str();
    }
  else
    {
    return NULL;
    }
}


//---------------------------------------------------------------------------
int vtkFourDAnalysisLogic::SaveIntensityCurves(vtkIntensityCurves* curves, const char* fileNamePrefix)
{
  if (!curves)
    {
    return 0;
    }

  vtkIntArray* labels = curves->GetLabelList();
  int nLabels = labels->GetNumberOfTuples();
  for (int i = 0; i < nLabels; i ++)
    {
    int label = labels->GetValue(i);
    char filename[256];
    sprintf (filename, "%s_%03d.csv", fileNamePrefix, label);
    
    std::ofstream fout;
    fout.open(filename, std::ios::out);
    if (fout.fail())
      {
      vtkErrorMacro ("vtkFourDAnalysisLogic: could not open file " << filename );
      return 0;
      }
    curves->OutputDataInCSV(fout, label);
    fout.close();
    }
  
  return 1;
  
}

//---------------------------------------------------------------------------
void vtkFourDAnalysisLogic::RunCurveFitting(const char* script, vtkMRMLCurveAnalysisNode* curveNode)
{
  PyObject* v;
  std::string pythonCmd;

  // Obtain MRML CurveAnalysis Node instance
  pythonCmd += "from Slicer import slicer\n";
  pythonCmd += "scene = slicer.MRMLScene\n";
  pythonCmd += "curveNode  = scene.GetNodeByID('";
  pythonCmd += curveNode->GetID();
  pythonCmd += "')\n";

  // Load 4D Analysis Python Module
  pythonCmd += "import imp\n";
  pythonCmd += "fp, pathname, description = imp.find_module('FourDAnalysis')\n";
  pythonCmd += "try:\n";
  pythonCmd += "    fda = imp.load_module('FourDAnalysis', fp, pathname, description)\n";
  pythonCmd += "finally:\n";
  pythonCmd += "    if fp:\n";
  pythonCmd += "        fp.close()\n";

  // Get input and output curves from MRML node
  pythonCmd += "inputCurve  = curveNode.GetSourceData().ToArray()\n";
  pythonCmd += "outputCurve = curveNode.GetFittedData().ToArray()\n";
  pythonCmd += "caexec = fda.CurveAnalysisExecuter('";
  pythonCmd += script;
  pythonCmd += "')\n";

  // Run curve fitting
  pythonCmd += "result = caexec.Execute(inputCurve, outputCurve)\n";

  v = PyRun_String(pythonCmd.c_str(),
                   Py_file_input,
                   (PyObject*)(vtkSlicerApplication::GetInstance()->GetPythonDictionary()),
                   (PyObject*)(vtkSlicerApplication::GetInstance()->GetPythonDictionary()));
  
  if (Py_FlushLine())
    {
      PyErr_Clear();
    }

}


//---------------------------------------------------------------------------
void vtkFourDAnalysisLogic::GenerateParameterMap(const char* script,
                                                 vtkMRML4DBundleNode* bundleNode,
                                                 const char* outputNodeNamePrefix,
                                                 int start, int end)
{
  // Add a new vtkMRMLCurveAnalysisNode to the MRML scene
  vtkMRMLCurveAnalysisNode* curveNode = vtkMRMLCurveAnalysisNode::New();
  vtkMRMLScene* scene = this->GetMRMLScene();
  this->GetMRMLScene()->AddNode(curveNode);

  // Prepare vtkDoubleArray to pass the source cueve data
  vtkDoubleArray* srcCurve = vtkDoubleArray::New();
  srcCurve->SetNumberOfComponents(2);

  vtkDoubleArray* fittedCurve = vtkDoubleArray::New();
  fittedCurve->SetNumberOfComponents(2);
  
  curveNode->SetSourceData(srcCurve);
  curveNode->SetFittedData(fittedCurve);

  if (!bundleNode)
    {
    return;
    }

  int nFrames = bundleNode->GetNumberOfFrames();  
  
  // ----------------------------------------
  // Setup the executer
  std::cerr << "Setting up the executer" << std::endl;
  PyObject* v;
  std::string pythonCmd;
  
  // Obtain MRML CurveAnalysis Node instance
  pythonCmd += "from Slicer import slicer\n";
  pythonCmd += "scene = slicer.MRMLScene\n";
  pythonCmd += "curveNode  = scene.GetNodeByID('";
  pythonCmd += curveNode->GetID();
  pythonCmd += "')\n";
  
  // Load 4D Analysis Python Module
  pythonCmd += "import imp\n";
  pythonCmd += "fp, pathname, description = imp.find_module('FourDAnalysis')\n";
  pythonCmd += "try:\n";
  pythonCmd += "    fda = imp.load_module('FourDAnalysis', fp, pathname, description)\n";
  pythonCmd += "finally:\n";
  pythonCmd += "    if fp:\n";
  pythonCmd += "        fp.close()\n";
  pythonCmd += "caexec = fda.CurveAnalysisExecuter('";
  pythonCmd += script;
  pythonCmd += "')\n";
  
  // Get output parameter list
  std::cerr << "Get output parameter list" << std::endl;
  pythonCmd += "parameters = caexec.GetOutputParameterNames()\n";
  pythonCmd += "for key in parameters:\n";
  pythonCmd += "    curveNode.SetParameter(key, 0.0)\n";

  v = PyRun_String(pythonCmd.c_str(),
                   Py_file_input,
                   (PyObject*)(vtkSlicerApplication::GetInstance()->GetPythonDictionary()),
                   (PyObject*)(vtkSlicerApplication::GetInstance()->GetPythonDictionary()));
  if (Py_FlushLine())
    {
    PyErr_Clear();
    }
  
  vtkStringArray* nameArray = curveNode->GetParameterNameArray();
  
  // Create map volumes for each parameter
  int numKeys = nameArray->GetNumberOfTuples();
  typedef std::map<std::string, vtkImageData*> ParameterImageMapType;
  typedef std::map<std::string, vtkMRMLScalarVolumeNode*> ParameterVolumeNodeMapType;
  ParameterImageMapType ParameterImages;
  ParameterVolumeNodeMapType ParameterImageNodes;
  for (int i = 0; i < numKeys; i ++)
    {
    char  nodeName[256];
    const char* paramName = nameArray->GetValue(i);
    sprintf(nodeName, "%s_%s", outputNodeNamePrefix, paramName);
    std::cerr << "Creating " << nodeName << std::endl;
    vtkMRMLScalarVolumeNode* node = AddMapVolumeNode(bundleNode, nodeName);
    ParameterImages[paramName] = node->GetImageData();
    ParameterImageNodes[paramName] = node;
    }
  
  // Check the index range
  int max = nFrames;
  if (start < 0)   start = 0;
  if (end >= max)  end   = max-1;
  if (start > end) start = end;
  
  // Make an array of vtkImageData
  int nSrcPoints = end - start + 1;
  vtkImageData* imageArray[nSrcPoints];
  for (int i = start; i <= end; i ++)
    {
    vtkMRMLScalarVolumeNode* node = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(i));
    imageArray[i-start] = node->GetImageData();
    std::cerr << "Listing image data: " << node->GetName() << std::endl;
    }
  
  int* dim = imageArray[0]->GetDimensions();
  int x = dim[0]; int y = dim[1]; int z = dim[2];

  srcCurve->SetNumberOfTuples(nSrcPoints);
  fittedCurve->SetNumberOfTuples(0);

  // Run Map generation
  //  -- prepare python commands 
  pythonCmd  = "";
  pythonCmd += "inputCurve  = curveNode.GetSourceData().ToArray()\n";
  pythonCmd += "outputCurve = curveNode.GetFittedData().ToArray()\n";

  v = PyRun_String(pythonCmd.c_str(),
                   Py_file_input,
                   (PyObject*)(vtkSlicerApplication::GetInstance()->GetPythonDictionary()),
                   (PyObject*)(vtkSlicerApplication::GetInstance()->GetPythonDictionary()));
  if (Py_FlushLine())
    {
    PyErr_Clear();
    }
  
  pythonCmd  = "";
  pythonCmd += "result      = caexec.Execute(inputCurve, outputCurve)\n";
  pythonCmd += "for key, value in result.iteritems():\n";
  pythonCmd += "    curveNode.SetParameter(key, value)\n";
  pythonCmd += "\n";
  
  for (int k = 0; k < z; k ++)
    {
    std::cerr << std::endl;
    std::cerr << "Processing Slice k = " << k << std::endl;
    for (int j = 0; j < y; j ++)
      {
      std::cerr << "    Processing line j = " << j << std::endl;
      for (int i = 0; i < x; i ++)
        {
        // Copy intensity data
        for (int t = 0; t < nSrcPoints; t ++)
          {
          double xy[2];
          xy[0] = (double) t + start;
          xy[1] = imageArray[t]->GetScalarComponentAsDouble(i, j, k, 0);
          srcCurve->SetTuple(t, xy);
          }

        // Run curve fitting
        v = PyRun_String(pythonCmd.c_str(),
                         Py_file_input,
                         (PyObject*)(vtkSlicerApplication::GetInstance()->GetPythonDictionary()),
                         (PyObject*)(vtkSlicerApplication::GetInstance()->GetPythonDictionary()));
        if (Py_FlushLine())
          {
          PyErr_Clear();
          }

        // Put results
        ParameterImageMapType::iterator iter;
        for (iter = ParameterImages.begin(); iter != ParameterImages.end(); iter ++)
          {
          float param = (float)curveNode->GetParameter(iter->first.c_str());
          if (!std::isnormal(param))
            {
            param = 0.0;
            }
          std::cerr << param << ", ";
          iter->second->SetScalarComponentFromFloat(i, j, k, 0, param);
          }
        std::cerr << std::endl;
        }
      }
    //std::cerr << std::endl;
    }
  
  // Put results
  ParameterVolumeNodeMapType::iterator iter;
  for (iter = ParameterImageNodes.begin(); iter != ParameterImageNodes.end(); iter ++)
    {
    double range[2];
    vtkImageData* imageData = iter->second->GetImageData();
    vtkMRMLScalarVolumeDisplayNode* displayNode 
      = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(iter->second->GetDisplayNode());
    imageData->Update();
    imageData->GetScalarRange(range);
    std::cerr << "range = (" << range[0] << ", " << range[1] << ")" << std::endl;
    displayNode->SetAutoWindowLevel(0);
    displayNode->SetAutoThreshold(0);
    displayNode->SetLowerThreshold(range[0]);
    displayNode->SetUpperThreshold(range[1]);
    displayNode->SetWindow(range[1] - range[0]);
    displayNode->SetLevel(0.5 * (range[1] + range[0]));
    }
  
  std::cerr << "END " << std::endl;
}


//---------------------------------------------------------------------------
int vtkFourDAnalysisLogic::RunSeriesRegistration(int sIndex, int eIndex, int kIndex, 
                                             const char* inputBundleNodeID,
                                             const char* outputBundleNodeID,
                                             RegistrationParametersType& param)
{
  vtkMRML4DBundleNode* inputBundleNode 
    = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(inputBundleNodeID));

  if (!inputBundleNode)
    {
    std::cerr << "int vtkFourDAnalysisGUI::RunSeriesRegistration(): Irregular input bundle" << std::endl;
    return 0;
    }
  if (!inputBundleNode ||
      sIndex < 0 ||
      eIndex >= inputBundleNode->GetNumberOfFrames() ||
      kIndex < 0 ||
      kIndex >= inputBundleNode->GetNumberOfFrames() )
    {
    std::cerr << "int vtkFourDAnalysisGUI::RunSeriesRegistration(): irregular index" << std::endl;
    return 0;
    }
  
  vtkMRML4DBundleNode* outputBundleNode 
    = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(outputBundleNodeID));
  if (!outputBundleNode)
    {
    std::cerr << "int vtkFourDAnalysisGUI::RunSeriesRegistration(): Irregular output bundle" << std::endl;
    return 0;
    }
  
  if (outputBundleNode->GetNumberOfFrames() != inputBundleNode->GetNumberOfFrames())
    {
    GenerateBundleFrames(inputBundleNode, outputBundleNode);
    }
  
  vtkMRMLScalarVolumeNode* fixedNode = 
    vtkMRMLScalarVolumeNode::SafeDownCast(inputBundleNode->GetFrameNode(kIndex));

  if (!fixedNode)
    {
    std::cerr << "int vtkFourDAnalysisGUI::RunSeriesRegistration(): no fixed frame node ID found." << std::endl;
    }
 
  StatusMessageType statusMessage;
  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Running image registration ....";
  this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

  for (int i = sIndex; i <= eIndex; i++)
    {
    statusMessage.progress = (double)(i-sIndex) / (double)(eIndex-sIndex + 1);
    this->InvokeEvent ( vtkFourDAnalysisLogic::ProgressDialogEvent, &statusMessage);

    vtkMRMLScalarVolumeNode* movingNode = 
      vtkMRMLScalarVolumeNode::SafeDownCast(inputBundleNode->GetFrameNode(i));
    vtkMRMLScalarVolumeNode* outputNode =
      vtkMRMLScalarVolumeNode::SafeDownCast(outputBundleNode->GetFrameNode(i));

    if (movingNode && outputNode)
      {
      RunRegistration(inputBundleNode, fixedNode, movingNode, outputNode, param);
      }
    }

  return 1;

}
  

//---------------------------------------------------------------------------
int vtkFourDAnalysisLogic::RunRegistration(vtkMRML4DBundleNode* bundleNode,
                                       vtkMRMLScalarVolumeNode* fixedNode,
                                       vtkMRMLScalarVolumeNode* movingNode,
                                       vtkMRMLScalarVolumeNode* outputNode,
                                       RegistrationParametersType& param)
{
  vtkCommandLineModuleGUI* cligui;

  vtkSlicerApplication* app = this->GetApplication();
  cligui = vtkCommandLineModuleGUI::SafeDownCast(app->GetModuleGUIByName ("Deformable BSpline registration"));
  //cligui = vtkCommandLineModuleGUI::SafeDownCast(app->GetModuleGUIByName ("Linear registration"));

  if (cligui)
    {
    cligui->Enter();
    vtkMRMLCommandLineModuleNode* node = 
      static_cast<vtkMRMLCommandLineModuleNode*>(this->GetMRMLScene()->CreateNodeByClass("vtkMRMLCommandLineModuleNode"));
    if(!node)
      {
      std::cerr << "Cannot create Rigid registration node." << std::endl;
      }
    this->GetMRMLScene()->AddNode(node);
    //tensorCLM->SetModuleDescription("Diffusion Tensor Estimation");
    //tensorCLM->SetName("GradientEditor: Tensor Estimation");
    node->SetModuleDescription(cligui->GetModuleDescription());  // this is very important !!!
    node->SetName("Deformable BSpline registration");

    //if(outputNode)
    //  {
    //  this->GetMRMLScene()->RemoveNode(outputNode);
    //  }
    
    // Create output transform node
    vtkMRMLBSplineTransformNode *transformNode =
      vtkMRMLBSplineTransformNode::New();
    //assert(transformNode);
    char name[128];
    sprintf(name, "BSpline%s", movingNode->GetName());
    //std::cerr << "Transform = " << name << std::endl;
    transformNode->SetName(name);
    transformNode->SetAndObserveTransformNodeID(bundleNode->GetID());
    this->GetMRMLScene()->AddNode(transformNode);
    
    RegistrationParametersType::iterator iter;
    for (iter = param.begin(); iter != param.end(); iter ++)
      {
      node->SetParameterAsString(iter->first.c_str(), iter->second.c_str());
      }
    //node->SetModuleDescription( this->ModuleDescriptionObject );
    /*
    node->SetParameterAsInt("Iterations", 20);
    node->SetParameterAsInt("gridSize", 5);  
    node->SetParameterAsInt("HistogramBins", 100); 
    node->SetParameterAsInt("SpatialSamples", 5000); 
    node->SetParameterAsBool("ConstrainDeformation", 0); 
    node->SetParameterAsDouble("MaximumDeformation", 1.0);
    */

    node->SetParameterAsInt("DefaultPixelValue", 0); 
    //node->SetParameterAsString("InitialTransform", NULL);
    node->SetParameterAsString("FixedImageFileName", fixedNode->GetID());
    node->SetParameterAsString("MovingImageFileName", movingNode->GetID());
    node->SetParameterAsString("OutputTransform", transformNode->GetID());
    node->SetParameterAsString("ResampledImageFileName", outputNode->GetID());
    transformNode->Delete();

    cligui->GetLogic()->SetTemporaryDirectory(app->GetTemporaryDirectory());

    ModuleDescription moduleDesc = node->GetModuleDescription();
    if(moduleDesc.GetTarget() == "Unknown")
      {
      // Entry point is unknown
      // "Linear registration" is shared object module, at least at this moment
      //assert(moduleDesc.GetType() == "SharedObjectModule");
      typedef int (*ModuleEntryPoint)(int argc, char* argv[]);
      itksys::DynamicLoader::LibraryHandle lib =
        itksys::DynamicLoader::OpenLibrary(moduleDesc.GetLocation().c_str());
      if(lib)
        {
        ModuleEntryPoint entryPoint = 
          (ModuleEntryPoint) itksys::DynamicLoader::GetSymbolAddress(
                                                                     lib, "ModuleEntryPoint");
        if(entryPoint)
          {
          char entryPointAsText[256];
          std::string entryPointAsString;
          
          sprintf(entryPointAsText, "%p", entryPoint);
          entryPointAsString = std::string("slicer:")+entryPointAsText;
          moduleDesc.SetTarget(entryPointAsString);
          node->SetModuleDescription(moduleDesc);      
          } 
        else
          {
          std::cerr << "Failed to find entry point for Rigid registration. Abort." << std::endl;
          }
        } else {
      std::cerr << "Failed to locate module library. Abort." << std::endl;
      }
    }

    cligui->SetCommandLineModuleNode(node);
    cligui->GetLogic()->SetCommandLineModuleNode(node);

    std::cerr << "Starting Registration.... " << std::endl;    
    cligui->GetLogic()->ApplyAndWait(node);
    //cligui->GetLogic()->Apply();
    std::cerr << "Starting Registration.... done." << std::endl;    
    //this->SaveVolume(app, outputNode);
    node->Delete(); // AF: is it right to delete this here?
    //std::cerr << "Temp dir = " << ((vtkSlicerApplication*)this->GetApplication())->GetTemporaryDirectory() << std::endl;

    return 1;
    }
  else
    {
    std::cerr << "Couldn't find CommandLineModule !!!" << std::endl;
    return 0;
    }

}
