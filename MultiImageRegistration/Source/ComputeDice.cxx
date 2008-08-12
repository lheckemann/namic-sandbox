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
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkImageRegionIterator.h"

#include "itkNaryFunctorImageFilter.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>


//Define the global types for image type
#define PixelType unsigned char
#define Dimension 3

#include "itkTransformFileReader.h"

#include <itksys/SystemTools.hxx>

using namespace std;

std::string replaceExtension(const std::string oldname, const std::string extension)
{
  return oldname.substr(0, oldname.rfind(".")) + "." + extension;
}

int getCommandLine(       int argc, char *initFname, vector<string>& fileNames, string& inputFolder, string& outputFolder,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          string& useBspline, string& useBsplineHigh,
                          string& labelFileFolder, vector<string>& labelFileNames, string& labelType )
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
    if(dummy == "-labelFolder")
    {
      initFile >> dummy;
      labelFileFolder = dummy;
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
    else if (dummy == "-f")
    {
      initFile >> dummy;
      fileNames.push_back(dummy); // get file name
    }
    else if (dummy == "-labelType")
    {
      initFile >> dummy;
      labelType = dummy; // get file name
    }
    else if (dummy == "-lf")
    {
      initFile >> dummy;
      labelFileNames.push_back(dummy); // get file name
    }

  }

  initFile.close();
  return EXIT_SUCCESS;
}

// And function
class AND
{
public:
  AND() {m_Number=0;m_Number2=0;};
  ~AND() {};
  bool operator!=( const AND & ) const
  {
    return false;
  }
  bool operator==( const AND & other ) const
  {
    return !(*this != other);
  }
  inline PixelType operator()( const std::vector< PixelType > pixelStack)
  {
    PixelType count = 0;
    for(unsigned int i=0; i<pixelStack.size(); i++)
    {
      if( pixelStack[i] == m_Number || pixelStack[i] == m_Number2 )
      {
        count++;
      }
    }
    return count;
  }

  PixelType m_Number;
  PixelType m_Number2;

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
  int bsplineInitialGridSize = 4;
  int numberOfBsplineLevel = 0;
    
  string useBspline("off");
  string useBsplineHigh("off");
  
  string labelFileFolder;
  vector<string> labelFileNames;
      

  string labelType("ICC");
    //Get the command line arguments
  for(int i=1; i<argc; i++)
  {
    if ( getCommandLine(
                  argc, argv[i], fileNames, inputFolder, outputFolder,
                  bsplineInitialGridSize,  numberOfBsplineLevel,
                  useBspline, useBsplineHigh,
                  labelFileFolder, labelFileNames, labelType )
       ) 
    {
      std:: cout << "Error reading parameter file " << std::endl;
      return EXIT_FAILURE;
    }
  }

  const int N = fileNames.size();

  // check whether all labels exist
  if( (unsigned int)N != labelFileNames.size())
  {
    cout << " Number of label files do not match number of input files " << endl;
    return EXIT_FAILURE;
  }

  typedef itk::Image< PixelType,  Dimension >   ImageType;

  typedef itk::ImageFileReader< ImageType  >  ReaderType;
  typedef itk::ImageFileWriter< ImageType >  WriterType;

  // Read the input labels
  std::vector< ReaderType::Pointer > labelReaderArray(N);
  for(int i=0; i<N; i++)
  {
    labelReaderArray[i] = ReaderType::New();
    string fname = labelFileFolder + labelFileNames[i];
    labelReaderArray[i]->SetFileName(fname.c_str());
    std::cout << "Reading  " << fname.c_str() << std::endl;
    labelReaderArray[i]->Update();
  }

