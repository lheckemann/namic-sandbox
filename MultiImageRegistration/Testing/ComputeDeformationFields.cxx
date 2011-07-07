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
#include "itkBSplineDeformableTransform.h"
#include "itkTransformFactory.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkImageRegionIterator.h"

#include "itkNaryFunctorImageFilter.h"

#include "itkDeformationFieldFromTransform.h"
#include "itkIterativeInverseDeformationFieldImageFilter.h"

#include <string>
#include <sstream>
#include <fstream>
#include <vector>


//Define the global types for image type
#define PixelType unsigned short
#define InternalPixelType float
#define Dimension 3

#include "itkTransformFileReader.h"
#include "ComputeDeformationFieldsCLP.h"

#include <itksys/SystemTools.hxx>


std::string replaceExtension(const std::string oldname, const std::string extension)
{
  return oldname.substr(0, oldname.rfind(".nii.gz")) + "." + extension;
}

int getCommandLine(       std::string initFname, std::vector<std::string>& fileNames, std::string& inputFolder, std::string& outputFolder,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          std::string& useBspline, std::string& useBsplineHigh,
                          std::string& writeDeformationFields, std::string& write3DImages )
{


  std::ifstream initFile(initFname.c_str());
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
    
  }

  initFile.close();
  return EXIT_SUCCESS;
}

// And function
class MEAN
{
public:
  MEAN() {};
  ~MEAN() {};
  bool operator!=( const MEAN & ) const
  {
    return false;
  }
  bool operator==( const MEAN & other ) const
  {
    return !(*this != other);
  }
  inline PixelType operator()( const std::vector< PixelType > pixelStack)
  {
    double sum = 0.0;
    for(unsigned int i=0; i<pixelStack.size(); i++)
    {
      sum += pixelStack[i];
    }
    return  (PixelType)(sum/(double)pixelStack.size());
  }

};

