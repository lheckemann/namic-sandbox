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
  this->CurrentFrameFG = NULL;
  this->CurrentFrameBG = NULL;

  this->IntensityCurveCache.clear();
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
int vtkControl4DLogic::LoadImagesFromDir(const char* path)
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

  for (int i = 0; i < nVolumes; i ++)
    {
    statusMessage.progress = (double)i / (double)nVolumes;
    this->InvokeEvent ( vtkControl4DLogic::ProgressDialogEvent, &statusMessage);

    vtkMRMLVolumeNode *volumeNode = NULL;
    vtkMRMLScalarVolumeNode *scalarNode = vtkMRMLScalarVolumeNode::New();
    vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();
    vtkMRMLVolumeArchetypeStorageNode*storageNode = vtkMRMLVolumeArchetypeStorageNode::New();
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
    range[0] = 0.0;
    range[1] = 256.0;
    displayNode->SetLowerThreshold(range[0]);
    displayNode->SetUpperThreshold(range[1]);
    displayNode->SetWindow(range[1] - range[0]);
    displayNode->SetLevel(0.5 * (range[1] - range[0]) );
    vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
    displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());

    scene->AddNode(storageNode);  
    scene->AddNode(displayNode);  
    volumeNode->SetAndObserveStorageNodeID(storageNode->GetID());
    volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
    scene->AddNode(volumeNode);  
    this->FrameNodeVector.push_back(std::string(volumeNode->GetID()));

    scalarNode->Delete();
    storageNode->Delete();
    colorLogic->Delete();
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
const char* vtkControl4DLogic::SwitchNodeFG(int index)
{
  if (index >= 0 && index < (int)this->FrameNodeVector.size())
    {
    this->CurrentFrameFG = this->FrameNodeVector[index].c_str();
    return this->CurrentFrameFG;
    }
  else
    {
    return NULL;
    }
}


//---------------------------------------------------------------------------
const char* vtkControl4DLogic::SwitchNodeBG(int index)
{
  if (index >= 0 && index < (int)this->FrameNodeVector.size())
    {
    this->CurrentFrameBG = this->FrameNodeVector[index].c_str();
    return this->CurrentFrameBG;
    }
  else
    {
    return NULL;
    }
}

//---------------------------------------------------------------------------
double vtkControl4DLogic::GetMeanIntencity(vtkImageData* image)
{
  // this function should be optimized.
  double sum = 0.0;

  IndexTableType::iterator iter;
  for (iter = this->MaskIndexTable.begin(); iter != this->MaskIndexTable.end(); iter ++)
    {
    sum += image->GetScalarComponentAsDouble(iter->x, iter->y, iter->z, 0);
    }

  double mean = sum / (double)this->MaskIndexTable.size();

  return mean;
}


//---------------------------------------------------------------------------
void vtkControl4DLogic::ClearIntensityCurveCache(const char* maskID)
{
  if (maskID == NULL)
    {
    this->IntensityCurveCache.clear();
    return;
    }
  
  IntensityCurveCacheType::iterator icciter = this->IntensityCurveCache.find(maskID);
  if (icciter != this->IntensityCurveCache.end())
    {
    this->IntensityCurveCache.erase(icciter);
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
vtkDoubleArray* vtkControl4DLogic::GetIntensityCurve(const char* maskID, int label)
{
  vtkMRMLScalarVolumeNode* mnode =
    vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(maskID));

  if (!mnode)
    {
    return NULL;
    }

  // Check cache to see if the intensity curve has already been generated.
  IntensityCurveCacheType::iterator icciter;
  icciter = this->IntensityCurveCache.find(maskID);
  if (icciter != this->IntensityCurveCache.end())
    {
    IntensityCurveSetType::iterator icsiter = icciter->second.find(label);
    if (icsiter != icciter->second.end())
      {
      return icsiter->second;
      }
    }
  else
    {
    IntensityCurveSetType curveSet;
    this->IntensityCurveCache[std::string(maskID)] = curveSet;
    }


  // generate mask index table
  GenerateIndexTable(mnode->GetImageData(), label);
  
  vtkDoubleArray* iarray = vtkDoubleArray::New();
  iarray->SetNumberOfValues(0);
  FrameNodeVectorType::iterator iter;

  for (iter = this->FrameNodeVector.begin(); iter != this->FrameNodeVector.end(); iter ++)
    {
    const char* imageID = iter->c_str();
    std::cerr << "image id = " << imageID << std::endl;

    vtkMRMLScalarVolumeNode* inode =
      vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(imageID));
    double value = 0.0;
    if (inode)
      {
      value = GetMeanIntencity(inode->GetImageData());
      }
    std::cerr << "value = " << value << std::endl;
    iarray->InsertNextValue(value); 
    }

  this->IntensityCurveCache[std::string(maskID)][label] = iarray;

  return iarray;
  
}

//---------------------------------------------------------------------------
int vtkControl4DLogic::SaveIntensityCurve(const char* maskID, int label, const char* filename)
{
  vtkDoubleArray* iarray = GetIntensityCurve(maskID, label);

  if (!iarray)
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

  int n = iarray->GetNumberOfTuples();
  for (int i = 0; i < n; i ++)
    {
    fout << i << ", " << iarray->GetValue(i) << std::endl;
    }

  fout.close();

  return 1;

}
