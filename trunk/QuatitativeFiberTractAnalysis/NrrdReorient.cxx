/*=========================================================================

  Program:   Local Project
  Module:    $RCSfile: NrrdReorient.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/15 16:52:01 $
  Version:   $Revision: 1.7 $

  Copyright (c) General Electric Global Research. All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <list>
#include <map>
#include <fstream>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkNrrdImageIO.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkOrientImageFilter.h"
#include "itkSpatialOrientation.h"
#include "itkExtractImageFilter.h"
#include "itkDICOMImageIO2Factory.h"
#include "itkDICOMImageIO2.h"
#include "itkImageSeriesReader.h"
#include "itkDICOMSeriesFileNames.h"
#include "itkImportImageFilter.h"
#include "itkCastImageFilter.h"

#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_math.h"


typedef itk::NrrdImageIO ImageIOType;
typedef itk::Image<unsigned short, 4> ImageType;
typedef itk::Image<unsigned short, 3> OutputImageType;

typedef itk::Image<float, 4> FloatImageType4D;

typedef vnl_vector_fixed<double, 3> vnlVectorType;

itk::SpatialOrientation::ValidCoordinateOrientationFlags
ConvertString2Code(std::string orientString)
{
  // Map between axis string labels and SpatialOrientation
  if (orientString.find("RIP") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP;
  else if (orientString.find("LIP") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIP;
  else if (orientString.find("RSP") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSP;
  else if (orientString.find("LSP") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSP;
  else if (orientString.find("RIA") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIA;
  else if (orientString.find("LIA") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LIA;
  else if (orientString.find("RSA") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSA;
  else if (orientString.find("LSA") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LSA;
  else if (orientString.find("IRP") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRP;
  else if (orientString.find("ILP") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILP;
  else if (orientString.find("SRP") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRP;
  else if (orientString.find("SLP") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLP;
  else if (orientString.find("IRA") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IRA;
  else if (orientString.find("ILA") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ILA;
  else if (orientString.find("SRA") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SRA;
  else if (orientString.find("SLA") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SLA;
  else if (orientString.find("RPI") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI;
  else if (orientString.find("LPI") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI;
  else if (orientString.find("RAI") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI;
  else if (orientString.find("LAI") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAI;
  else if (orientString.find("RPS") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPS;
  else if (orientString.find("LPS") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPS;
  else if (orientString.find("RAS") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS;
  else if (orientString.find("LAS") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LAS;
  else if (orientString.find("PRI") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRI;
  else if (orientString.find("PLI") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLI;
  else if (orientString.find("ARI") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARI;
  else if (orientString.find("ALI") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALI;
  else if (orientString.find("PRS") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PRS;
  else if (orientString.find("PLS") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PLS;
  else if (orientString.find("ARS") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ARS;
  else if (orientString.find("ALS") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ALS;
  else if (orientString.find("IPR") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPR;
  else if (orientString.find("SPR") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPR;
  else if (orientString.find("IAR") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAR;
  else if (orientString.find("SAR") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAR;
  else if (orientString.find("IPL") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IPL;
  else if (orientString.find("SPL") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SPL;
  else if (orientString.find("IAL") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_IAL;
  else if (orientString.find("SAL") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_SAL;
  else if (orientString.find("PIR") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIR;
  else if (orientString.find("PSR") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSR;
  else if (orientString.find("AIR") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIR;
  else if (orientString.find("ASR") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASR;
  else if (orientString.find("PIL") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIL;
  else if (orientString.find("PSL") != std::string::npos) 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_PSL;
  else if (orientString.find("AIL") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_AIL;
  else if (orientString.find("ASL") != std::string::npos)
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_ASL;
  else 
    return itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS;
}


void DetermineDirectionCosine(std::vector<std::string> info, OutputImageType::DirectionType & inputDirection)
{

  vnlVectorType R(1,0,0);
  vnlVectorType L(-1,0,0);
  vnlVectorType A(0,1,0);
  vnlVectorType P(0,-1,0);
  vnlVectorType S(0,0,1);
  vnlVectorType I(0,0,-1);

  std::vector <vnlVectorType> d(3);
  bool spaceFlag = false;
  bool directionFlag = false;

  std::string buf;
  std::string aLine;
  unsigned int pos_direction = std::string::npos;
  unsigned int pos_space = std::string::npos;

  for (int k = 0; k < info.size(); k++)
    {
    if (spaceFlag && directionFlag)
      {
      break;
      }

    aLine = info[k];

    if (pos_direction == std::string::npos)
      {
      pos_direction = aLine.find("space directions:");
      }
    if (pos_space == std::string::npos)
      {
      pos_space = aLine.find("space:");
      }

    if ( (pos_direction == std::string::npos || directionFlag ) && 
         (pos_space == std::string::npos || spaceFlag) )
      {
      continue;
      }

    // the following code inside the if statement determines the direction cosine 
    // in the space, which is specified by "space" value in the header. At this moment
    // we may or may not know it.

    if (pos_direction != std::string::npos && !directionFlag)
      {
      directionFlag = true;
      pos_direction = aLine.find_first_of('(');
      buf = aLine.substr(pos_direction, buf.npos);
      double x1, x2, x3, y1, y2, y3, z1, z2, z3;

      std::sscanf(buf.c_str(), "(%lf,%lf,%lf) (%lf,%lf,%lf) (%lf,%lf,%lf)", 
        &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3);

      // for sanity, normalize the vectors. The direction cosines used by ITK are 
      // the column of the matrix direction.

      double R = x1*x1+y1*y1+z1*z1; 
      R = sqrt(R);
      x1 /= R; y1 /= R; z1 /= R;
      R = x2*x2+y2*y2+z2*z2; 
      R = sqrt(R);
      x2 /= R; y2 /= R; z2 /= R;
      R = x3*x3+y3*y3+z3*z3; 
      R = sqrt(R);
      x3 /= R; y3 /= R; z3 /= R;
 
      // I am still not sure whether ITK uses the columns as the direction cosines. 
      // From the following code from itkOrientImageFilter.txx line 459--477,
      // it seems that the columns are used from different directions.
      
      //if (m_UseImageDirection)
      //{
      //// Compute the GivenOrientation from the image's direction cosines
      //std::string orientation =
      //  this->GetMajorAxisFromPatientRelativeDirectionCosine(
      //    inputPtr->GetDirection()[0][0],
      //    inputPtr->GetDirection()[1][0],
      //    inputPtr->GetDirection()[2][0])
      //  + this->GetMajorAxisFromPatientRelativeDirectionCosine(
      //    inputPtr->GetDirection()[0][1],
      //    inputPtr->GetDirection()[1][1],
      //    inputPtr->GetDirection()[2][1])
      //  + this->GetMajorAxisFromPatientRelativeDirectionCosine(
      //    inputPtr->GetDirection()[0][2],
      //    inputPtr->GetDirection()[1][2],
      //    inputPtr->GetDirection()[2][2]);

      //this->SetGivenCoordinateOrientation (m_StringToCode[orientation]);
      //}

      //inputDirection[0][0] = x1; inputDirection[0][1] = x2; inputDirection[0][2] = x3; 
      //inputDirection[1][0] = y1; inputDirection[1][1] = y2; inputDirection[1][2] = y3; 
      //inputDirection[2][0] = z1; inputDirection[2][1] = z2; inputDirection[2][2] = z3; 

      inputDirection[0][0] = x1; inputDirection[0][1] = y1; inputDirection[0][2] = z1; 
      inputDirection[1][0] = x2; inputDirection[1][1] = y2; inputDirection[1][2] = z2; 
      inputDirection[2][0] = x3; inputDirection[2][1] = y3; inputDirection[2][2] = z3; 

      }
       
    // the following code determine what is the space that the volume lives in,
    // with this piece of information, we can have the correct direction cosines
    // represented in RAS space (+x -- right; +y -- anterior; +z -- superior)
    // I will stick to this space forever.

    if (pos_space != std::string::npos && !spaceFlag)
      {
      spaceFlag = true;

      pos_space = aLine.find_first_of(':');
      buf = aLine.substr(pos_space+1, aLine.npos) + "-";
      for (int k = 0; k < 3; k++)
        {
        unsigned int p = buf.find_first_of("-");
        std::string spaceName = buf.substr(0, p);
        buf = buf.substr(p+1, buf.npos);
        //std::cout << spaceName << "--" << buf << std::endl;
        if (spaceName.find("right") != std::string.npos)
          {
          d[k] = R;
          }
        else if (spaceName.find("left") != std::string.npos)
          {
          d[k] = L;
          }
        else if (spaceName.find("anterior") != std::string.npos)
          {
          d[k] = A;
          }
        else if (spaceName.find("posterior") != std::string.npos)
          {
          d[k] = P;
          }
        else if (spaceName.find("superior") != std::string.npos)
          {
          d[k] = S;
          }
        else if (spaceName.find("inferior") != std::string.npos)
          {
          d[k] = I;
          }
        else {
          std::cout << "Invalid space name. Valid space names are: right, left, anterior, posterior, superior, and inferior.\n";
          exit (-1);
          }
        std::cout << d[k] << std::endl;
        }
      }
    }
  
  // convert the direction cosines in current space (nrrd space) into directions cosines in RAS space
  // the following assumes that the columns of the matrix inputDirecton are the direction cosines.
  for (int k = 0; k < 3; k++)
    {
    vnlVectorType directionInRAS (0,0,0);
    for (int m = 0; m < 3; m ++)
      {
      directionInRAS += inputDirection[m][k] *d[m];
      }
    for (int m = 0; m < 3; m++)
      {
      inputDirection[m][k] = directionInRAS[m];
      }
    }

  std::cout << inputDirection << std::endl;
  return;
}

int main(int ac, char* av[])
{

  if(ac < 4)
  {
    std::cerr << "Usage: " << av[0] << " NrrdFileName (.nhdr)" << " OutFileNameBase DesiredOrientation (e.g. RSA) [sigma]\n";
    return EXIT_FAILURE;
  }


  itk::ImageFileReader<ImageType>::Pointer reader 
    = itk::ImageFileReader<ImageType>::New();

  itk::ImageFileWriter<OutputImageType>::Pointer writer 
    = itk::ImageFileWriter<OutputImageType>::New();

  ImageIOType::Pointer nrrdio = ImageIOType::New();
  reader->SetFileName(av[1]);
  reader->SetImageIO(nrrdio);

  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  /* read in the directional cosine and the slice thickness information 
   * from the nrrd header file.
   */
  std::ifstream nheader;
  nheader.open(av[1], std::ios::in);

  std::vector <std::string> hdrInfo;

  OutputImageType::DirectionType inputDirection;
  inputDirection.SetIdentity();

  while (!nheader.eof())
    {
    char buf_str[256];
    nheader.getline(buf_str, 256);
    std::string buf(buf_str);
    hdrInfo.push_back(buf);
    }
  DetermineDirectionCosine(hdrInfo, inputDirection);
  //std::cout << inputDirection << std::endl;

  ImageType::Pointer img = reader->GetOutput();
  //ImageType::Pointer oNrrdImg = ImageType::New();
  //oNrrdImg->CopyInformation(img);

  double sigma = 0.0;
  if (ac >= 5)
    {
    sigma = atof(av[4]);
    }

  if ( sigma > 0.0 )
    {
    std::cout << sigma << std::endl;
    typedef itk::CastImageFilter<ImageType, FloatImageType4D> UpCastType;
    typedef itk::CastImageFilter<FloatImageType4D, ImageType> DownCastType;
    
    UpCastType::Pointer upCaster = UpCastType::New();
    DownCastType::Pointer downCaster = DownCastType::New();

    upCaster -> SetInput (img);

    typedef itk::RecursiveGaussianImageFilter<FloatImageType4D,FloatImageType4D> SmoothImageType;
    SmoothImageType::Pointer smoothX = SmoothImageType::New();
    SmoothImageType::Pointer smoothY = SmoothImageType::New();
    SmoothImageType::Pointer smoothZ = SmoothImageType::New();

    smoothX->SetInput(upCaster->GetOutput());

    smoothY->SetInput(smoothX->GetOutput());
    smoothZ->SetInput(smoothY->GetOutput());

    smoothX->SetNormalizeAcrossScale( false );
    smoothY->SetNormalizeAcrossScale( false );
    smoothZ->SetNormalizeAcrossScale( false );

    smoothX->SetDirection( 0 );
    smoothY->SetDirection( 1 );
    smoothZ->SetDirection( 2 );

    smoothX -> SetSigma ( sigma );
    smoothY -> SetSigma ( sigma );
    smoothZ -> SetSigma ( sigma );

    downCaster->SetInput( smoothZ -> GetOutput() );
    downCaster->Update();

    img = downCaster->GetOutput();
    }

  itk::ImageFileWriter<ImageType>::Pointer nrrdWriter
      = itk::ImageFileWriter<ImageType>::New();

  const char* OutputFilenameBase = av[2];
  char oFilename[128];

  typedef itk::ExtractImageFilter< ImageType, OutputImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();

  ImageType::RegionType inputRegion =
           reader->GetOutput()->GetLargestPossibleRegion();

  ImageType::SizeType size = inputRegion.GetSize();
  int nAngle = size[3];
  size[3] = 0;

  int nVoxel = 1;
  for (int k = 0; k < 3; k++)
    {
    nVoxel *= size[k];
    }

  ImageType::IndexType start = inputRegion.GetIndex();
  ImageType::RegionType desiredRegion;

  ImageType::PixelType* ptrToImage;
  ptrToImage = (ImageType::PixelType*) malloc(nVoxel * size[3] * sizeof(ImageType::PixelType));

  ImageType::Pointer oNrrdImage = ImageType::New();
  oNrrdImage->CopyInformation(img);
  oNrrdImage->SetRequestedRegion(oNrrdImage->GetLargestPossibleRegion());
  oNrrdImage->SetBufferedRegion(oNrrdImage->GetLargestPossibleRegion());

  oNrrdImage->Allocate();

  itk::ImageRegionIteratorWithIndex<ImageType> itNrrd (oNrrdImage, oNrrdImage->GetLargestPossibleRegion());
  itNrrd.GoToBegin();

  filter->SetInput( img );

  //for (int sliceNumber = 0; sliceNumber < 1; sliceNumber ++)
  for (int sliceNumber = 0; sliceNumber < nAngle; sliceNumber ++)  
  {
    start[3] = sliceNumber;

    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );

    filter->SetExtractionRegion( desiredRegion );
    filter->Update();

    OutputImageType::Pointer oImg;
    oImg = filter -> GetOutput();
    oImg -> SetDirection(inputDirection); // this is the correct direction of the volume
                                          // according to the nrrd header

    typedef itk::OrientImageFilter<OutputImageType, OutputImageType> ReorienterType;
    ReorienterType::Pointer orienter = ReorienterType::New();

    orienter->SetInput(oImg);
    orienter->SetDesiredCoordinateOrientation(ConvertString2Code(av[3]));   
    orienter->SetUseImageDirection ( true );
    orienter->Update();

    sprintf(oFilename, "%s_%d.img",OutputFilenameBase, sliceNumber);
    writer->SetFileName(oFilename);

    writer->SetInput( orienter->GetOutput() );

    try 
    { 
      writer->Update(); 
    } 
    catch( itk::ExceptionObject & err ) 
    { 
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      return EXIT_FAILURE;
    } 

    oImg = orienter->GetOutput();
    itk::ImageRegionConstIteratorWithIndex<OutputImageType> itRASImage (oImg, oImg->GetLargestPossibleRegion());

    for (itRASImage.GoToBegin(); !itRASImage.IsAtEnd(); ++itRASImage, ++itNrrd)
      {
      itNrrd.Set(itRASImage.Get());
      }
  }
 
  //itk::ImageFileWriter<ImageType>::Pointer nrrdWriter
  //    = itk::ImageFileWriter<ImageType>::New();


  sprintf(oFilename, "%s_nrrd.nhdr",OutputFilenameBase);
  nrrdWriter->SetFileName(oFilename);
  nrrdWriter->SetInput(oNrrdImage);


  try 
    { 
      nrrdWriter->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      return EXIT_FAILURE;
    } 

  return EXIT_SUCCESS;
}