  // Read intensity images
  std::vector< ReaderType::Pointer > imageReaderArray(N);
  for(int i=0; i<N; i++)
  {
    imageReaderArray[i] = ReaderType::New();
    string fname = inputFolder + fileNames[i];
    imageReaderArray[i]->SetFileName(fname.c_str());
    std::cout << "Reading  " << fname.c_str() << std::endl; 
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
        transformFileNames[i][j] = replaceExtension(transformFileNames[i][j], "txt");
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
        transformFileNames[i][j] = replaceExtension(transformFileNames[i][j], "txt");

      }
    }

  }

  ofstream dicePredFile;
  if(labelType == "ICC")
  {
    dicePredFile.open(( (outputFolder + "DicePredictionICC.txt").c_str() ));
  }
  else
  {
    dicePredFile.open(( (outputFolder + "DicePredictionHandLabels.txt").c_str() ));
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
      

      if( i > -1)
      {
        cout << "Reading Transform" << transformFileNames[i][j] << endl;
      }

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


      resampleArray[j] = ResampleFilterType::New();

      typedef itk::NearestNeighborInterpolateImageFunction< ImageType, double >  InterpolatorType;
      InterpolatorType::Pointer interpolator = InterpolatorType::New();
      resampleArray[j]->SetInterpolator( interpolator );
      resampleArray[j]->SetTransform( transformArray[j] );
      
      labelReaderArray[j]->Update();
      
      ImageType::Pointer imagePointer = labelReaderArray[j]->GetOutput();

      resampleArray[j]->SetInput( imagePointer );
      resampleArray[j]->SetSize(    imagePointer->GetLargestPossibleRegion().GetSize() );
      resampleArray[j]->SetOutputOrigin(  imagePointer->GetOrigin() );
      resampleArray[j]->SetOutputSpacing( imagePointer->GetSpacing() );
      resampleArray[j]->SetOutputDirection( imagePointer->GetDirection());
      resampleArray[j]->SetDefaultPixelValue( 0 );
      resampleArray[j]->Update();

      // write out the resampled label files
      WriterType::Pointer writer = WriterType::New();
      string fname = outputFolder;
      if(i==-1)
      {
        fname += "LabelsResampled/InputImage/";
        itksys::SystemTools::MakeDirectory( fname.c_str() );
        fname += labelFileNames[j];
      }
      else if(i==0)
      {
        fname += "LabelsResampled/Affine/";
        itksys::SystemTools::MakeDirectory( fname.c_str() );
        fname += labelFileNames[j];
      }
      else
      {
        ostringstream bsplineFolderName;
        bsplineFolderName << "LabelsResampled/Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1) << "/";
        itksys::SystemTools::MakeDirectory( (fname+bsplineFolderName.str()).c_str() );
        fname += bsplineFolderName.str() + labelFileNames[j];
      }
      writer->SetFileName(fname.c_str());
      writer->SetImageIO(labelReaderArray[0]->GetImageIO());
      writer->SetInput(resampleArray[j]->GetOutput());
      writer->Update();
      
    }

    // Compute the dice measures
    typedef itk::NaryFunctorImageFilter< ImageType,  ImageType,  AND > NaryANDImageFilter;
    NaryANDImageFilter::Pointer naryANDImageFilter = NaryANDImageFilter::New();

    for(int j=0; j<N; j++)
    {
      naryANDImageFilter->SetInput(j,resampleArray[j]->GetOutput());
    }

    cout << "Computing dice measure " << endl;
    if(labelType == "ICC")
    {
      for(int j=0; j<3 ; j++)
      {
        // Output message
        std::cout << "Computing label " << j << std::endl;
        
        // Set the labels
        int currentNumber;
        int currentNumber2;

        if(j ==0)
        {
          // White Matter
          naryANDImageFilter->GetFunctor().m_Number = 23;
          naryANDImageFilter->GetFunctor().m_Number2 = 159;
          naryANDImageFilter->Modified();
          currentNumber = 23;
          currentNumber2 = 159;
        }
        else if(j==1)
        {
          // White Matter
          naryANDImageFilter->GetFunctor().m_Number = 25;
          naryANDImageFilter->GetFunctor().m_Number2 = 255;
          naryANDImageFilter->Modified();
          currentNumber = 25;
          currentNumber2 = 255;
        }
        else
        {
          // White Matter
          naryANDImageFilter->GetFunctor().m_Number = 30;
          naryANDImageFilter->GetFunctor().m_Number2 = 127;
          naryANDImageFilter->Modified();
          currentNumber = 30;
          currentNumber2 = 127;
        }
        naryANDImageFilter->Update();

        typedef itk::ImageRegionIterator<ImageType>  IteratorType;
    

        // Write the overlap image
        WriterType::Pointer writer= WriterType::New();
        writer->SetInput(naryANDImageFilter->GetOutput());

        // Set the file name
        string fname = outputFolder;

        if(i==-1)
        {
          ostringstream affine;
          affine << j << ".hdr";
          fname += "Labels/InputImage/";
          itksys::SystemTools::MakeDirectory( fname.c_str() );
          fname += affine.str();
        }
        else if(i==0)
        {
          ostringstream affine;
          affine << j << ".hdr";
          fname += "Labels/Affine/";
          itksys::SystemTools::MakeDirectory( fname.c_str() );
          fname += affine.str();
        }
        else
        {
          ostringstream bsplineFolderName;
          bsplineFolderName << "Labels/Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1) << "/";
          itksys::SystemTools::MakeDirectory( (fname+bsplineFolderName.str()).c_str() );
          bsplineFolderName << j << ".hdr";
          fname += bsplineFolderName.str();
        }
        writer->SetFileName(fname.c_str());
        writer->SetImageIO(labelReaderArray[0]->GetImageIO());
        writer->Update();


        
        // Extract the central slice
        typedef itk::Image< unsigned char, 2 >    SliceImageType;
        typedef itk::ImageFileWriter< SliceImageType >  SliceWriterType;
        SliceWriterType::Pointer  sliceWriter = SliceWriterType::New();
        
        // Filter to extract a slice from an image
        typedef itk::ExtractImageFilter< ImageType, SliceImageType > SliceExtractFilterType;
        SliceExtractFilterType::Pointer sliceExtractFilter = SliceExtractFilterType::New();

        //Write the central slice

        ImageType::SizeType size = naryANDImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
        ImageType::IndexType start = naryANDImageFilter->GetOutput()->GetLargestPossibleRegion().GetIndex();
        start[0] = size[0]/2;
        size[0] = 0;
        
        ImageType::RegionType extractRegion;
        extractRegion.SetSize(  size  );
        extractRegion.SetIndex( start );
        sliceExtractFilter->SetExtractionRegion( extractRegion );
      
        sliceExtractFilter->SetInput( naryANDImageFilter->GetOutput() );
        sliceWriter->SetInput( sliceExtractFilter->GetOutput() );

        string sliceName = fname;
        sliceName.replace(sliceName.size()-4, 4, "_X.tiff" );

        sliceWriter->SetFileName( sliceName.c_str() );
        sliceWriter->Update(); 

        // Write y direction
        size = naryANDImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
        start = naryANDImageFilter->GetOutput()->GetLargestPossibleRegion().GetIndex();
        start[1] = size[1]/2;
        size[1] = 0;
        extractRegion.SetSize(  size  );
        extractRegion.SetIndex( start );
        sliceExtractFilter->SetExtractionRegion( extractRegion );
        sliceName = fname;
        sliceName.replace(sliceName.size()-4, 4, "_Y.tiff" );
        sliceWriter->SetFileName( sliceName.c_str() );
        sliceWriter->Update(); 
                
        // Write y direction
        size = naryANDImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
        start = naryANDImageFilter->GetOutput()->GetLargestPossibleRegion().GetIndex();
        start[2] = size[2]/2;
        size[2] = 0;
        extractRegion.SetSize(  size  );
        extractRegion.SetIndex( start );
        sliceExtractFilter->SetExtractionRegion( extractRegion );
        sliceName = fname;
        sliceName.replace(sliceName.size()-4, 4, "_Z.tiff" );
        sliceWriter->SetFileName( sliceName.c_str() );
        sliceWriter->Update(); 

        
        // Compute prediction overlap
        dicePredFile << "Level:" << j << " ";
        for(int k=0; k<N; k++)
        {

          // compute dice
          IteratorType predIt(naryANDImageFilter->GetOutput(),naryANDImageFilter->GetOutput()->GetLargestPossibleRegion() );
          IteratorType imageIt(resampleArray[k]->GetOutput(),resampleArray[k]->GetOutput()->GetLargestPossibleRegion() );
          double predIntersection = 0.0;
          double predUnion = 0.0;
          
          imageIt.GoToBegin();
          for ( predIt.GoToBegin(); !predIt.IsAtEnd(); ++predIt)
          {
            int currentLabel;
            if(imageIt.Get() == currentNumber || imageIt.Get() == currentNumber2)
            {
              currentLabel = 1;
            }
            else
            {
              currentLabel = 0;
            }

            if( predIt.Get() - currentLabel > (N-1)*0.5  
                && (imageIt.Get() == currentNumber || imageIt.Get() == currentNumber2) )
            {
              predIntersection += 2.0;
            }

            if( predIt.Get() - currentLabel > (N-1)*0.5 )
            {
              predUnion += 1.0;
            }
            if( imageIt.Get() == currentNumber || imageIt.Get() == currentNumber2)
            {
              predUnion += 1.0;
            }
            ++imageIt;
          }
          dicePredFile << predIntersection/predUnion << " ";

        } // end k
        dicePredFile << endl;

      } // end j

    } // end if
    // Compute the overlap measures for hand labels
    else
    {
      for(int j=3; j<=10 ; j++)
      {
        // Output message
        std::cout << "Computing label " << j << std::endl;
            
        naryANDImageFilter->GetFunctor().m_Number = j;
        naryANDImageFilter->GetFunctor().m_Number2 = 255;
        naryANDImageFilter->Modified();
        naryANDImageFilter->Update();

        typedef itk::ImageRegionIterator<ImageType>  IteratorType;
    

        // Write the overlap image
        WriterType::Pointer writer= WriterType::New();
        writer->SetInput(naryANDImageFilter->GetOutput());

        // Set the file name
        string fname = outputFolder;
        if(i==-1)
        {
          ostringstream affine;
          affine << j << ".hdr";
          fname += "HandLabels/InputImage/";
          itksys::SystemTools::MakeDirectory( fname.c_str() );
          fname += affine.str();
        }
        else if(i==0)
        {
          ostringstream affine;
          affine << j << ".hdr";
          fname += "HandLabels/Affine/";
          itksys::SystemTools::MakeDirectory( fname.c_str() );
          fname += affine.str();
        }
        else
        {
          ostringstream bsplineFolderName;
          bsplineFolderName << "HandLabels/Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1) << "/";
          itksys::SystemTools::MakeDirectory( (fname+bsplineFolderName.str()).c_str() );
          bsplineFolderName << j << ".hdr";
          fname += bsplineFolderName.str();
        }
        writer->SetFileName(fname.c_str());
        writer->SetImageIO(labelReaderArray[0]->GetImageIO());
        writer->Update();


        // Compute prediction overlap
        dicePredFile << "Level:" << j << " ";
        for(int k=0; k<N; k++)
        {
          // compute dice
          IteratorType predIt(naryANDImageFilter->GetOutput(),naryANDImageFilter->GetOutput()->GetLargestPossibleRegion() );
          IteratorType imageIt(resampleArray[k]->GetOutput(),resampleArray[k]->GetOutput()->GetLargestPossibleRegion() );
          double predIntersection = 0.0;
          double predUnion = 0.0;
          
          imageIt.GoToBegin();
          for ( predIt.GoToBegin(); !predIt.IsAtEnd(); ++predIt)
          {
            int currentLabel;
            if(imageIt.Get() == j )
            {
              currentLabel = 1;
            }
            else
            {
              currentLabel = 0;
            }
            
            // intersection 90
            if( predIt.Get() - currentLabel > (N-1)*0.5 && imageIt.Get() == j)
            {
              predIntersection += 2.0;
            }

            // intersection 90
            if( predIt.Get() - currentLabel > (N-1)*0.5 )
            {
              predUnion += 1.0;
            }
            if( imageIt.Get() == j)
            {
              predUnion += 1.0;
            }
            ++imageIt;
          }
          dicePredFile << predIntersection/predUnion << " ";

        } // end k
        dicePredFile << endl;

      }
    }




    
  } // End of transform levels

  return EXIT_SUCCESS;
}

