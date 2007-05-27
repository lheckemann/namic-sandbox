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

#include "itkResampleImageFilter.h"

#include "itkAffineTransform.h"
#include "UserBSplineDeformableTransform.h"
#include "itkTransformFactory.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include "itkImageRegionIterator.h"

#include "itkNaryFunctorImageFilter.h"
#include <string>
#include <sstream>
#include <fstream>

//Define the global types for image type
#define PixelType unsigned char
#define InternalPixelType float
#define Dimension 3

#include "itkTransformFileReader.h"
    
using namespace std;
int getCommandLine(       int argc, char *initFname, vector<string>& fileNames, string& inputFolder, string& outputFolder,
                          int& bsplineInitialGridSize,  int& numberOfBsplineLevel,
                          string& useBspline, string& useBsplineHigh,
                          string& labelFileFolder, vector<string>& labelFileNames )
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
    for(int i=0; i<pixelStack.size(); i++)
    {
      if( pixelStack[i] != m_Number)
      {
         return 0;
      }
    }
    return 1;
  }

  PixelType m_Number;
};

class OR
{
public:
  OR() {m_Number=0;};
  ~OR() {};
  bool operator!=( const OR & ) const
  {
    return false;
  }
  bool operator==( const OR & other ) const
  {
    return !(*this != other);
  }
  inline PixelType operator()( const std::vector< PixelType > pixelStack)
  {
    for(int i=0; i<pixelStack.size(); i++)
    {
      if( pixelStack[i] == m_Number)
      {
         return 1;
      }
    }
    return 0;
  }

  PixelType m_Number;
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
      

    //Get the command line arguments
  for(int i=1; i<argc; i++)
  {
    if ( getCommandLine(
                  argc, argv[i], fileNames, inputFolder, outputFolder,
                  bsplineInitialGridSize,  numberOfBsplineLevel,
                  useBspline, useBsplineHigh,
                  labelFileFolder, labelFileNames )
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
  cout << "Reading Images" << endl;
  std::vector< ReaderType::Pointer > labelReaderArray(N);
  for(int i=0; i<N; i++)
  {
    labelReaderArray[i] = ReaderType::New();
    string fname = labelFileFolder + labelFileNames[i];
    labelReaderArray[i]->SetFileName(fname.c_str());
    labelReaderArray[i]->Update();
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
        transformFileNames[i][j] = outputFolder + "Affine/TransformFiles/" + fileNames[i];
        transformFileNames[i][j].replace(transformFileNames[i][j].size()-3, 3, "txt" );
      }
      transformNames[i] = "Affine";
    }
    else // generate bspline names
    {
      ostringstream bsplineFolderName;
      bsplineFolderName << "Bspline_Grid_" << (int) pow((double)bsplineInitialGridSize,i);
      transformNames[i] = bsplineFolderName.str();
      for( int j=0; j<N; j++)
      {
        transformFileNames[i][j] = outputFolder + bsplineFolderName.str() + "/TransformFiles/" + fileNames[i];
        transformFileNames[i][j].replace(transformFileNames[i][j].size()-3, 3, "txt" );
      }
    }

  }

  ofstream output( (outputFolder + "DiceMeasures.txt").c_str() );
  output << "\tLabel:25 \tLabel:30 \tLabel:23" << endl;

