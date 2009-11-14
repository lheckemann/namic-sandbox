#include "MeningiomaSegCLP.h"

/*
  Given the center point of the meningioma, the segmentor performs
  global/local statistics based active contour segmentation on the
  meningioma image.
*/

#include <iostream>

#include <fstream>


#include "SFLSLocalChanVeseSegmentor3D.h"
#include "SFLSChanVeseSegmentor3D.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkImageRegionConstIterator.h"


template< typename TPixel >
itk::Image< unsigned char, 3 >::Pointer 
getFinalMask(typename itk::Image< TPixel, 3 >::Pointer img, TPixel thod = 0);


template< typename TPixel >
bool
useLocal(typename itk::Image< TPixel, 3 >::Pointer img);



int main(int argc, char** argv)
{
  PARSE_ARGS; 

//   if (argc != 9)
//     {
//       std::cerr<<"Parameters: inputImage outputImage numIter lambda cx cy cz globalSwitch\n";
//       exit(-1);
//     }

//   const char* inputImage = argv[1];
//   const char* outputImage = argv[2];
//   unsigned long numiter = atol(argv[3]);
//   double lambda = atof(argv[4]);
//   long cx = atol(argv[5]);
//   long cy = atol(argv[6]);
//   long cz = atol(argv[7]);
//   int globalSwitch = atoi(argv[8]);


  /*----------------------------------------------------------------------
    check ranges of input parameters */
  if (0 > curvatureWeight)
    {
      curvatureWeight = 0;
    }

  if (10 < curvatureWeight)
    {
      curvatureWeight = 10;
    }


  if (0 > expectedVolume)
    {
      expectedVolume = 0.1;
    }
  /* check ranges of input parameters 
     ----------------------------------------------------------------------*/


  typedef double PixelType;
  typedef itk::Image< PixelType, 3 > ImageType;
  typedef itk::Image< unsigned char, 3 > MaskImageType;

  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(originalImageFileName.c_str());

  ImageType::Pointer img;
  try
    {
      reader->Update();
      img = reader->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }


  /* --------------------------------------------------------------------------------
     set the mask image according to the seeds  */
  MaskImageType::Pointer mask = MaskImageType::New();
  MaskImageType::RegionType region = img->GetLargestPossibleRegion();
  mask->SetRegions(region);
  mask->Allocate();
  mask->CopyInformation(img);
  //  mask->SetOrientation(img->GetOrientation());
  mask->FillBuffer(0);



  //debug//
  //  std::ofstream f("/tmp/outputMen.txt");
//   f<<mask->GetLargestPossibleRegion().GetIndex()<<std::endl;
//   f<<"--------------------\n";
  //DEBUG//

//   f<< "seed.size =  " << seed.size() << std::endl;
//   f<<"--------------------\n";

  if (seed.size() > 0)
    {
      MaskImageType::PointType lpsPoint;
      MaskImageType::IndexType index;
      for (::size_t i=0; i<seed.size(); ++i)
        {
          // seeds come in ras, convert to lps
          lpsPoint[0] = -seed[i][0];  
          lpsPoint[1] = -seed[i][1];
          lpsPoint[2] = seed[i][2];

          img->TransformPhysicalPointToIndex(lpsPoint, index);

          for (long ix = index[0]-3; ix < index[0]+4; ++ix)
            {
              for (long iy = index[1]-3; iy < index[1]+4; ++iy)
                {
                  for (long iz = index[2]-3; iz < index[2]+4; ++iz)
                    {
                      MaskImageType::IndexType idx = {{ix, iy, iz}};
                      mask->SetPixel(idx, 2);
                    }
                }
            }

//           f << "LPS: " << lpsPoint << std::endl;
//           f << "IJK: " << index << std::endl;
        }
    }
  else
    {
      std::cerr << "No seeds specified." << std::endl;
      return -1;
    }



  typedef itk::ImageFileWriter< MaskImageType > WriterType;

  /* set the mask image according to the seeds
     --------------------------------------------------------------------------------*/
  //debug//

//   f<<"outputing init mask to file..."<<std::flush;

//   WriterType::Pointer outputWriter1 = WriterType::New();
//   outputWriter1->SetFileName("/tmp/initMask.nrrd");
//   outputWriter1->SetInput(mask);
//   outputWriter1->Update();

//   f<<"done\n"<<std::flush;

//   f.close();
  //DEBUG//


  //  MaskImageType::Pointer finalMask = mask;
  MaskImageType::Pointer finalMask = NULL;

  bool globalSwitch = useLocal<PixelType>(img);

  if (!globalSwitch)
    {
      // perform global statistics based segmentation
      CSFLSChanVeseSegmentor3D< PixelType > cv;
      cv.setImage(img);
      cv.setMask(mask);

      long ChanVeseNumOfIteration = 100000;
      cv.setNumIter(ChanVeseNumOfIteration);
      cv.setExpectedVolume(expectedVolume);

      cv.setCurvatureWeight(curvatureWeight/20.0); // in the interface, it's 0~10 scale, internally, it's 0~0.5

      cv.doChanVeseSegmenation();

      finalMask = getFinalMask<double>(cv.mp_phi);
    }
  else
    {
      // perform local statistics based segmentation
      CSFLSLocalChanVeseSegmentor3D< PixelType > cv;
      cv.setImage(img);
      cv.setMask(mask);

      long localChanVeseNumOfIteration = 1000;
      cv.setNumIter(localChanVeseNumOfIteration);
      cv.setExpectedVolume(expectedVolume);

      cv.setCurvatureWeight(curvatureWeight/20.0); // in the interface, it's 0~10 scale, here need to /10
      cv.setNBHDSize(30, 30, 5);


      // do segmentation
      cv.doLocalChanVeseSegmenation();

      finalMask = getFinalMask<double>(cv.mp_phi);
    }
  
//   finalMask->SetSpacing(img->GetSpacing());
//   finalMask->SetOrigin(img->GetOrigin());
  finalMask->CopyInformation(img);
//  imageITK->SetOrientation(inputReader>GetOrientation());


  typedef itk::ImageFileWriter< MaskImageType > WriterType;
  WriterType::Pointer outputWriter = WriterType::New();
  outputWriter->SetFileName(segmentedImageFileName.c_str());
  outputWriter->SetInput(finalMask);
  outputWriter->Update();
  
  
  return EXIT_SUCCESS;
}


