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

#include "itkAffineTransform.h"
#include "UserBSplineDeformableTransform.h"
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
#define PixelType unsigned short
#define InternalPixelType float
#define Dimension 3

#include "itkTransformFileReader.h"

#include <itksys/SystemTools.hxx>

using namespace std;
int getCommandLine(       int argc, char *initFname, vector<string>& fileNames, string& inputFolder, string& outputFolder,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          string& useBspline, string& useBsplineHigh,
                          string& labelFileFolder, vector<string>& labelFileNames, string& labelType )
{


  ifstream initFile(initFname);
  if( initFile.fail() )
  {
    std::cout << "could not open file: " << initFname << std::endl;
    return 1;
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
  return 0;
}

// And function
class AND
{
public:
  AND() {m_Number=0;};
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
    for(int i=0; i<pixelStack.size(); i++)
    {
      if( pixelStack[i] == m_Number)
      {
        count++;
      }
    }
    return count;
  }

  PixelType m_Number;
};

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
    for(int i=0; i<pixelStack.size(); i++)
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
    for(int i=0; i<pixelStack.size(); i++)
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
      return 1;
    }
  }

  const unsigned int N = fileNames.size();

  // check whether all labels exist
  if( N != labelFileNames.size())
  {
    cout << " Number of label files do not match number of input files " << endl;
    return 1;
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
    labelReaderArray[i]->Update();
  }

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

  ofstream output;
  ofstream dice50;
  ofstream dice90;
  if(labelType == "ICC")
  {
    output.open(( (outputFolder + "DiceMeasuresICC.txt").c_str() ));
    dice50.open(( (outputFolder + "DicePredictionICC50.txt").c_str() ));
    dice90.open(( (outputFolder + "DicePredictionICC90.txt").c_str() ));
  }
  else
  {
    output.open(( (outputFolder + "DiceMeasuresHandLabels.txt").c_str() ));
    dice50.open(( (outputFolder + "DicePredictionHandLabels50.txt").c_str() ));
    dice90.open(( (outputFolder + "DicePredictionHandLabels90.txt").c_str() ));
  }
  // Resample the images
  // Read the input transforms and compute the dice measure
  for(int i=0; i<transformLevels; i++)
  {
    // typedef for transformation types
    typedef itk::Transform< double, Dimension,Dimension >  TransformType;
    typedef itk::AffineTransform< double, Dimension >  AffineTransformType;
    typedef itk::UserBSplineDeformableTransform< double,
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
      

      cout << "Reading Transform" << transformFileNames[i][j] << endl;

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
      
      if( i == 0)
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
      ResampleFilterType::OriginPointType  origin = labelReaderArray[j]->GetOutput()->GetOrigin();
      origin[0] = 111.5625;
      origin[1] = 111.5625;
      origin[2] = -138.0;
      imagePointer->SetOrigin(origin);

      resampleArray[j]->SetInput( imagePointer );
      resampleArray[j]->SetSize(    imagePointer->GetLargestPossibleRegion().GetSize() );
      resampleArray[j]->SetOutputOrigin(  imagePointer->GetOrigin() );
      resampleArray[j]->SetOutputSpacing( imagePointer->GetSpacing() );
      resampleArray[j]->SetOutputDirection( imagePointer->GetDirection());
      resampleArray[j]->SetDefaultPixelValue( 0 );
      
      cout << "Updating " << labelFileFolder + labelFileNames[j] << endl;
      //for(int k=0; k<100; k++)
      //{
      //  cout << transformArray[j]->GetParameters()[k] << " " ;
      //}
      //cout << transformArray[j]->GetFixedParameters() << endl;
      
      resampleArray[j]->Update();

      WriterType::Pointer writer = WriterType::New();
      writer->SetFileName("temp3.hdr");
      writer->SetImageIO(labelReaderArray[i]->GetImageIO());
      writer->SetInput(resampleArray[j]->GetOutput());
      if(j==0 && i==1)
      {
        //writer->Update();
      }
      
    }

    // Compute the dice measures
    typedef itk::NaryFunctorImageFilter< ImageType,  ImageType,  AND > NaryANDImageFilter;
    NaryANDImageFilter::Pointer naryANDImageFilter = NaryANDImageFilter::New();

    for(int j=0; j<N; j++)
    {
      naryANDImageFilter->SetInput(j,resampleArray[j]->GetOutput());
    }

    NaryANDImageFilter::Pointer naryANDPredictionImageFilter = NaryANDImageFilter::New();
    for(int j=0; j<N-1; j++)
    {
      naryANDPredictionImageFilter->SetInput(j,resampleArray[j]->GetOutput());
    }

    cout << "Computing dice measure " << endl;
    //output << transformNames[i] << "\t";
    if(labelType == "ICC")
    {
      for(int j=0; j<3 ; j++)
      {
        // Set the labels
        int currentNumber;
        if(j ==0)
        {
          // White Matter
          naryANDImageFilter->GetFunctor().m_Number = 23;
          naryANDImageFilter->Modified();
          naryANDPredictionImageFilter->GetFunctor().m_Number = 23;
          naryANDPredictionImageFilter->Modified();
          currentNumber = 23;
        }
        else if(j==1)
        {
          // White Matter
          naryANDImageFilter->GetFunctor().m_Number = 25;
          naryANDImageFilter->Modified();
          naryANDPredictionImageFilter->GetFunctor().m_Number = 25;
          naryANDPredictionImageFilter->Modified();
          currentNumber = 25;
        }
        else
        {
          // White Matter
          naryANDImageFilter->GetFunctor().m_Number = 30;
          naryANDImageFilter->Modified();
          naryANDPredictionImageFilter->GetFunctor().m_Number = 30;
          naryANDPredictionImageFilter->Modified();
          currentNumber = 30;
        }
        //naryANDImageFilter->Update();

        typedef itk::ImageRegionIterator<ImageType>  IteratorType;
    

        // Write the overlap image
        WriterType::Pointer writer= WriterType::New();
        writer->SetInput(naryANDImageFilter->GetOutput());

        // Set the file name
        string fname = outputFolder;

        if(i==0)
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
        start[0] = 106;
        size[0] = 0;

        
        ImageType::RegionType extractRegion;
        extractRegion.SetSize(  size  );
        extractRegion.SetIndex( start );
        sliceExtractFilter->SetExtractionRegion( extractRegion );
      
        sliceExtractFilter->SetInput( naryANDImageFilter->GetOutput() );
        sliceWriter->SetInput( sliceExtractFilter->GetOutput() );

        string sliceName = fname;
        sliceName.replace(sliceName.size()-3, 3, "bmp" );

        sliceWriter->SetFileName( sliceName.c_str() );
        sliceWriter->Update(); 

        
        IteratorType andIt(naryANDImageFilter->GetOutput(),naryANDImageFilter->GetOutput()->GetLargestPossibleRegion() );

        // Compute The histogram
        std::vector<double> histogram(N,0.0);
        for ( andIt.GoToBegin(); !andIt.IsAtEnd(); ++andIt)
        {
          if(andIt.Get())
          {
            histogram[andIt.Get()-1] += 1.0;
          }
        }

        
        // Write ROC curve
        output << "Level:" << i << " ";
        double cumSum = 0.0;
        for(int k=0; k<N; k++)
        {
          cumSum += histogram[N-1-k];
          output << cumSum << " ";
        }
        output << endl;


        // Write number of nonzero points
        std::vector<IteratorType> sumIterators(N);
        for(int k=0; k<N; k++)
        {
          sumIterators[k] = IteratorType(resampleArray[k]->GetOutput(),resampleArray[k]->GetOutput()->GetLargestPossibleRegion() );
        }

        output << "AreaSum: " ;
        for(int k=0; k<N; k++)
        {
          double areaSum = 0.0;
          for ( sumIterators[k].GoToBegin(); !sumIterators[k].IsAtEnd(); ++sumIterators[k])
          {
            if(sumIterators[k].Get() == currentNumber)
            {
              areaSum += 1.0;
            }
          }
          output << areaSum << " ";
        }
        output << endl;


        // Compute prediction overlap
        dice50 << "Level:" << j << " ";
        dice90 << "Level:" << j << " ";
        for(int k=0; k<N; k++)
        {
          int count = 0;
          for(int l=0; l<N; l++)
          {
            if( l != k)
            {
              naryANDPredictionImageFilter->SetInput(count,resampleArray[l]->GetOutput());
              count++;
            }
          }
          naryANDPredictionImageFilter->Update();

          // compute dice
          IteratorType predIt(naryANDPredictionImageFilter->GetOutput(),naryANDPredictionImageFilter->GetOutput()->GetLargestPossibleRegion() );
          IteratorType imageIt(resampleArray[k]->GetOutput(),resampleArray[k]->GetOutput()->GetLargestPossibleRegion() );
          double intersection50 = 0.0;
          double intersection90 = 0.0;
          double union50 = 0.0;
          double union90 = 0.0;
          
          imageIt.GoToBegin();
          for ( predIt.GoToBegin(); !predIt.IsAtEnd(); ++predIt)
          {
            // intersection 50
            if( imageIt.Get() == currentNumber)
            {
              intersection50 += predIt.Get()/(double)(N-1);
            }
            // intersection 90
            if( predIt.Get() > (N-1)*0.5 && imageIt.Get() == currentNumber)
            {
              intersection90 += 1.0;
            }

            // union 50
            if( imageIt.Get() == currentNumber)
            {
              union50 += 1.0;
            }
            // intersection 90
            if( predIt.Get() > (N-1)*0.5 || imageIt.Get() == currentNumber)
            {
              union90 += 1.0;
            }
            ++imageIt;
          }
          dice50 << intersection50/union50 << " ";
          dice90 << intersection90/union90 << " ";

        } // end k
        dice50 << endl;
        dice90 << endl;

      } // end j

    } // end if
    // Compute the overlap measures for hand labels
    else
    {
      for(int j=3; j<=10 ; j++)
      {
        // Set the labels

        // White Matter
        naryANDPredictionImageFilter->GetFunctor().m_Number = j;
        naryANDPredictionImageFilter->Modified();
        naryANDPredictionImageFilter->Update();
        naryANDImageFilter->GetFunctor().m_Number = j;
        naryANDImageFilter->Modified();
        naryANDImageFilter->Update();

        typedef itk::ImageRegionIterator<ImageType>  IteratorType;
    

        // Write the overlap image
        WriterType::Pointer writer= WriterType::New();
        writer->SetInput(naryANDImageFilter->GetOutput());

        // Set the file name
        string fname = outputFolder;
        if(i==0)
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


        
        IteratorType andIt(naryANDImageFilter->GetOutput(),naryANDImageFilter->GetOutput()->GetLargestPossibleRegion() );

        // Compute The histogram
        std::vector<double> histogram(N,0.0);
        for ( andIt.GoToBegin(); !andIt.IsAtEnd(); ++andIt)
        {
          const PixelType  current = andIt.Get();
          if(current)
          {
            histogram[current-1] += 1.0;
          }
        }

        
        // Write ROC curve
        output << "Level:" << i << " ";
        double cumSum = 0.0;
        for(int k=0; k<N; k++)
        {
          cumSum += histogram[N-1-k];
          output << cumSum << " ";
        }
        output << endl;

        // Write number of nonzero points
        std::vector<IteratorType> sumIterators(N);
        for(int k=0; k<N; k++)
        {
          sumIterators[k] = IteratorType(resampleArray[k]->GetOutput(),resampleArray[k]->GetOutput()->GetLargestPossibleRegion() );
        }

        output << "AreaSum: " ;
        for(int k=0; k<N; k++)
        {
          double areaSum = 0.0;
          for ( sumIterators[k].GoToBegin(); !sumIterators[k].IsAtEnd(); ++sumIterators[k])
          {
            const PixelType  current = sumIterators[k].Get();
            if(current == j)
            {
              areaSum += 1.0;
            }
          }
          output << areaSum << " ";
        }
        output << endl;


        // Compute prediction overlap
        dice50 << "Level:" << j << " ";
        dice90 << "Level:" << j << " ";
        for(int k=0; k<N; k++)
        {
          int count = 0;
          for(int l=0; l<N; l++)
          {
            if( l != k)
            {
              naryANDPredictionImageFilter->SetInput(count,resampleArray[l]->GetOutput());
              count++;
            }
          }
          naryANDPredictionImageFilter->Update();

          // compute dice
          IteratorType predIt(naryANDPredictionImageFilter->GetOutput(),naryANDPredictionImageFilter->GetOutput()->GetLargestPossibleRegion() );
          IteratorType imageIt(resampleArray[k]->GetOutput(),resampleArray[k]->GetOutput()->GetLargestPossibleRegion() );
          double intersection50 = 0.0;
          double intersection90 = 0.0;
          double union50 = 0.0;
          double union90 = 0.0;
          
          imageIt.GoToBegin();
          for ( predIt.GoToBegin(); !predIt.IsAtEnd(); ++predIt)
          {
            // intersection 50
            if( imageIt.Get() == j)
            {
              intersection50 += predIt.Get()/(double)(N-1);
            }
            // intersection 90
            if( predIt.Get() > (N-1)*0.5 && imageIt.Get() == j)
            {
              intersection90 += 1.0;
            }

            // union 50
            if( imageIt.Get() == j)
            {
              union50 += 1.0;
            }
            // intersection 90
            if( predIt.Get() > (N-1)*0.5 || imageIt.Get() == j)
            {
              union90 += 1.0;
            }
            ++imageIt;
          }
          dice50 << intersection50/union50 << " ";
          dice90 << intersection90/union90 << " ";

        } // end k
        dice50 << endl;
        dice90 << endl;

      }
    }



    // Compute mean and standard deviation
    typedef itk::NaryFunctorImageFilter< ImageType,  ImageType,  MEAN > NaryMEANImageFilter;
    NaryMEANImageFilter::Pointer naryMeanImageFilter = NaryMEANImageFilter::New();

    typedef itk::NaryFunctorImageFilter< ImageType,  ImageType,  VARIANCE > NarySTDImageFilter;
    NarySTDImageFilter::Pointer narySTDImageFilter = NarySTDImageFilter::New();
    
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
      origin[0] = 111.5625;
      origin[1] = 111.5625;
      origin[2] = -138.0;
      imagePointer->SetOrigin(origin);

      imageResampleArray[j]->SetInput( imagePointer );
      imageResampleArray[j]->SetSize(    imagePointer->GetLargestPossibleRegion().GetSize() );
      imageResampleArray[j]->SetOutputOrigin(  imagePointer->GetOrigin() );
      imageResampleArray[j]->SetOutputSpacing( imagePointer->GetSpacing() );
      imageResampleArray[j]->SetOutputDirection( imagePointer->GetDirection());
      imageResampleArray[j]->SetDefaultPixelValue( 0 );
      
      naryMeanImageFilter->SetInput(j,imageResampleArray[j]->GetOutput());
      narySTDImageFilter->SetInput(j,imageResampleArray[j]->GetOutput());
      
    }

    // Set the file name
    string meanFname = outputFolder;
    string stdFName = outputFolder;

    if(i==0)
    {
      meanFname  += "Labels/Affine/MeanSlice.jpg";
      stdFName   += "Labels/Affine/STDSlice.jpg";
    }
    else
    {
      ostringstream bsplineFolderName;
      bsplineFolderName << "Labels/Bspline_Grid_" << (int) bsplineInitialGridSize * pow(2.0,i-1) << "/";
      meanFname  += bsplineFolderName.str() + "MeanSlice.jpg";
      stdFName   += bsplineFolderName.str() + "STDSlice.jpg";

    }

    //naryMeanImageFilter->Update();
    //narySTDImageFilter->Update();
        
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
    start[0] = 106;
    size[0] = 0;

        
    ImageType::RegionType extractRegion;
    extractRegion.SetSize(  size  );
    extractRegion.SetIndex( start );
    sliceExtractFilter->SetExtractionRegion( extractRegion );
      
    sliceExtractFilter->SetInput( naryMeanImageFilter->GetOutput() );
    sliceWriter->SetInput( sliceExtractFilter->GetOutput() );

    // write mean image
    sliceWriter->SetFileName( meanFname.c_str() );
    sliceWriter->Update();     

    // write std image
    sliceExtractFilter->SetInput( narySTDImageFilter->GetOutput() );
    sliceWriter->SetInput( sliceExtractFilter->GetOutput() );

    sliceWriter->SetFileName( stdFName.c_str() );
    sliceWriter->Update();

    
  } // End of transform levels
  output.close();

  return EXIT_SUCCESS;
}