  // Resample the images
  // Read the input transforms and compute the dice measure
  for(int i=0; i<transformLevels; i++)
  {
    // typedef for transformation types
    typedef itk::Transform< double, Dimension >  TransformType;
    typedef itk::AffineTransform< double, Dimension >  AffineTransformType;
    typedef itk::UserBSplineDeformableTransform< double,
                                             Dimension,
                                             3 >     BSplineTransformType;

    std::vector< TransformType::Pointer >   transformArray(N);

    for(int j=0; j<N; j++)
    {
      // Typedef for reader
      typedef itk::TransformFileReader    TransformFileReader;
      typedef TransformFileReader::TransformListType   TransformListType;
      
      TransformFileReader::Pointer        transformFileReader = TransformFileReader::New();
      transformFileReader->SetFileName(transformFileNames[i][j].c_str());

      if( i == 0)
      {
        // Create the transforms
        transformFileReader->Update();
        TransformListType*   transformList = transformFileReader->GetTransformList();
        
        AffineTransformType::Pointer  affineTransform = AffineTransformType::New();
        affineTransform->SetFixedParameters(transformList->front()->GetFixedParameters());
        affineTransform->SetParameters(transformList->front()->GetParameters());
        transformArray[j] = affineTransform;
      }
      else
      {

        // Get Affine transform
        TransformFileReader::Pointer        affineTransformFileReader = TransformFileReader::New();
        affineTransformFileReader->SetFileName(transformFileNames[0][j].c_str());

        affineTransformFileReader->Update();
        TransformListType*   affineTransformList = affineTransformFileReader->GetTransformList();

        
        AffineTransformType::Pointer  affineTransform = AffineTransformType::New();
        affineTransform->SetFixedParameters(affineTransformList->front()->GetFixedParameters());
        affineTransform->SetParameters(affineTransformList->front()->GetParameters());
        

        // Get BSpline transform
        itk::TransformFactoryBase::Pointer f = itk::TransformFactoryBase::GetFactory();
        BSplineTransformType::Pointer  bsplineTransform = BSplineTransformType::New();
        f->RegisterTransform(bsplineTransform->GetTransformTypeAsString().c_str(),
                             bsplineTransform->GetTransformTypeAsString().c_str(),
                             bsplineTransform->GetTransformTypeAsString().c_str(),
                             1,
                             itk::CreateObjectFunction<BSplineTransformType>::New());
        transformFileReader->Update();
        TransformListType*   transformList = transformFileReader->GetTransformList();
        bsplineTransform->SetFixedParameters(transformList->front()->GetFixedParameters());
        bsplineTransform->SetParameters(transformList->front()->GetParameters());
        bsplineTransform->SetBulkTransform(affineTransform);
        transformArray[j] = bsplineTransform;
      }
    }


    // Resample the labels according to transforms
    typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleFilterType;
    std::vector< ResampleFilterType::Pointer> resampleArray(N);

    typedef itk::NearestNeighborInterpolateImageFunction< ImageType, double >  InterpolatorType;

    for(int j=0; j<N; j++)
    {
      InterpolatorType::Pointer interpolator = InterpolatorType::New();
      resampleArray[j] = ResampleFilterType::New();
      resampleArray[j]->SetInterpolator( interpolator );
      resampleArray[j]->SetTransform( transformArray[j] );
      resampleArray[j]->SetInput( labelReaderArray[j]->GetOutput() );
      resampleArray[j]->SetSize(    labelReaderArray[j]->GetOutput()->GetLargestPossibleRegion().GetSize() );
      resampleArray[j]->SetOutputOrigin(  labelReaderArray[j]->GetOutput()->GetOrigin() );
      resampleArray[j]->SetOutputSpacing( labelReaderArray[j]->GetOutput()->GetSpacing() );
      resampleArray[j]->SetOutputDirection( labelReaderArray[j]->GetOutput()->GetDirection());
      resampleArray[j]->SetDefaultPixelValue( 0 );
    }

    // Compute the dice measures
    typedef itk::NaryFunctorImageFilter< ImageType,  ImageType,  AND > NaryANDImageFilter;
    NaryANDImageFilter::Pointer naryANDImageFilter = NaryANDImageFilter::New();

    typedef itk::NaryFunctorImageFilter< ImageType,  ImageType,  OR > NaryORImageFilter;
    NaryORImageFilter::Pointer naryORImageFilter = NaryORImageFilter::New();

    for(int j=0; j<N; j++)
    {
      naryANDImageFilter->SetInput(j,resampleArray[j]->GetOutput());
      naryORImageFilter->SetInput( j,resampleArray[j]->GetOutput());
    }

    cout << "Computing dice measure " << endl;
    output << transformNames[i] << "\t";
    for(int j=0; j<3 ; j++)
    {
      // Set the labels
      if(j ==0)
      {
        // White Matter
        naryANDImageFilter->GetFunctor().m_Number = 25;
        naryORImageFilter->GetFunctor().m_Number = 25;
        naryANDImageFilter->Modified();
        naryORImageFilter->Modified();
      }
      else if(j==1)
      {
        // White Matter
        naryANDImageFilter->GetFunctor().m_Number = 30;
        naryORImageFilter->GetFunctor().m_Number = 30;
        naryANDImageFilter->Modified();
        naryORImageFilter->Modified();
      }
      else
      {
        // White Matter
        naryANDImageFilter->GetFunctor().m_Number = 23;
        naryORImageFilter->GetFunctor().m_Number = 23;
        naryANDImageFilter->Modified();
        naryORImageFilter->Modified();
      }
      naryANDImageFilter->Update();
      naryORImageFilter->Update();
      
      typedef itk::ImageRegionIterator<ImageType>  IteratorType;
    
      IteratorType andIt(naryANDImageFilter->GetOutput(),naryANDImageFilter->GetOutput()->GetLargestPossibleRegion() );
      IteratorType orIt( naryORImageFilter->GetOutput(),naryORImageFilter->GetOutput()->GetLargestPossibleRegion() );

      double andSum = 0;
      double orSum = 0;
      for ( andIt.GoToBegin(); !andIt.IsAtEnd(); ++andIt)
      {
        andSum += andIt.Get();
        orSum += orIt.Get();
        ++orIt;
      }
      output << andSum / orSum << "\t";
      cout << andSum << " " << orSum << endl;
    }
    output << endl;

  }
  output.close();

  return EXIT_SUCCESS;
}

