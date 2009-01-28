/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkSlicerColorLogic.h"
#include "vtkControl4DLogic.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
//#include "vtkMRMLVolumeHeaderlessStorageNode.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"
#include "vtkCommandLineModuleLogic.h"
#include "vtkCommandLineModuleGUI.h"

#include "itkOrientedImage.h"
#include "itkImageSeriesReader.h"
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


vtkCxxRevisionMacro(vtkControl4DLogic, "$Revision$");
vtkStandardNewMacro(vtkControl4DLogic);

//---------------------------------------------------------------------------
vtkControl4DLogic::vtkControl4DLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkControl4DLogic::DataCallback);

  this->FrameNodeVector.clear();

  this->CurveCache.clear();

  this->VolumeBundleID = "";
  this->RegisteredVolumeBundleID = "";

}


//---------------------------------------------------------------------------
vtkControl4DLogic::~vtkControl4DLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkControl4DLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkControl4DLogic:             " << this->GetClassName() << "\n";

}

//----------------------------------------------------------------------------
void vtkControl4DLogic::ProcessLogicEvents(vtkObject *caller, 
                                            unsigned long event, 
                                            void *callData)
{
  if (event ==  vtkCommand::ProgressEvent) 
    {
    this->InvokeEvent ( vtkCommand::ProgressEvent,callData );
    }
}

//---------------------------------------------------------------------------
void vtkControl4DLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkControl4DLogic *self = reinterpret_cast<vtkControl4DLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkControl4DLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkControl4DLogic::UpdateAll()
{
}


//---------------------------------------------------------------------------
int vtkControl4DLogic::LoadImagesFromDir(const char* path, double& rangeLower, double& rangeUpper)
{
  std::cerr << "loading from " << path << std::endl;

  const int SpaceDim = 3;
  typedef short PixelValueType;
  typedef itk::OrientedImage< PixelValueType, 2 > SliceType;
  typedef itk::ImageFileReader< SliceType > SliceReaderType;
  typedef itk::OrientedImage< PixelValueType, SpaceDim > VolumeType;
  typedef itk::ImageSeriesReader< VolumeType > ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
  typedef itk::ImageFileWriter< VolumeType >  WriterType;
  typedef itk::VectorImage< PixelValueType, SpaceDim > NRRDImageType;
  
  StatusMessageType statusMessage;
  
  //itk::TimeProbesCollectorBase collector;

  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Checking directory....";
  this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);
  
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  gdcmIO->LoadPrivateTagsOn();

  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetUseSeriesDetails(true);
  inputNames->SetDirectory(path);

  itk::SerieUIDContainer seriesUIDs = inputNames->GetSeriesUIDs();
  int nVolumes;
  std::vector<ReaderType::FileNamesContainer> fileNamesContainerList;

  if (seriesUIDs.size() == 1)  // if single series UID is used
    {
    statusMessage.message = "Splitting series....";
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

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
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

    for (int i = 1; i < nSlices; i ++)
      {
      statusMessage.progress = (double)i/(double)nSlices;
      this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);
      
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
      
  //std::vector<ReaderType::FileNamesContainer>::iterator fnciter;
  //for (fnciter = fileNamesContainerList.begin(); fnciter != fileNamesContainerList.end(); fnciter ++)

  //vtkMRMLScene* scene = vtkMRMLScene::New();
  vtkMRMLScene* scene = this->GetMRMLScene();
  scene->SaveStateForUndo();
  this->FrameNodeVector.clear();

  statusMessage.progress = 0.0;
  statusMessage.message = "Reading Volumes....";
  this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

  rangeLower = 0.0;
  rangeUpper = 0.0;

  // prepare 4D bundle
  vtkMRML4DBundleNode* bundleNode = vtkMRML4DBundleNode::New();
  //vtkMRMLLinearTransformNode* bundleNode = vtkMRMLLinearTransformNode::New();
  bundleNode->SetName("4DBundle");
  bundleNode->SetDescription("Created by Control4D");

  vtkMatrix4x4* transform = vtkMatrix4x4::New();
  transform->Identity();
  bundleNode->ApplyTransform(transform);
  transform->Delete();
  scene->AddNode(bundleNode);

  for (int i = 0; i < nVolumes; i ++)
    {
    statusMessage.progress = (double)i / (double)nVolumes;
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

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
    displayNode->SetLevel(0.5 * (range[1] - range[0]) );
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
    std::cerr << "calling volumeNode->SetAndObserveTransformNodeID(bundleNode->GetID());" << std::endl;

    volumeNode->SetAndObserveTransformNodeID(bundleNode->GetID());
    volumeNode->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    std::cerr << "calling volumeNode->SetAndObserveTransformNodeID() -- end" << std::endl;
    bundleNode->AddFrame(volumeNode->GetID());

    volumeNode->Delete();
    storageNode->Delete();
    displayNode->Delete();
    }

  AddDisplayBufferNode(bundleNode, 0);
  AddDisplayBufferNode(bundleNode, 1);

  statusMessage.show = 0;
  this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);
  
  return nVolumes;
}