template< typename TPixel >
itk::Image< unsigned char, 3 >::Pointer 
getFinalMask(typename itk::Image< TPixel, 3 >::Pointer img, TPixel thod)
{
  typedef itk::Image< unsigned char, 3 > MaskType;

  MaskType::SizeType size = img->GetLargestPossibleRegion().GetSize();

  long nx = size[0];
  long ny = size[1];
  long nz = size[2];


  MaskType::Pointer mask = MaskType::New();
  MaskType::IndexType start = {{0, 0, 0}};

  MaskType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  mask->SetRegions( region );

  mask->SetSpacing(img->GetSpacing());
  mask->SetOrigin(img->GetOrigin());

  mask->Allocate();
  mask->FillBuffer(0);
  

  for (long ix = 0; ix < nx; ++ix)
    {
      for (long iy = 0; iy < ny; ++iy)
        {
          for (long iz = 0; iz < nz; ++iz)
            {
              MaskType::IndexType idx = {{ix, iy, iz}};
              TPixel v = img->GetPixel(idx);
              
              mask->SetPixel(idx, v<=thod?1:0);
            }
        }
    }



  return mask;
}


template< typename TPixel >
bool
useLocal(typename itk::Image< TPixel, 3 >::Pointer img)
{
  typedef itk::Image< TPixel, 3 > TImage;
  typedef itk::ImageRegionConstIterator<TImage> TImageRegionConstIterator;
  TImageRegionConstIterator it(img, img->GetLargestPossibleRegion());

  bool useLocalStat = false;

  double mean = 0;

  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      mean += it.Get();
    }

  typename TImage::SizeType s = img->GetLargestPossibleRegion().GetSize();
  long n = s[0]*s[1]*s[2];

  mean /= n;

  double stddev = 0;
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      TPixel v = it.Get();
      stddev += (v - mean)*(v - mean);
    }
  
  stddev /= n;
  stddev = sqrt(stddev);

  if (stddev > 10000)
    {
      useLocalStat = true;
    }

  //debug//
  std::ofstream f("/tmp/l.txt");
  f<<stddev<<std::endl<<useLocalStat<<std::endl;
  f.close();
  //DEBUG//


  return useLocalStat;
}
