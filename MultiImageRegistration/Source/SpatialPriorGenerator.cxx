/*
 *  This file is designed to provide the funcationality of creating a spatial prior image from 
 *
 *
 */


#include <iostream>
#include <vector>
#include <string>
#include <set>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkCastImageFilter.h>
#include "SpatialPriorGeneratorCLP.h"

const   unsigned int                          Dimension = 3;
typedef long int                              LabelVoxelType;
typedef float                                 PriorVoxelType;
typedef itk::Image<LabelVoxelType, Dimension> LabelmapType;
typedef LabelmapType::Pointer                 LabelmapPointer;
typedef itk::ImageFileReader<LabelmapType>    LabelmapReaderType;
typedef itk::Image<PriorVoxelType, Dimension> PriorType;
typedef PriorType::Pointer                    PriorPointer;
typedef itk::ImageFileWriter<PriorType>       PriorWriterType;

void GenerateSpatialPrior(unsigned int                        numVoxels,
                          const std::vector<LabelVoxelType*>& labelmaps,
                          PriorVoxelType*                     spatialPrior,
                          std::set<LabelVoxelType>            foregroundVoxels,
                          const LabelVoxelType&               intensityScale)
{
  unsigned int numLabelmaps = labelmaps.size();

  //
  // count labels in set as foreground
  if (!foregroundVoxels.empty())
    {
    for (unsigned int i = 0; i < numVoxels; ++i)
      {
      // count the foreground voxels
      int numForegroundVoxels = 0;
      for (unsigned int j = 0; j < numLabelmaps; ++j)
        {
        if (foregroundVoxels.count(labelmaps[j][i]) > 0)
          {
          ++numForegroundVoxels;
          }
        }
    
      // set the probability
      spatialPrior[i] = 
        (static_cast<PriorVoxelType>(numForegroundVoxels) / numLabelmaps);
      }
    }
  //
  // count all nonzero labels as foreground
  else
    {
    for (unsigned int i = 0; i < numVoxels; ++i)
      {
      // count the foreground voxels
      int numForegroundVoxels = 0;
      for (unsigned int j = 0; j < numLabelmaps; ++j)
        {
        if (labelmaps[j][i] != 0)
          {
          ++numForegroundVoxels;
          }
        }
    
      // set the probability
      spatialPrior[i] = intensityScale *
        (static_cast<PriorVoxelType>(numForegroundVoxels) / numLabelmaps);
      }
    }
}

void PrintUsage()
{
  std::cout << "Usage: atlasgen <numImages> <filename 1> <filename 2> ... "
            << std::endl
            << "                <output filename> "
            << std::endl
            << "                [<numLabels> <label 1> <label 2> ...]" 
            << std::endl
            << "                [<prior scaling factor = 255>]" 
            << std::endl;
}

int main(int argc, char** argv)
{
  PARSE_ARGS;
  
  std::set<LabelVoxelType>           foregroundValues;

  //
  if((unsigned int)numImages != labelmapFilenames.size())
  {
    std::cout<<"Error: wrong number of file name arguments; numImages="<<numImages<<std::endl;
    return -1;
  }
  // display information about what we will do
  std::cout << "Number of labelmap images: " << labelmapFilenames.size() 
            << std::endl;
  for (unsigned int i = 0; i < labelmapFilenames.size(); ++i)
    {
    std::cout << "  " << labelmapFilenames[i] << std::endl;
    }
  if((unsigned int)numLabels != labelValues.size())
  {
    std::cout<<"Error: wrong number of foreground value arguments; numLabels="<<numLabels<<std::endl;
    return -1;
  }
  std::cout << "Number of foreground labels: " << labelValues.size() 
            << std::endl;    
  for (int i = 0; i < numLabels; ++i)
    {
      foregroundValues.insert(atoi(labelValues[i].c_str()));
    }

  if (foregroundValues.empty())
    {
    std::cout << "Counting all nonzero voxels as foreground" << std::endl;
    }
  std::cout << "Scaling all final priors by: " << intensityScale << std::endl;
  std::cout << "Spatial Prior Filename: " << priorFilename 
            << std::endl;

  // 
  // load images; note the conversion to LabelVoxelType
  std::cerr << "Loading images..." << std::endl;

  std::vector<LabelmapReaderType::Pointer> labelmapReaders;
  std::vector<LabelVoxelType*>             labelmaps;
  unsigned int numVoxelsPerImage = 0;

  for (unsigned int i = 0; i < labelmapFilenames.size(); ++i)
    {
    labelmapReaders.push_back(LabelmapReaderType::New());
    labelmapReaders[i]->SetFileName(labelmapFilenames[i].c_str());

    std::cerr << "  " << labelmapFilenames[i] << std::endl;

    labelmapReaders[i]->Update();
    labelmaps.push_back(labelmapReaders[i]->GetOutput()->GetBufferPointer());

    unsigned int numVoxels    = labelmapReaders[i]->GetOutput()->
      GetLargestPossibleRegion().GetNumberOfPixels();

    //
    // make sure all images have the same number of voxels
    if (i == 0)
      {
        numVoxelsPerImage = numVoxels;
      }
    else
      {
        if (numVoxels != numVoxelsPerImage)
          {
            std::cerr <<
              "Error: all input images must have the same number of voxels"
                      << std::endl;
            PrintUsage();
            return -2;
          }
      }
    }

  //
  // allocate the spatial prior image
  std::cerr << "Allocating prior..." << std::endl;

  typedef itk::CastImageFilter<LabelmapType, PriorType> PriorAllocatorFilter;
  PriorAllocatorFilter::Pointer priorAllocator = PriorAllocatorFilter::New();
  priorAllocator->SetInput(labelmapReaders[0]->GetOutput());
  priorAllocator->Update();
  PriorVoxelType* spatialPrior = 
    priorAllocator->GetOutput()->GetBufferPointer();
  
  // how do we add metadata? 
  //priorAllocator->GetMetaDataDictionary()["Kangaroo"] = "Captain";

  //
  // compute the spatial prior and write it to disk
  std::cerr << "Computing prior..." << std::endl;
  GenerateSpatialPrior(numVoxelsPerImage, labelmaps, spatialPrior, 
                       foregroundValues, intensityScale);
  priorAllocator->GetOutput()->Modified();
  PriorWriterType::Pointer priorWriter = PriorWriterType::New();
  priorWriter->SetFileName(priorFilename);
  priorWriter->SetInput(priorAllocator->GetOutput());

  std::cerr << "Writing prior..." << std::endl;
  priorWriter->Write();

  return 0;
}
