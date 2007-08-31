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

#include "itkNaryFunctorImageFilter.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>


//Define the global types for image type
#define PixelType unsigned short
#define InternalPixelType float
#define Dimension 3

#include "itkTransformFileReader.h"

#include <itksys/SystemTools.hxx>

using namespace std;
int getCommandLine(       int argc, char *initFname, vector<string>& fileNames, string& inputFolder, string& outputFolder,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          string& useBspline, string& useBsplineHigh,
                          string& writeDeformationFields, string& write3DImages )
{


  ifstream initFile(initFname);
  if( initFile.fail() )
  {
    std::cout << "could not open file: " << initFname << std::endl;
    return EXIT_FAILURE;
  }

  while( !initFile.eof() )
  {
    
    string dummy;
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
  if( argc < 3 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  <filenames.init>  <parameters.init>" << endl;
    return EXIT_FAILURE;
  }

  // Input Parameter declarations
  vector<string> fileNames;
  string inputFolder;
  string outputFolder;
  string writeDeformationFields = "off";
  string write3DImages = "off";
  
  int bsplineInitialGridSize = 4;
  int numberOfBsplineLevel = 0;
    
  string useBspline("off");
  string useBsplineHigh("off");
  

    //Get the command line arguments
  for(int i=1; i<argc; i++)
  {
    if ( getCommandLine(
                  argc, argv[i], fileNames, inputFolder, outputFolder,
                  bsplineInitialGridSize,  numberOfBsplineLevel,
                  useBspline, useBsplineHigh,
                  writeDeformationFields, write3DImages )
       ) 
    {
      std:: cout << "Error reading parameter file " << std::endl;
      return EXIT_FAILURE;
    }
  }

  const int N = fileNames.size();

  if(N<2)
  {
     cout << "Not enough filenames" << endl;
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
    string fname = inputFolder + fileNames[i];
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
  vector< vector < string > >  transformFileNames(transformLevels);
  vector< string > transformNames(transformLevels);
  
  // Generate the transform filenames
  for(int i=0; i<transformLevels; i++)
  {
    transformFileNames[i].resize(N);
    if( i==0 ) // Generate affine transform filenames
    {
      for( int j=0; j<N; j++)
      {
        transformFileNames[i][j] = outputFolder + "Affine/TransformFiles/" + fileNames[j];
        transformFileNames[i][j].replace(transformFileNames[i][j].size()-3, 3, "txt" );
      }
      transformNames[i] = "Affine";
    }
    else // generate bspline names
    {
      ostringstream bsplineFolderName;
      bsplineFolderName << "Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1);
      transformNames[i] = bsplineFolderName.str();
      for( int j=0; j<N; j++)
      {
        transformFileNames[i][j] = outputFolder + bsplineFolderName.str() + "/TransformFiles/" + fileNames[j];
        transformFileNames[i][j].replace(transformFileNames[i][j].size()-3, 3, "txt" );

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
    typedef itk::BSplineDeformableTransformOpt< double,
                                             Dimension,
                                             3 >     BSplineTransformType;


    std::vector< TransformType::Pointer >   transformArray(N);
    std::vector< BSplineTransformType::Pointer >   bsplineTransformArray(N);
    std::vector< AffineTransformType::Pointer >    affineTransformArray(N);

    typedef BSplineTransformType::ParametersType BsplineParametersType;
    std::vector< BsplineParametersType > bsplineParametersArray(N);

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
    string currentFolderName;
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
      ostringstream bsplineFolderName;
      bsplineFolderName << "Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1) << "/";
      currentFolderName = outputFolder + bsplineFolderName.str();
    }
    itksys::SystemTools::MakeDirectory( currentFolderName.c_str() );
    
    // Compute mean and standard deviation
    typedef itk::NaryFunctorImageFilter< ImageType,  ImageType,  MEAN > NaryMEANImageFilter;
    NaryMEANImageFilter::Pointer naryMeanImageFilter = NaryMEANImageFilter::New();

    typedef itk::NaryFunctorImageFilter< ImageType,  ImageType,  VARIANCE > NarySTDImageFilter;
    NarySTDImageFilter::Pointer narySTDImageFilter = NarySTDImageFilter::New();

    
    ImageType::Pointer deformationImage = ImageType::New();

    for(int j=0; j<N; j++)
    {
      imageResampleArray[j] = ResampleFilterType::New();

      typedef itk::LinearInterpolateImageFunction< ImageType, double >  InterpolatorType;
      InterpolatorType::Pointer interpolator = InterpolatorType::New();
      imageResampleArray[j]->SetInterpolator( interpolator );
      imageResampleArray[j]->SetTransform( transformArray[j] );
      
      imageReaderArray[j]->Update();
      
      ImageType::Pointer imagePointer = imageReaderArray[j]->GetOutput();
      ResampleFilterType::OriginPointType  origin = imageReaderArray[j]->GetOutput()->GetOrigin();
      imagePointer->SetOrigin(origin);

      imageResampleArray[j]->SetInput( imagePointer );
      imageResampleArray[j]->SetSize(    imagePointer->GetLargestPossibleRegion().GetSize() );
      imageResampleArray[j]->SetOutputOrigin(  imagePointer->GetOrigin() );
      imageResampleArray[j]->SetOutputSpacing( imagePointer->GetSpacing() );
      imageResampleArray[j]->SetOutputDirection( imagePointer->GetDirection());
      imageResampleArray[j]->SetDefaultPixelValue( 0 );
      
      naryMeanImageFilter->SetInput(j,imageResampleArray[j]->GetOutput());
      narySTDImageFilter->SetInput(j,imageResampleArray[j]->GetOutput());

      // Write the output images
      WriterType::Pointer writer= WriterType::New();
      writer->SetInput(imageResampleArray[j]->GetOutput());

      itksys::SystemTools::MakeDirectory( (currentFolderName+"Images/").c_str() );
      writer->SetFileName((currentFolderName+"Images/"+fileNames[j]).c_str());
      writer->SetImageIO(imageReaderArray[j]->GetImageIO());
      
      if(write3DImages == "on")
      {
        cout << "Writing " << (currentFolderName+"Images/"+fileNames[j]).c_str() << endl;
        writer->Update();
      }

      // Extract the central slice
      typedef itk::Image< unsigned char, 2 >    SliceImageType;
      typedef itk::ImageFileWriter< SliceImageType >  SliceWriterType;
      SliceWriterType::Pointer  sliceWriter = SliceWriterType::New();
        
      // Filter to extract a slice from an image
      typedef itk::ExtractImageFilter< ImageType, SliceImageType > SliceExtractFilterType;
      SliceExtractFilterType::Pointer sliceExtractFilter = SliceExtractFilterType::New();

      //Write the central slice
      ImageType::SizeType size = imageReaderArray[j]->GetOutput()->GetLargestPossibleRegion().GetSize();
      ImageType::IndexType start = imageReaderArray[j]->GetOutput()->GetLargestPossibleRegion().GetIndex();
      /** change here for slice direction */
      start[1] = size[1]/2;
      size[1] = 0;
      // these are user specific
      //start[0] = 106;
      //size[0] = 0;

        
      ImageType::RegionType extractRegion;
      extractRegion.SetSize(  size  );
      extractRegion.SetIndex( start );
      sliceExtractFilter->SetExtractionRegion( extractRegion );
      
      sliceExtractFilter->SetInput( imageResampleArray[j]->GetOutput() );
      sliceWriter->SetInput( sliceExtractFilter->GetOutput() );

       // write mean image
      itksys::SystemTools::MakeDirectory( (currentFolderName+"Slices/").c_str() );
      ostringstream sliceStream;
      sliceStream << j;
      string sliceName = currentFolderName+"Slices/"+sliceStream.str()+".jpg";
      sliceWriter->SetFileName( sliceName.c_str() );
      
      cout << "Writing " << sliceName.c_str() << endl; 
      sliceWriter->Update();     

      // Write visualization images for deformation fields
      if(Dimension == 3 && i != -1)
      {

        //Create a toy image
        if(j==0)
        {
          deformationImage->SetRegions( imageReaderArray[j]->GetOutput()->GetLargestPossibleRegion() );
          deformationImage->CopyInformation( imageReaderArray[j]->GetOutput() );
          deformationImage->Allocate();
          deformationImage->FillBuffer( 0 );


          // Create perpendicular planes in the deformationImage
          typedef itk::ImageRegionIteratorWithIndex< ImageType > IteratorWithIndexType;
          IteratorWithIndexType defIt( deformationImage, deformationImage->GetRequestedRegion() );
          ImageType::IndexType index;
          for ( defIt.GoToBegin(); !defIt.IsAtEnd(); ++defIt)
          {
            index = defIt.GetIndex();
            if(index[2]%8 == 0 || index[0]%8 ==0 )
            {
              defIt.Set( 255 );
            }
          }
        }

        imageResampleArray[j]->SetInput( deformationImage );

        // Generate the outputfilename and write the output
        if(writeDeformationFields =="on")
        {
          itksys::SystemTools::MakeDirectory( (currentFolderName+"DeformationImage/").c_str() );
          writer->SetFileName( (currentFolderName+"DeformationImage/"+fileNames[j]).c_str() );
          writer->Update();
        }

        // Extract the central slices of the the deformation field
        itksys::SystemTools::MakeDirectory( (currentFolderName+"DeformationSlices/").c_str() );
        sliceExtractFilter->SetInput( imageResampleArray[j]->GetOutput() );
        sliceWriter->SetInput( sliceExtractFilter->GetOutput() );
        ostringstream sliceStream;
        sliceStream << j;
        string sliceName = currentFolderName+"DeformationSlices/"+sliceStream.str()+".jpg";
        sliceWriter->SetFileName( sliceName.c_str() );
        cout << "Writing " << sliceName << endl;
        sliceWriter->Update();

        imageResampleArray[j]->SetInput( imagePointer );

      }
      
    }


    
    // Set the folder name
    string meanFolder = currentFolderName + "MeanImages/";
    string stdFolder = currentFolderName  + "STDImages/";

    itksys::SystemTools::MakeDirectory( meanFolder.c_str() );
    itksys::SystemTools::MakeDirectory( stdFolder.c_str() );


    // Write the mean image
    WriterType::Pointer writer= WriterType::New();
    writer->SetInput(naryMeanImageFilter->GetOutput());
    writer->SetFileName((meanFolder+"Mean.mhd").c_str());
    
    if(write3DImages == "on")
    {
      cout << "Writing " << (meanFolder+"Mean.mhd").c_str() << endl;
      writer->Update();
    }
    
    // Extract the central slice
    typedef itk::Image< unsigned char, 2 >    SliceImageType;
    typedef itk::ImageFileWriter< SliceImageType >  SliceWriterType;
    SliceWriterType::Pointer  sliceWriter = SliceWriterType::New();
        
        // Filter to extract a slice from an image
    typedef itk::ExtractImageFilter< ImageType, SliceImageType > SliceExtractFilterType;
    SliceExtractFilterType::Pointer sliceExtractFilter = SliceExtractFilterType::New();

    //Write the central slice
    ImageType::SizeType size = imageReaderArray[0]->GetOutput()->GetLargestPossibleRegion().GetSize();
    ImageType::IndexType start = imageReaderArray[0]->GetOutput()->GetLargestPossibleRegion().GetIndex();
    
    /** change here for slice direction */
    start[1] = size[1]/2;
    size[1] = 0;
    // these are user specific
    //start[0] = 106;
    //size[0] = 0;
                
    ImageType::RegionType extractRegion;
    extractRegion.SetSize(  size  );
    extractRegion.SetIndex( start );
    sliceExtractFilter->SetExtractionRegion( extractRegion );
      
    sliceExtractFilter->SetInput( naryMeanImageFilter->GetOutput() );
    sliceWriter->SetInput( sliceExtractFilter->GetOutput() );

    // write mean image
    cout << "Writing " << (meanFolder+"MeanSlice.jpg").c_str() << endl;
    sliceWriter->SetFileName( (meanFolder+"MeanSlice.jpg").c_str() );
    sliceWriter->Update();     


    writer->SetInput(narySTDImageFilter->GetOutput());
    writer->SetFileName((stdFolder+"STD.mhd").c_str());
    if(write3DImages == "on")
    {
      cout << "Writing " << (stdFolder+"STD.mhd").c_str() << endl;
      writer->Update();
    }    
    // write std image
    sliceExtractFilter->SetInput( narySTDImageFilter->GetOutput() );
    sliceWriter->SetInput( sliceExtractFilter->GetOutput() );

    cout << "Writing " << (stdFolder+"STDSlice.jpg").c_str() << endl;
    sliceWriter->SetFileName( (stdFolder+"STDSlice.jpg").c_str() );
    sliceWriter->Update();

    
  } // End of transform levels

  return EXIT_SUCCESS;
}

