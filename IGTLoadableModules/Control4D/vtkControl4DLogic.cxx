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

#include "vtkMRMLBSplineTransformNode.h"
#include "itksys/DynamicLoader.hxx"


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

  this->IntensityCurveCache.clear();
  this->IntensitySDCurveCache.clear();
  this->RegisteredIntensityCurveCache.clear();
  this->RegisteredIntensitySDCurveCache.clear();

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

  for (int i = 0; i < nVolumes; i ++)
    {
    statusMessage.progress = (double)i / (double)nVolumes;
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

    vtkMRMLVolumeNode *volumeNode = NULL;
    vtkMRMLScalarVolumeNode *scalarNode = vtkMRMLScalarVolumeNode::New();
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
    storageNode->ReadData(scalarNode);

    volumeNode = scalarNode;
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

    scalarNode->Delete();
    storageNode->Delete();
    displayNode->Delete();
    }

  statusMessage.show = 0;
  this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);
  
  /*
  scene->SetURL( "test.mrml" );
  scene->Commit();
  scene->Delete();
  */
  return nVolumes;
}


//---------------------------------------------------------------------------
int vtkControl4DLogic::CreateRegisteredVolumeNodes()
{

  int nVolumes = this->FrameNodeVector.size();

  if (this->RegisteredFrameNodeVector.size() != nVolumes)
    {
    if (this->RegisteredFrameNodeVector.size() != 0)
      {
      this->RegisteredFrameNodeVector.clear();
      }

    StatusMessageType statusMessage;
    statusMessage.show = 1;
    statusMessage.progress = 0.0;
    statusMessage.message = "Creating registerd image nodes....";
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);
    
    vtkMRMLScene* scene = this->GetMRMLScene();
    for (int i = 0; i < nVolumes; i ++)
      {
      statusMessage.progress = (double)i / (double)nVolumes;
      this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

      vtkMRMLVolumeNode *volumeNode = NULL;
      vtkMRMLScalarVolumeNode *scalarNode = vtkMRMLScalarVolumeNode::New();
      vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();
      //vtkMRMLVolumeArchetypeStorageNode*storageNode = vtkMRMLVolumeArchetypeStorageNode::New();
      char nodeName[128];
      //std::cerr << "filename = " << fileNamesContainerList[i][0].c_str() << std::endl;
      //ReaderType::FileNamesContainer::iterator fnciter;
      //storageNode->SetFileName(fileNamesContainerList[i][0].c_str());
      //storageNode->ResetFileNameList();
      //for (fnciter = fileNamesContainerList[i].begin(); fnciter != fileNamesContainerList[i].end(); fnciter ++)
      //  {
      //  storageNode->AddFileName(fnciter->c_str());
      //  }
      //storageNode->SetSingleFile(0);
      ////storageNode->AddObserver(vtkCommand::ProgressEvent,  this->LogicCallbackCommand);
      //storageNode->ReadData(scalarNode);
      
      volumeNode = scalarNode;
      sprintf(nodeName, "RegVol_%03d", i);
      volumeNode->SetName(nodeName);
      volumeNode->SetScene(scene);
      //storageNode->SetScene(scene);
      displayNode->SetScene(scene);
      
      //double range[2];
      //volumeNode->GetImageData()->GetScalarRange(range);
      //range[0] = 0.0;
      //range[1] = 256.0;
      //displayNode->SetLowerThreshold(range[0]);
      //displayNode->SetUpperThreshold(range[1]);
      //displayNode->SetWindow(range[1] - range[0]);
      //displayNode->SetLevel(0.5 * (range[1] - range[0]) );
      vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
      displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
      
      //scene->AddNode(storageNode);  
      scene->AddNode(displayNode);  
      //volumeNode->SetAndObserveStorageNodeID(storageNode->GetID());
      volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
      scene->AddNode(volumeNode);  
      this->RegisteredFrameNodeVector.push_back(std::string(volumeNode->GetID()));
      
      scalarNode->Delete();
      //storageNode->Delete();
      colorLogic->Delete();
      displayNode->Delete();
      }
    statusMessage.show = 0;
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);
    }
  
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
double vtkControl4DLogic::GetMeanIntencity(vtkImageData* image)
{
  double sum = 0.0;

  IndexTableType::iterator iter;
  for (iter = this->MaskIndexTable.begin(); iter != this->MaskIndexTable.end(); iter ++)
    {
    sum += image->GetScalarComponentAsDouble(iter->x, iter->y, iter->z, 0);
    }

  double mean = sum / (double)this->MaskIndexTable.size();

  return mean;
}