class VARIANCE
{
public:
  VARIANCE() {};
  ~VARIANCE() {};
  bool operator!=( const VARIANCE & ) const
  {
    return false;
  }
  bool operator==( const VARIANCE & other ) const
  {
    return !(*this != other);
  }
  inline PixelType operator()( const std::vector< PixelType > pixelStack)
  {
    // Compute mean
    double sum = 0.0;
    double squareSum = 0.0;
    for(unsigned int i=0; i<pixelStack.size(); i++)
    {
      sum += pixelStack[i];
      squareSum += pixelStack[i]*pixelStack[i];
    }
    sum /= (double)pixelStack.size();
    squareSum /= (double) pixelStack.size();

    double std = sqrt(squareSum - sum*sum);
    if(std > 255.0)
    {
      std = 255.0;
    }
    return (PixelType) (std);
    
  }

};
int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  // Input Parameter declarations
  std::vector<std::string> fileNames;
  std::string inputFolder;
  std::string outputFolder;
  std::string writeDeformationFields = "off";
  std::string write3DImages = "off";
  
  int bsplineInitialGridSize = 4;
  int numberOfBsplineLevel = 0;
    
  std::string useBspline("off");
  std::string useBsplineHigh("off");
  

    //Get the command line arguments
 
    if ( getCommandLine(
                  inputFilesName, fileNames, inputFolder, outputFolder,
                  bsplineInitialGridSize,  numberOfBsplineLevel,
                  useBspline, useBsplineHigh,
                  writeDeformationFields, write3DImages )
       ) 
    {
      std:: cout << "Error reading parameter file " << std::endl;
      return EXIT_FAILURE;
    }
    if ( getCommandLine(
                  parametersFileName, fileNames, inputFolder, outputFolder,
                  bsplineInitialGridSize,  numberOfBsplineLevel,
                  useBspline, useBsplineHigh,
                  writeDeformationFields, write3DImages )
       ) 
    {
      std:: cout << "Error reading parameter file " << std::endl;
      return EXIT_FAILURE;
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
  std::vector< std::string > transformNames(transformLevels);
  
  // Generate the transform filenames
  for(int i=0; i<transformLevels; i++)
  {
    transformFileNames[i].resize(N);
    if( i==0 ) // Generate affine transform filenames
    {
      for( int j=0; j<N; j++)
      {
        transformFileNames[i][j] = outputFolder + "Affine/TransformFiles/" + fileNames[j];
        transformFileNames[i][j] = replaceExtension(transformFileNames[i][j], "txt" );
      }
      transformNames[i] = "Affine";
    }
    else // generate bspline names
    {
      std::ostringstream bsplineFolderName;
      bsplineFolderName << "Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1);
      transformNames[i] = bsplineFolderName.str();
      for( int j=0; j<N; j++)
      {
        transformFileNames[i][j] = outputFolder + bsplineFolderName.str() + "/TransformFiles/" + fileNames[j];
        transformFileNames[i][j] = replaceExtension(transformFileNames[i][j], "txt");
      }
    }

  }


  // Resample the images
  // Read the input transforms and compute the dice measure
  for(int i=-1; i<transformLevels; i++)
  {
    // typedef for transformation types
    typedef itk::Transform< double, Dimension,Dimension >  TransformType;
    typedef itk::AffineTransform< double, Dimension >  AffineTransformType;
    typedef itk::BSplineDeformableTransform< double,
                                             Dimension,
                                             3 >     BSplineTransformType;


    std::vector< TransformType::Pointer >   transformArray(N);
    std::vector< BSplineTransformType::Pointer >   bsplineTransformArray(N);
    std::vector< AffineTransformType::Pointer >    affineTransformArray(N);

    typedef BSplineTransformType::ParametersType BsplineParametersType;
    std::vector< BsplineParametersType > bsplineParametersArray(N);

    for(int j=0; j<N; j++)
    {
      // Typedef for reader
      typedef itk::TransformFileReader    TransformFileReader;
      typedef TransformFileReader::TransformListType   TransformListType;
      
      // Create reader factories
      itk::TransformFactoryBase::Pointer f = itk::TransformFactoryBase::GetFactory();
      affineTransformArray[j] = AffineTransformType::New();
      f->RegisterTransform(affineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           affineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           affineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           1,
                           itk::CreateObjectFunction<AffineTransformType>::New());

      bsplineTransformArray[j] = BSplineTransformType::New();
      f->RegisterTransform(bsplineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           bsplineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           bsplineTransformArray[j]->GetTransformTypeAsString().c_str(),
                           1,
                           itk::CreateObjectFunction<BSplineTransformType>::New());
      // Inpute image, apply identity tranform
      if( i == -1)
      {
        typedef itk::IdentityTransform< double, Dimension >  IdentityTransformType;
        
        transformArray[j] = IdentityTransformType::New();
      }
      else if( i == 0)
      {
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

    }

    // Create the output folders
    std::string currentFolderName;
    if( i==-1)
    {
      currentFolderName = outputFolder + "InputImage/";
    }
    else if(i==0)
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
    

    typedef itk::Vector<double, Dimension> DeformationPixelType;
    typedef itk::Image<DeformationPixelType, 3> DeformationImageType;
    typedef itk::DeformationFieldFromTransform<DeformationImageType, double> DeformationSourceType;

    DeformationSourceType::Pointer deformationSource = DeformationSourceType::New(); 
    deformationSource->SetOutputRegion(imageReaderArray[0]->GetOutput()->GetLargestPossibleRegion());
    deformationSource->SetOutputOrigin(imageReaderArray[0]->GetOutput()->GetOrigin());
    deformationSource->SetOutputSpacing(imageReaderArray[0]->GetOutput()->GetSpacing());

    typedef itk::ImageFileWriter<DeformationImageType> DeformationWriter;
    DeformationWriter::Pointer writer = DeformationWriter::New();

    typedef itk::IterativeInverseDeformationFieldImageFilter<DeformationImageType, DeformationImageType> DeformationInverterType;
    DeformationInverterType::Pointer deformationinvert = DeformationInverterType::New();
    deformationinvert->SetNumberOfIterations(100);
    deformationinvert->SetStopValue(0.5);

    for(int j=0; j<N; j++)
    {

      // Write visualization images for deformation fields
      if(Dimension == 3 && i != -1)
      {
        std::cout << "Computing deformation [" << j+1 << "/" << fileNames.size() << "]\n";
        // Create deformation image
        deformationSource->SetTransform(transformArray[j]);
        deformationSource->Update();
        
        // Compute inverse
        std::cout << "Computing inverse [" << j+1 << "/" << fileNames.size() << "]\n";
        deformationinvert->SetInput(deformationSource->GetOutput());
        deformationinvert->Update();

        // Generate the outputfilename and write the output
        itksys::SystemTools::MakeDirectory( (currentFolderName+"DeformationImage/").c_str() );
        writer->SetFileName( (currentFolderName+"DeformationImage/forward-"+fileNames[j]).c_str() );
        writer->SetInput( deformationSource->GetOutput() );
        writer->Update();

        writer->SetFileName( (currentFolderName+"DeformationImage/inverse-"+fileNames[j]).c_str() );
        writer->SetInput( deformationinvert->GetOutput() );
        writer->Update();

      }
      
    }
    
  } // End of transform levels [i]

  return EXIT_SUCCESS;
}