//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkControl4DLogic::AddDisplayBufferNode(vtkMRML4DBundleNode* bundleNode, int index)
{
  vtkMRMLScene* scene = this->GetMRMLScene();

  // Create Display Buffer Node
  vtkMRMLScalarVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::New();
  //vtkMRMLVolumeArchetypeStorageNode*storageNode = vtkMRMLVolumeArchetypeStorageNode::New();
  vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();
  

  std::cerr << "SetScene() " << std::endl;
  volumeNode->SetScene(scene);
  //storageNode->SetScene(scene);
  displayNode->SetScene(scene);

  vtkImageData* imageData = vtkImageData::New();
  vtkMRMLScalarVolumeNode *firstFrameNode 
    = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(0));

  if (firstFrameNode && firstFrameNode->GetImageData())
    {
    volumeNode->Copy(firstFrameNode);
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
  
  volumeNode->Delete();
  //storageNode->Delete();
  displayNode->Delete();

}


//---------------------------------------------------------------------------
int vtkControl4DLogic::GenerateBundleFrames(vtkMRML4DBundleNode* inputBundleNode,
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
  this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

  vtkMRMLScene* scene = this->GetMRMLScene();
  for (int i = 0; i < nVolumes; i ++)
    {
    statusMessage.progress = (double)i / (double)nVolumes;
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

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
  this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

}


//---------------------------------------------------------------------------
int vtkControl4DLogic::GetNumberOfFrames()
{
  return this->FrameNodeVector.size();
}

//---------------------------------------------------------------------------
const char* vtkControl4DLogic::GetFrameNodeID(int index)
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
const char* vtkControl4DLogic::GetRegisteredFrameNodeID(int index)
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
double vtkControl4DLogic::GetMeanIntensity(vtkImageData* image, IndexTableType& indexTable)
{
  double sum = 0.0;
  
  IndexTableType::iterator iter;
  for (iter = indexTable.begin(); iter != indexTable.end(); iter ++)
    {
    sum += image->GetScalarComponentAsDouble(iter->x, iter->y, iter->z, 0);
    }

  double mean = sum / (double)indexTable.size();

  return mean;
}


double vtkControl4DLogic::GetSDIntensity(vtkImageData* image, double mean, IndexTableType& indexTable)
{
  double s = 0.0;
  double n = (double) indexTable.size();

  IndexTableType::iterator iter;
  for (iter = indexTable.begin(); iter != indexTable.end(); iter ++)
    {
    double i = image->GetScalarComponentAsDouble(iter->x, iter->y, iter->z, 0);
    s += (i - mean)*(i - mean);
    }
  double sigma = sqrt(s/n);
  return sigma;
}


//---------------------------------------------------------------------------
void vtkControl4DLogic::GenerateIndexTable(vtkImageData* mask, int label, IndexTableType& indexTable)
{
  int maskDim[3];

  indexTable.clear();
  mask->GetDimensions(maskDim);
  int x = maskDim[0];
  int y = maskDim[1];
  int z = maskDim[2];

  for (int i = 0; i < x; i ++)
    {
    for (int j = 0; j < y; j ++)
      {
      for (int k = 0; k < z; k ++)
        {
        if ((int)mask->GetScalarComponentAsDouble(i, j, k, 0) == label)
          {
          CoordType index;
          index.x = i;
          index.y = j;
          index.z = k;
          indexTable.push_back(index);
          }
        }
      }
    }
}


//---------------------------------------------------------------------------
vtkDoubleArray* vtkControl4DLogic::GetIntensityCurve(const char* bundleID,
                                                     const char* maskID,
                                                     int label, int type)
{
  vtkMRML4DBundleNode* bundleNode 
    = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

  vtkMRMLScalarVolumeNode* mnode =
    vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(maskID));

  if (!bundleNode || !mnode)
    {
    return NULL;
    }

  // Get the cache
  CurveCacheType::iterator iter = this->CurveCache.find(bundleID);
  if (iter == this->CurveCache.end())
    {
    CurveDataSetType curveDataSet;
    this->CurveCache[bundleID] = curveDataSet;
    this->CurveCache[bundleID].MaskNodeID = maskID;
    }
  else if (iter->second.MaskNodeID != maskID)
    {
    iter->second.MaskNodeID = maskID;
    iter->second.CurveList.clear(); // MEMO: should we delete Means and SDs before clear the list?
    }

  if (this->CurveCache[bundleID].CurveList.find(label) == this->CurveCache[bundleID].CurveList.end())
    {
    CurveDataType curveData;
    this->CurveCache[bundleID].CurveList[label] = curveData;
    this->CurveCache[bundleID].CurveList[label].MaskModifiedTime = 0;
    this->CurveCache[bundleID].CurveList[label].Mean = vtkDoubleArray::New();
    this->CurveCache[bundleID].CurveList[label].SD = vtkDoubleArray::New();
    }

  std::cerr << "Modified time = " << mnode->GetMTime() << std::endl;
  if (this->CurveCache[bundleID].CurveList[label].MaskModifiedTime < mnode->GetMTime())
    {
    // generate the curve
    this->CurveCache[bundleID].CurveList[label].MaskModifiedTime = mnode->GetMTime();
    
    IndexTableType indexTable;
    GenerateIndexTable(mnode->GetImageData(), label, indexTable);
    vtkDoubleArray* meanArray = this->CurveCache[bundleID].CurveList[label].Mean;
    vtkDoubleArray* sdArray = this->CurveCache[bundleID].CurveList[label].SD;
    meanArray->SetNumberOfValues(0);
    sdArray->SetNumberOfValues(0);

    int nFrames = bundleNode->GetNumberOfFrames();
    for (int i = 0; i < nFrames; i ++)
      {
      vtkMRMLScalarVolumeNode* inode
        = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(i));
      if (inode)
        {
        double meanvalue = 0.0;
        double sdvalue = 0.0;
        meanvalue = GetMeanIntensity(inode->GetImageData(), indexTable);
        sdvalue   = GetSDIntensity(inode->GetImageData(), meanvalue, indexTable);
        std::cerr << "mean = " << meanvalue << ", sd = " << sdvalue << std::endl;
        meanArray->InsertNextValue(meanvalue);
        sdArray->InsertNextValue(sdvalue);
        }
      }
    }

  if (type == TYPE_MEAN)
    {
    return this->CurveCache[bundleID].CurveList[label].Mean;
    }
  else // type == TYPE_SD
    {
    return this->CurveCache[bundleID].CurveList[label].SD;
    }
  
}