double vtkControl4DLogic::GetSDIntencity(vtkImageData* image, double mean)
{
  double s = 0.0;
  double n = (double) MaskIndexTable.size();

  IndexTableType::iterator iter;
  for (iter = this->MaskIndexTable.begin(); iter != this->MaskIndexTable.end(); iter ++)
    {
    double i = image->GetScalarComponentAsDouble(iter->x, iter->y, iter->z, 0);
    s += (i - mean)*(i - mean);
    }
  double sigma = sqrt(s/n);
  return sigma;
}


//---------------------------------------------------------------------------
void vtkControl4DLogic::ClearIntensityCurveCache(const char* maskID)
{
  if (maskID == NULL)
    {
    this->IntensityCurveCache.clear();
    this->IntensitySDCurveCache.clear();
    this->RegisteredIntensityCurveCache.clear();
    this->RegisteredIntensitySDCurveCache.clear();

    return;
    }
  
  std::cerr << "void vtkControl4DLogic::ClearIntensityCurveCache(): clearing cache..." << std::endl;
  IntensityCurveCacheType::iterator icciter;

  icciter = this->IntensityCurveCache.find(maskID);
  if (icciter != this->IntensityCurveCache.end())
    {
    this->IntensityCurveCache.erase(icciter);
    }

  icciter = this->IntensitySDCurveCache.find(maskID);
  if (icciter != this->IntensitySDCurveCache.end())
    {
    this->IntensitySDCurveCache.erase(icciter);
    }

  icciter = this->RegisteredIntensityCurveCache.find(maskID);
  if (icciter != this->RegisteredIntensityCurveCache.end())
    {
    this->RegisteredIntensityCurveCache.erase(icciter);
    }

  icciter = this->RegisteredIntensitySDCurveCache.find(maskID);
  if (icciter != this->RegisteredIntensityCurveCache.end())
    {
    this->RegisteredIntensitySDCurveCache.erase(icciter);
    }
  

}


//---------------------------------------------------------------------------
void vtkControl4DLogic::GenerateIndexTable(vtkImageData* mask, int label)
{
  int maskDim[3];

  this->MaskIndexTable.clear();
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
          this->MaskIndexTable.push_back(index);
          }
        }
      }
    }
}

