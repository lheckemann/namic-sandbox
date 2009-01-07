#include <iostream>

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

#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLVolumeHeaderlessStorageNode.h"

#include "Load4DCLP.h"


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


int main(int argc, char * argv [])
{
  PARSE_ARGS;

  itk::TimeProbesCollectorBase collector;

  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  gdcmIO->LoadPrivateTagsOn();

  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetUseSeriesDetails(true);
  inputNames->SetDirectory( inputDirectory.c_str() );

  itk::SerieUIDContainer seriesUIDs = inputNames->GetSeriesUIDs();
  int nVolumes;
  std::vector<ReaderType::FileNamesContainer> fileNamesContainerList;

  if (seriesUIDs.size() == 1)  // if single series UID is used
    {
    const ReaderType::FileNamesContainer & filenames = 
      inputNames->GetFileNames(seriesUIDs[0]);
    
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetImageIO( gdcmIO );
    reader->SetFileNames( filenames );
    try
      {
      collector.Start( "Checking series ...." );
      reader->Update();
      collector.Stop( "Checking series ...." );
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
    for (int i = 1; i < nSlices; i ++)
      {
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
    std::cerr << "Number of slices in Volume is " << nSlicesInVolume << std::endl;
    std::cerr << "Number of slices " << nSlices << std::endl;
    std::cerr << "Number of slices " << filenames.size() << std::endl;

    nVolumes = nSlices / nSlicesInVolume;
    fileNamesContainerList.resize(nVolumes);

    //ReaderType::FileNamesContainer::iterator iter = filenames.begin();
    for (int i = 0; i < nVolumes; i ++)
      {
      fileNamesContainerList[i].resize(nSlicesInVolume);
      for (int j = 0; j < nSlicesInVolume; j ++)
        {
        std::cerr << "fileNamesContainerList " << i << ", " << j << std::endl;
        fileNamesContainerList[i][j] = filenames[i*nSlicesInVolume + j];
        //iter ++;
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
      std::cerr << "UID = " << *iter << std::endl;
      fileNamesContainerList.push_back(inputNames->GetFileNames(*iter));
      }
    }
      
  //std::vector<ReaderType::FileNamesContainer>::iterator fnciter;
  //for (fnciter = fileNamesContainerList.begin(); fnciter != fileNamesContainerList.end(); fnciter ++)
  vtkMRMLScene* scene = vtkMRMLScene::New();
  for (int i = 0; i < nVolumes; i ++)
    {
    std::cerr << "Reading Volume " << i << std::endl;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetImageIO( gdcmIO );
    //reader->SetFileNames( *fnciter );
    reader->SetFileNames( fileNamesContainerList[i] );
    try
      {
      char str[128];
      sprintf (str, "Reading Volume #%d", i);
      collector.Start( str );
      reader->Update();
      collector.Stop( str );
      }
    catch (itk::ExceptionObject &excp)
      {
      std::cerr << "Exception thrown while reading the series" << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
      }
    //VolumeType::Pointer volume

    WriterType::Pointer      writer =  WriterType::New();
    char filename[128];
    sprintf(filename, "output_%04d.dcm", i);
    writer->SetFileName( filename );
    writer->SetInput( reader->GetOutput() );
    writer->Update();
    std::cerr << "output : " << filename << std::endl; 

    vtkMRMLVolumeNode *volumeNode = NULL;
    vtkMRMLScalarVolumeNode *scalarNode = vtkMRMLScalarVolumeNode::New();
    vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();
    vtkMRMLVolumeHeaderlessStorageNode *storageNode = vtkMRMLVolumeHeaderlessStorageNode::New();
    storageNode->SetFileName(filename);
    storageNode->ReadData(scalarNode);
    volumeNode = scalarNode;
    volumeNode->SetName(filename);

    scene->SaveStateForUndo();
    volumeNode->SetScene(scene);
    storageNode->SetScene(scene);
    displayNode->SetScene(scene);

    scene->AddNode(storageNode);  
    scene->AddNode(displayNode);  
    volumeNode->SetAndObserveStorageNodeID(storageNode->GetID());
    volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
    scene->AddNode(volumeNode);  

    scalarNode->Delete();
    storageNode->Delete();
    if (displayNode)
      {
      displayNode->Delete();
      }
    
    //scene->Import();
    }
  scene->SetURL( "test.mrml" );
  scene->Commit();

  scene->Delete();
  // load to MRML scene

  return EXIT_SUCCESS;
}
