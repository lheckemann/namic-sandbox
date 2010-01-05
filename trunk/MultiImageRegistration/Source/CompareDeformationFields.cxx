/*=========================================================================

    Program:   Insight Segmentation & Registration Toolkit
    Module:    $RCSfile: ResampleImageFilter.cxx,v $
    Language:  C++
    Date:      $Date: 2006/05/14 12:12:52 $
    Version:   $Revision: 1.32 $

    Copyright (c) Insight Software Consortium. All rights reserved.
    See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notices for more information.

    =========================================================================*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkResampleImageFilter.h"

#include "itkIdentityTransform.h"
#include "itkAffineTransform.h"
#include "BSplineDeformableTransformOpt.h"
#include "itkBSplineDeformableTransform.h"
#include "itkTransformFactory.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkImageRegionIterator.h"

#include <string>
#include <sstream>
#include <fstream>
#include <vector>


//Define the global types for image type
#define PixelType float
#define InternalPixelType float
#define Dimension 3

#include "itkTransformFileReader.h"

#include <itksys/SystemTools.hxx>

std::string replaceExtension(const std::string oldname, const std::string extension)
{
  return oldname.substr(0, oldname.rfind(".")) + "." + extension;
}

int getCommandLine(       int argc, char *initFname, std::vector<std::string>& fileNames, std::string& inputFolder, std::string& outputFolder,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          std::string& useBspline, std::string& useBsplineHigh,
                          std::string& writeDeformationFields, std::string& write3DImages,
                          std::string& syntheticFolderName )
{


  std::ifstream initFile(initFname);
  if( initFile.fail() )
  {
    std::cout << "could not open file: " << initFname << std::endl;
    return EXIT_FAILURE;
  }

  while( !initFile.eof() )
  {
    
    std::string dummy;
    initFile >> dummy;

    if(dummy == "-i")
    {
      initFile >> dummy;
      inputFolder = dummy;
    }
    else if (dummy == "-o")
    {
      initFile >> dummy;
      outputFolder = dummy;
    }
    else if (dummy == "-bsplineInitialGridSize")
    {
      initFile >> dummy;
      bsplineInitialGridSize = atoi(dummy.c_str());
    }
    else if (dummy == "-numberOfBsplineLevel")
    {
      initFile >> dummy;
      numberOfBsplineLevel = atoi(dummy.c_str());
    }

    else if (dummy == "-useBspline")
    {
      initFile >> dummy;
      useBspline = dummy;
    }
    else if (dummy == "-useBsplineHigh")
    {
      initFile >> dummy;
      useBsplineHigh = dummy;
    }
    else if (dummy == "-writeDeformationFields")
    {
      initFile >> dummy;
      writeDeformationFields = dummy;
    }
    else if (dummy == "-write3DImages")
    {
      initFile >> dummy;
      write3DImages = dummy;
    }    
    else if (dummy == "-f")
    {
      initFile >> dummy;
      fileNames.push_back(dummy); // get file name
    }
    else if (dummy == "-syntheticFolderName")
    {
      initFile >> dummy;
      syntheticFolderName = dummy;
    } 
  }

  initFile.close();
  return EXIT_SUCCESS;
}


int main( int argc, char * argv[] )
{
  if( argc < 3 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  <filenames.init>  <parameters.init>" << std::endl;
    return EXIT_FAILURE;
  }

  // Input Parameter declarations
  std::vector<std::string> fileNames;
  std::string inputFolder;
  std::string syntheticFolderName;
  std::string outputFolder;
  std::string writeDeformationFields = "off";
  std::string write3DImages = "off";
  
  int bsplineInitialGridSize = 4;
  int numberOfBsplineLevel = 0;
    
  std::string useBspline("off");
  std::string useBsplineHigh("off");
  

    //Get the command line arguments
  for(int i=1; i<argc; i++)
  {
    if ( getCommandLine(
                  argc, argv[i], fileNames, inputFolder, outputFolder,
                  bsplineInitialGridSize,  numberOfBsplineLevel,
                  useBspline, useBsplineHigh,
                  writeDeformationFields, write3DImages, syntheticFolderName )
       ) 
    {
      std::cout << "Error reading parameter file " << std::endl;
      return EXIT_FAILURE;
    }
  }

  const int N = fileNames.size();

  if(N<2)
  {
     std::cout << "Not enough filenames" << std::endl;
     return EXIT_FAILURE;
  }
  
  typedef itk::Image< PixelType,  Dimension >   ImageType;

  typedef itk::ImageFileReader< ImageType  >  ReaderType;
  typedef itk::ImageFileWriter< ImageType >  WriterType;

  // Read intensity images
  std::vector< ReaderType::Pointer > imageReaderArray(N);
  for(int i=0; i<N; i++)
  {
    imageReaderArray[i] = ReaderType::New();
    std::string fname = inputFolder + fileNames[i];
    imageReaderArray[i]->SetFileName(fname.c_str());
    imageReaderArray[i]->Update();
  } 
  
  // Get the number of tranform levels
  int transformLevels;
  if( useBsplineHigh == "on")
  {
    transformLevels = numberOfBsplineLevel + 2;
  }
  else if ( useBspline == "on" )
  {
    transformLevels = 2;
  }
  else
  {
    transformLevels = 1;
  }
  std::vector< std::vector < std::string > >  transformFileNames(transformLevels);
  std::vector < std::string >   syntheticTransformFileNames(N);

  if( syntheticFolderName == "" )
  {
    syntheticFolderName = inputFolder + "../";
  }
  for(int i=0; i<N;i++)
  {
    syntheticTransformFileNames[i] = syntheticFolderName + "TransformFiles/" + fileNames[i];
    syntheticTransformFileNames[i] = replaceExtension(syntheticTransformFileNames[i], "txt");
  }
  
  // Generate the transform filenames
  for(int i=0; i<transformLevels; i++)
  {
    transformFileNames[i].resize(N);
    if( i==0 ) // Generate affine transform filenames
    {
      for( int j=0; j<N; j++)
      {
        transformFileNames[i][j] = outputFolder + "Affine/TransformFiles/" + fileNames[j];
        transformFileNames[i][j] = replaceExtension(transformFileNames[i][j], "txt");
      }
    }
    else // generate bspline names
    {
      std::ostringstream bsplineFolderName;
      bsplineFolderName << "Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1);

      for( int j=0; j<N; j++)
      {
        transformFileNames[i][j] = outputFolder + bsplineFolderName.str() + "/TransformFiles/" + fileNames[j];
        transformFileNames[i][j] = replaceExtension(transformFileNames[i][j], "txt");
      }
    }

  }


  // Resample the images
  // Read the input transforms and compute the dice measure
  for(int i=0; i<transformLevels; i++)
  {
    // typedef for transformation types
    typedef itk::Transform< double, Dimension,Dimension >  TransformType;
    typedef itk::AffineTransform< double, Dimension >  AffineTransformType;
    typedef itk::BSplineDeformableTransformOpt< double,
                                             Dimension,
                                             3 >     BSplineTransformType;


    std::vector< TransformType::Pointer >          transformArray(N);
    std::vector< TransformType::Pointer >          syntheticTransformArray(N);
    std::vector< BSplineTransformType::Pointer >   bsplineTransformArray(N);
    std::vector< AffineTransformType::Pointer >    affineTransformArray(N);
    std::vector< BSplineTransformType::Pointer >   syntheticBsplineTransformArray(N);
    std::vector< AffineTransformType::Pointer >    syntheticAffineTransformArray(N);
    
    typedef BSplineTransformType::ParametersType BsplineParametersType;
    std::vector< BsplineParametersType > bsplineParametersArray(N);
    std::vector< BsplineParametersType > syntheticBsplineParametersArray(N);
    BsplineParametersType meanParameters;
    
    // Resample the labels according to transforms
    typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
    std::vector<ResampleFilterType::Pointer> resampleArray(N);

    std::vector<ResampleFilterType::Pointer> imageResampleArray(N);

    
    for(int j=0; j<N; j++)
    {
      // Typedef for reader
      typedef itk::TransformFileReader    TransformFileReader;
      typedef TransformFileReader::TransformListType   TransformListType;
      
      // Create reader factories
      itk::TransformFactoryBase::Pointer f = itk::TransformFactoryBase::GetFactory();
      affineTransformArray[j] = AffineTransformType::New();
      syntheticAffineTransformArray[j] = AffineTransformType::New();
      f->RegisterTransform(affineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           affineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           affineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           1,
                           itk::CreateObjectFunction<AffineTransformType>::New());

      bsplineTransformArray[j] = BSplineTransformType::New();
      syntheticBsplineTransformArray[j] = BSplineTransformType::New();
      f->RegisterTransform(bsplineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           bsplineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           bsplineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           1,
                           itk::CreateObjectFunction<BSplineTransformType>::New());
      // Inpute image, apply identity tranform
      if( i == 0)
      {
        std::cout << "Reading " << transformFileNames[i][j].c_str() << std::endl;
        TransformFileReader::Pointer        transformFileReader = TransformFileReader::New();
        transformFileReader->SetFileName(transformFileNames[i][j].c_str());
        
        // Create the transforms
        transformFileReader->Update();
        TransformListType*   transformList = transformFileReader->GetTransformList();
        
        affineTransformArray[j]->SetFixedParameters(transformList->front()->GetFixedParameters());
        affineTransformArray[j]->SetParameters(transformList->front()->GetParameters());
        transformArray[j] = affineTransformArray[j];
      }
      else
      {

        std::cout << "Reading " << transformFileNames[i][j].c_str() << std::endl;

        // Get Affine transform
        TransformFileReader::Pointer        affineTransformFileReader = TransformFileReader::New();
        affineTransformFileReader->SetFileName(transformFileNames[0][j].c_str());
        affineTransformFileReader->Update();
        TransformListType*   affineTransformList = affineTransformFileReader->GetTransformList();

        
        affineTransformArray[j]->SetFixedParameters(affineTransformList->front()->GetFixedParameters());
        affineTransformArray[j]->SetParameters(affineTransformList->front()->GetParameters());
        

        
        TransformFileReader::Pointer        transformFileReader = TransformFileReader::New();
        transformFileReader->SetFileName(transformFileNames[i][j].c_str());
        transformFileReader->Update();
        TransformListType*   transformList = transformFileReader->GetTransformList();
        
        bsplineTransformArray[j]->SetFixedParameters(transformList->front()->GetFixedParameters());
        
        bsplineParametersArray[j].set_size(bsplineTransformArray[j]->GetNumberOfParameters());
        bsplineTransformArray[j]->SetParameters(bsplineParametersArray[j]);
        bsplineTransformArray[j]->SetParametersByValue(transformList->front()->GetParameters());
        
        bsplineTransformArray[j]->SetBulkTransform(affineTransformArray[j]);

        transformArray[j] = bsplineTransformArray[j];

      }
      
      std::cout << "Reading " << syntheticTransformFileNames[j] << std::endl;
      // Read the synthetic transforms from file
      if( useBspline != "on" ) // affine
      {
        TransformFileReader::Pointer        transformFileReader = TransformFileReader::New();
        transformFileReader->SetFileName(syntheticTransformFileNames[j].c_str());
        
        // Create the transforms
        transformFileReader->Update();
        TransformListType*   transformList = transformFileReader->GetTransformList();
        
        syntheticAffineTransformArray[j]->SetFixedParameters(transformList->front()->GetFixedParameters());
        syntheticAffineTransformArray[j]->SetParameters(transformList->front()->GetParameters());
        syntheticTransformArray[j] = syntheticAffineTransformArray[j];
      }
      else
      {
              
        TransformFileReader::Pointer        transformFileReader = TransformFileReader::New();
        transformFileReader->SetFileName(syntheticTransformFileNames[j].c_str());
        transformFileReader->Update();
        TransformListType*   transformList = transformFileReader->GetTransformList();
        
        syntheticBsplineTransformArray[j]->SetFixedParameters(transformList->front()->GetFixedParameters());
        
        syntheticBsplineParametersArray[j].set_size(syntheticBsplineTransformArray[j]->GetNumberOfParameters());
        syntheticBsplineTransformArray[j]->SetParameters(syntheticBsplineParametersArray[j]);
        syntheticBsplineTransformArray[j]->SetParametersByValue(transformList->front()->GetParameters());
        
        syntheticTransformArray[j] = syntheticBsplineTransformArray[j];

      }

    }

    // Create the output folders
    std::string currentFolderName;
    if(i==0)
    {
      currentFolderName = outputFolder + "Affine/";
    }
    else
    {
      std::ostringstream bsplineFolderName;
      bsplineFolderName << "Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1) << "/";
      currentFolderName = outputFolder + bsplineFolderName.str();
    }
    itksys::SystemTools::MakeDirectory( currentFolderName.c_str() );
    
    std::string diffFolderName = currentFolderName+"DefFieldDifference/";
    itksys::SystemTools::MakeDirectory( diffFolderName.c_str() );
    
    // compare the deformation fields and compute the magnitude difference images
    std::ofstream maxMeanDefFile((diffFolderName+"meanMax.txt").c_str());
    for(int j=0; j<N; j++)
    {
      std::cout << "Writing" << (diffFolderName+fileNames[j]).c_str() << std::endl;
      ImageType::Pointer imagePointer = imageReaderArray[j]->GetOutput();

      typedef itk::ImageRegionIteratorWithIndex< ImageType > IteratorWithIndexType;
      IteratorWithIndexType it(imagePointer, imagePointer->GetLargestPossibleRegion());
      ImageType::IndexType index;
      typedef TransformType::InputPointType        ImagePointType;
      ImagePointType point;
      ImagePointType mappedPoint;
      
      double max = 0.0;
      double mean = 0.0;
      for ( it.GoToBegin(); !it.IsAtEnd(); ++it)
      {
        // Get sampled index
        index = it.GetIndex();

        // Translate index to point
        imagePointer->TransformIndexToPhysicalPoint(index, point);
        
        // transform the point
        mappedPoint = syntheticTransformArray[j]->TransformPoint(point);

        // transform the point again
        mappedPoint = transformArray[j]->TransformPoint(mappedPoint);
        
       double diff = 0.0;
       for(int j=0; j<Dimension; j++)
       {
         diff += (point[j] - mappedPoint[j])*(point[j] - mappedPoint[j]);
       }
       
       diff = sqrt(diff);
       
       it.Set(diff);
       
       // Find mean max
       if(diff > max )
       {
         max = diff;
       }
       mean += diff;
       
      }
      
      maxMeanDefFile << mean /
                     (double)(imagePointer->GetLargestPossibleRegion().GetNumberOfPixels())
                     << " " << max << std::endl;
      
      // Write the output image
      WriterType::Pointer  writer = WriterType::New();
      writer->SetInput(imagePointer);
      writer->SetImageIO(imageReaderArray[j]->GetImageIO());
      writer->SetFileName( (diffFolderName+fileNames[j]).c_str() );
      writer->Update();
      
    }
      
  


    
    
  } // End of transform levels

  return EXIT_SUCCESS;
}