//---------------------------------------------------------------------------
vtkDoubleArray* vtkControl4DLogic::GetIntensityCurve(int series, const char* maskID, int label, int type)
{
  vtkMRMLScalarVolumeNode* mnode =
    vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(maskID));

  if (!mnode)
    {
    return NULL;
    }

  // Check cache to see if the intensity curve has already been generated.
  int newCurveSet = 0;
  IntensityCurveCacheType::iterator icciter;

  IntensityCurveCacheType* meanCache;
  IntensityCurveCacheType* sdCache;

  if (series == 0) // for original image
    {
    meanCache = &(this->IntensityCurveCache);
    sdCache   = &(this->IntensitySDCurveCache);
    }
  else
    {
    meanCache = &(this->RegisteredIntensityCurveCache);
    sdCache = &(this->RegisteredIntensitySDCurveCache);
    }

  if (type == TYPE_MEAN)
    {
    //icciter = this->IntensityCurveCache.find(maskID);
    icciter = meanCache->find(maskID);
    //if (icciter != this->IntensityCurveCache.end())
    if (icciter != meanCache->end())
      {
      IntensityCurveSetType::iterator icsiter = icciter->second.find(label);
      if (icsiter != icciter->second.end())
        {
        return icsiter->second;
        }
      }
    else
      {
      newCurveSet = 1;
      }
    }
  else // type == TYPE_SD
    {
    //icciter = this->IntensitySDCurveCache.find(maskID);
    icciter = sdCache->find(maskID);
    //if (icciter != this->IntensityCurveCache.end())
    if (icciter != meanCache->end())
      {
      IntensityCurveSetType::iterator icsiter = icciter->second.find(label);
      if (icsiter != icciter->second.end())
        {
        return icsiter->second;
        }
      }
    else
      {
      newCurveSet = 1;
      }
    }


  if (newCurveSet)
    {
    IntensityCurveSetType curveSet;
    //this->IntensityCurveCache[std::string(maskID)] = curveSet;
    (*meanCache)[std::string(maskID)] = curveSet;
    //this->IntensitySDCurveCache[std::string(maskID)] = curveSet;
    (*sdCache)[std::string(maskID)] = curveSet;
    }
    
  // generate mask index table
  GenerateIndexTable(mnode->GetImageData(), label);
  
  vtkDoubleArray* meanArray = vtkDoubleArray::New();
  vtkDoubleArray* sdArray = vtkDoubleArray::New();
  meanArray->SetNumberOfValues(0);
  sdArray->SetNumberOfValues(0);

  FrameNodeVectorType::iterator iter;
  FrameNodeVectorType* frameNodes;
  if (series == 0)
    {
    frameNodes = &this->FrameNodeVector;
    }
  else
    {
    frameNodes = &this->RegisteredFrameNodeVector;
    }
  //for (iter = this->FrameNodeVector.begin(); iter != this->FrameNodeVector.end(); iter ++)
  for (iter = frameNodes->begin(); iter != frameNodes->end(); iter ++)
    {
    const char* imageID = iter->c_str();
    std::cerr << "image id = " << imageID << std::endl;

    vtkMRMLScalarVolumeNode* inode =
      vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(imageID));
    double meanvalue = 0.0;
    double sdvalue = 0.0;
    if (inode)
      {
      meanvalue = GetMeanIntencity(inode->GetImageData());
      sdvalue   = GetSDIntencity(inode->GetImageData(), meanvalue);
      }
    std::cerr << "mean = " << meanvalue << ", sd = " << sdvalue << std::endl;
    meanArray->InsertNextValue(meanvalue); 
    sdArray->InsertNextValue(sdvalue); 
    }

  //this->IntensityCurveCache[std::string(maskID)][label] = meanArray;
  //this->IntensitySDCurveCache[std::string(maskID)][label] = sdArray;
  (*meanCache)[std::string(maskID)][label] = meanArray;
  (*sdCache)[std::string(maskID)][label] = sdArray;

  if (type == TYPE_MEAN)
    {
    return meanArray;
    }
  else
    {
    return sdArray;
    }
}


//---------------------------------------------------------------------------
int vtkControl4DLogic::SaveIntensityCurve(int series, const char* maskID, int label, const char* filename)
{
  vtkDoubleArray* meanArray = GetIntensityCurve(series, maskID, label, TYPE_MEAN);
  vtkDoubleArray* sdArray = GetIntensityCurve(series, maskID, label, TYPE_SD);

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
int vtkControl4DLogic::RunSeriesRegistration(int sIndex, int eIndex, int kIndex, RegistrationParametersType& param)
{
  if (sIndex < 0 ||
      eIndex >= GetNumberOfFrames() ||
      kIndex < 0 ||
      kIndex >= GetNumberOfFrames() )
    {
    std::cerr << "int vtkControl4DGUI::RunSeriesRegistration(): irregular index" << std::endl;
    return 0;
    }
  
  CreateRegisteredVolumeNodes();
  
  const char* fixedFrameNodeID = GetFrameNodeID(kIndex);
  vtkMRMLScalarVolumeNode* fixedNode;  
  if (fixedFrameNodeID)
    {
    fixedNode = 
      vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(fixedFrameNodeID));
    }
  else
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

    const char* movingFrameNodeID = GetFrameNodeID(i);
    const char* outputFrameNodeID = GetRegisteredFrameNodeID(i);
    if (movingFrameNodeID && outputFrameNodeID)
      {
      vtkMRMLScalarVolumeNode* movingNode;
      movingNode = 
        vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(movingFrameNodeID));

      vtkMRMLScalarVolumeNode* outputNode;
      outputNode = 
        vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(outputFrameNodeID));

      RunRegistration(fixedNode, movingNode, outputNode, param);
      }
    }

  return 1;

}
  

//---------------------------------------------------------------------------
int vtkControl4DLogic::RunRegistration(vtkMRMLScalarVolumeNode* fixedNode,
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
    sprintf(name, "BSpline-%s", movingNode->GetName());
    std::cerr << "Transform = " << name << std::endl;
    transformNode->SetName(name);
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