//---------------------------------------------------------------------------
int vtkControl4DLogic::SaveIntensityCurve(const char* bundleID, const char* maskID, int label, const char* filename)
{
  vtkDoubleArray* meanArray = GetIntensityCurve(bundleID, maskID, label, TYPE_MEAN);
  vtkDoubleArray* sdArray = GetIntensityCurve(bundleID, maskID, label, TYPE_SD);

  if (!meanArray || !sdArray)
    {
    vtkErrorMacro ("vtkControl4DLogic: could not generate intensity curve. ");
    return 0;
    }

  std::ofstream fout;
  fout.open(filename, std::ios::out);

  if ( fout.fail() )
    {
    vtkErrorMacro ("vtkControl4DLogic: could not open file " << filename );
    return 0;
    }

  int n = meanArray->GetNumberOfTuples();
  for (int i = 0; i < n; i ++)
    {
    fout << i << ", " << meanArray->GetValue(i) << ", " << sdArray->GetValue(i) << std::endl;
    }

  fout.close();

  return 1;

}


//---------------------------------------------------------------------------
int vtkControl4DLogic::RunSeriesRegistration(int sIndex, int eIndex, int kIndex, 
                                             const char* inputBundleNodeID,
                                             const char* outputBundleNodeID,
                                             RegistrationParametersType& param)
{
  vtkMRML4DBundleNode* inputBundleNode 
    = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(inputBundleNodeID));

  if (!inputBundleNode)
    {
    std::cerr << "int vtkControl4DGUI::RunSeriesRegistration(): Irregular input bundle" << std::endl;
    return 0;
    }
  if (!inputBundleNode ||
      sIndex < 0 ||
      eIndex >= inputBundleNode->GetNumberOfFrames() ||
      kIndex < 0 ||
      kIndex >= inputBundleNode->GetNumberOfFrames() )
    {
    std::cerr << "int vtkControl4DGUI::RunSeriesRegistration(): irregular index" << std::endl;
    return 0;
    }
  
  vtkMRML4DBundleNode* outputBundleNode 
    = vtkMRML4DBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(outputBundleNodeID));
  if (!outputBundleNode)
    {
    std::cerr << "int vtkControl4DGUI::RunSeriesRegistration(): Irregular output bundle" << std::endl;
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
    std::cerr << "int vtkControl4DGUI::RunSeriesRegistration(): no fixed frame node ID found." << std::endl;
    }
 
  StatusMessageType statusMessage;
  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Running image registration ....";
  this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

  for (int i = sIndex; i <= eIndex; i++)
    {
    statusMessage.progress = (double)(i-sIndex) / (double)(eIndex-sIndex + 1);
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

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
int vtkControl4DLogic::RunRegistration(vtkMRML4DBundleNode* bundleNode,
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
    std::cerr << "Found CommandLineModule !!!" << std::endl;
    
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
    std::cerr << "Transform = " << name << std::endl;
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
