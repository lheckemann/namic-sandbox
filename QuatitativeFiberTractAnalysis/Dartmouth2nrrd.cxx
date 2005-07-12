/*=========================================================================
  Program:   Dartmouth2Nrrd
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Author:    Casey Goodlett

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkExceptionObject.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileWriter.h>
#include <itkMetaDataObject.h>

const int numweightings = 14;
const int numbaselines = 2;

const std::string gradstrings[]  = {
    "0.0 0.0 0.0",
    "-0.8238094 -0.4178235 -0.3830949",
    "-0.5681645 0.5019867 -0.6520725",
    "0.4296590 0.1437401 0.8914774",
    "-0.0482123 0.6979894 0.7144833",
    "0.8286872 -0.0896669 -0.5524829",
    "0.9642489 -0.2240180 0.1415627",
    "-0.1944068 0.9526976 -0.2336092",
    "0.1662157 0.6172332 -0.7690224",
    "-0.3535898 -0.9178798 -0.1801968",
    "-0.7404186 -0.5774342 0.3440203",
    "-0.2763061 0.0476582 0.9598873",
    "0.6168819 -0.7348858 -0.2817793"};

int main(int argc, char* argv[])
{
  typedef itk::Image<unsigned short, 3>     DICOMImageType;
  typedef itk::ImageSeriesReader<DICOMImageType> ReaderType;
  typedef itk::GDCMImageIO                  ImageIOType;

  if(argc != 3)
    {
    std::cerr << "Usage: Dartmouth2nrrd <directory> <outfile>" << std::endl;
    return 0;
    }

  ImageIOType::Pointer dicomIO = ImageIOType::New();

  typedef itk::GDCMSeriesFileNames NamesGeneratorType;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
  nameGenerator->SetInputDirectory(argv[1]);

  typedef std::vector<std::string> fileNamesContainer;
  fileNamesContainer fileNames;
  fileNames = nameGenerator->GetInputFileNames();

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileNames(fileNames);
  reader->SetImageIO(dicomIO);

  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject e)
    {
    std::cerr << e << std::endl;
    }
  
  DICOMImageType::SpacingType dicomspacing = reader->GetOutput()->GetSpacing();
  std::cout << dicomspacing << std::endl;

  ReaderType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  const int nslices = size[2] / numweightings;

  // Create pseudo-4d nrrd volume
  typedef itk::Image<unsigned short, 4>     NrrdType;
  NrrdType::SizeType nrrdSize;
  nrrdSize[0] = size[0];
  nrrdSize[1] = size[1];
  nrrdSize[2] = size[2] / numweightings;
  nrrdSize[3] = numweightings - (numbaselines - 1);

  NrrdType::IndexType start = {0,0,0,0};
  
  NrrdType::Pointer newvolume = NrrdType::New();

  NrrdType::RegionType region;
  region.SetSize(nrrdSize);
  region.SetIndex(start);

  newvolume->SetRegions(region);
  newvolume->Allocate();

  // Iterate over pixels placing them in appropriate place in new
  // volume
  typedef itk::ImageRegionIteratorWithIndex<NrrdType> IteratorType;
  IteratorType outputIt(newvolume,newvolume->GetLargestPossibleRegion());
  
  // Note that the dartmouth slices are organized as follows
  // b0a_1    -- slice 1 of baseline a
  // b0a_2    -- slice 2 of baseline a
  // ...
  // b0a_36   -- slice 36 of baseline a
  // b0b_1    -- slice 1 of baseline b
  // ...
  // b0b_36   -- slice 36 of baseline b
  // b1_1     -- slice 1 of first gradient weighted image
  // ...
  // b1_26    -- slice 26 of first gradient weighted image
  for(outputIt.GoToBegin(); !outputIt.IsAtEnd(); ++outputIt)
    {
    NrrdType::IndexType idx = outputIt.GetIndex();

    // If the pixel to be written is the baseline average over all
    // baseline images

    if(idx[3] == 0)
      {
      DICOMImageType::IndexType bidx;
      bidx[0] = idx[0];
      bidx[1] = idx[1];
      bidx[2] = idx[2];

      unsigned short avg = 0;
      for(unsigned int i = 0; i < numbaselines; ++i)
        {
//        std::cout << bidx << std::endl;
        avg += reader->GetOutput()->GetPixel(bidx);
        bidx[2] += nslices;
        }
      avg = avg / numbaselines;

      outputIt.Set( avg );
      }
    else
      {
      DICOMImageType::IndexType dicomIDX;
      dicomIDX[0] = idx[0];
      dicomIDX[1] = idx[1];
      dicomIDX[2] = idx[3]*nslices + idx[2];

      outputIt.Set( reader->GetOutput()->GetPixel(dicomIDX) );
      }
    }
  
  itk::MetaDataDictionary meta;
  
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_000",gradstrings[0]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_001",gradstrings[1]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_002",gradstrings[2]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_003",gradstrings[3]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_004",gradstrings[4]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_005",gradstrings[5]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_006",gradstrings[6]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_007",gradstrings[7]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_008",gradstrings[8]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_009",gradstrings[9]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_010",gradstrings[10]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_011",gradstrings[11]);
  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_012",gradstrings[12]);
//  itk::EncapsulateMetaData<std::string>(meta,"DW_gradient_013",gradstrings[13]);

  newvolume->SetMetaDataDictionary(meta);
  newvolume->SetSpacing(dicomspacing);
  // TODO: We should probably set the orientation frame in the header
  // somehow here.
  // TODO: We should set the measurement frame for the gradient information.

  typedef itk::ImageFileWriter<NrrdType> WriterType;
  WriterType::Pointer nrrdWriter = WriterType::New();
  nrrdWriter->UseInputMetaDataDictionaryOn();
  nrrdWriter->SetInput(newvolume);
  nrrdWriter->SetFileName(argv[2]);
  try
    {
    nrrdWriter->Update();
    }
  catch (itk::ExceptionObject e)
    {
    std::cout << e << std::endl;
    }

  return 0;
}

