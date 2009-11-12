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

template< typename TPixel >
itk::Image< unsigned char, 3 >::Pointer 
getInitMask(typename itk::Image< TPixel, 3 >::Pointer img, long cx, long cy, long cz );


template< typename TPixel >
itk::Image< unsigned char, 3 >::Pointer 
getFinalMask(typename itk::Image< TPixel, 3 >::Pointer img, TPixel thod = 0);


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
  mask->CopyInformation(img);
  mask->Allocate();
  mask->FillBuffer(0);



  //debug//
  std::ofstream f("/tmp/outputMen.txt");
  f<<mask->GetLargestPossibleRegion().GetIndex()<<std::endl;
  f<<"--------------------\n";
  //DEBUG//

  f<< "seed.size =  " << seed.size() << std::endl;
  f<<"--------------------\n";

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

//           //mask = getInitMask< PixelType >(img, index[0], index[1], index[2]);
          f << "LPS: " << lpsPoint << std::endl;
          f << "IJK: " << index << std::endl;
        }
    }
  else
    {
      std::cerr << "No seeds specified." << std::endl;
      return -1;
    }

  f.close();


  typedef itk::ImageFileWriter< MaskImageType > WriterType;

  /* set the mask image according to the seeds
     --------------------------------------------------------------------------------*/
  //debug//
  WriterType::Pointer outputWriter1 = WriterType::New();
  outputWriter1->SetFileName("/tmp/initMask.nrrd");
  outputWriter1->SetInput(mask);
  outputWriter1->Update();
  //DEBUG//


  MaskImageType::Pointer finalMask = mask;
  //  MaskImageType::Pointer finalMask = NULL;

//   if (!globalSwitch)
//     {
//       // perform global statistics based segmentation
//       CSFLSChanVeseSegmentor3D< PixelType > cv;
//       cv.setImage(img);
//       cv.setMask(mask);

//       cv.setNumIter(numOfIteration);

//       cv.setCurvatureWeight(curvatureWeight);

//       cv.doChanVeseSegmenation();

//       finalMask = getFinalMask<double>(cv.mp_phi);
//     }
//   else
//     {
//       // perform local statistics based segmentation
//       CSFLSLocalChanVeseSegmentor3D< PixelType > cv;
//       cv.setImage(img);
//       cv.setMask(mask);

//       cv.setNumIter(numOfIteration);

//       cv.setCurvatureWeight(curvatureWeight);
//       cv.setNBHDSize(30, 30, 5);


//       // do segmentation
//       cv.doLocalChanVeseSegmenation();

//       finalMask = getFinalMask<double>(cv.mp_phi);
//     }
  
//   finalMask->SetSpacing(img->GetSpacing());
//   finalMask->SetOrigin(img->GetOrigin());

//   finalMask->CopyInformation(img);
// //  imageITK->SetOrientation(inputReader>GetOrientation());


  typedef itk::ImageFileWriter< MaskImageType > WriterType;
  WriterType::Pointer outputWriter = WriterType::New();
  outputWriter->SetFileName(segmentedImageFileName.c_str());
  outputWriter->SetInput(finalMask);
  outputWriter->Update();
  
  
  return EXIT_SUCCESS;
}


template< typename TPixel >
itk::Image< unsigned char, 3 >::Pointer 
getInitMask(typename itk::Image< TPixel, 3 >::Pointer img, long cx, long cy, long cz )
{
  typedef itk::Image< unsigned char, 3 > MaskType;

  MaskType::SizeType size = img->GetLargestPossibleRegion().GetSize();

  long nx = size[0];
  long ny = size[1];
  long nz = size[2];
  
  
  if (cx < 0 || cx > nx-1 || cy < 0 || cy > ny-1 || cz < 0 || cz > nz-1 )
    {
      std::cerr<<"(cx, cy, cz) not in the img domain. abort.\n";
      raise(SIGABRT);
    }


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
  

  for (long ix = cx-5; ix < cx+6; ++ix)
    {
      for (long iy = cy-5; iy < cy+6; ++iy)
        {
          for (long iz = cz-5; iz < cz+6; ++iz)
            {
              itk::Image< unsigned char, 3 >::IndexType idx = {{ix, iy, iz}};
              mask->SetPixel(idx, 1);
            }
        }
    }

  return mask;  
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
