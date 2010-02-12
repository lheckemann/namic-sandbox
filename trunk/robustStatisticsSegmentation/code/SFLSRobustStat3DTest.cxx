#include "SFLSRobustStat3DTestCLP.h"

#include <iostream>


//#include "SFLSRobustStatSegmentor3D.h"
#include "SFLSRobustStatSegmentor3DLabelMap.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


// // for reading seeds
// #include "aux/txtIO.h"


// template< typename TImage >
// void 
// seedsPreprocess(typename TImage::Pointer img,                           \
//                 const std::vector<std::vector<float> >& seedListLPS,    \
//                 std::vector<std::vector<long> >& seedListIJK);

template< typename TPixel >
itk::Image< short, 3 >::Pointer 
getFinalMask(typename itk::Image< TPixel, 3 >::Pointer img, unsigned char l, TPixel thod = 0);


int main(int argc, char** argv)
{
  PARSE_ARGS; 


  typedef short PixelType;
  typedef CSFLSRobustStatSegmentor3DLabelMap< PixelType > SFLSRobustStatSegmentor3DLabelMap_c;

  // read input image
  typedef SFLSRobustStatSegmentor3DLabelMap_c::TImage Image_t;

  typedef itk::ImageFileReader< Image_t > ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(originalImageFileName.c_str());
  Image_t::Pointer img;
    
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



  // read input label image
  typedef SFLSRobustStatSegmentor3DLabelMap_c::TLabelImage LabelImage_t;

  typedef itk::ImageFileReader< LabelImage_t > LabelImageReader_t;
  LabelImageReader_t::Pointer readerLabel = LabelImageReader_t::New();
  readerLabel->SetFileName(labelImageFileName.c_str());
  LabelImage_t::Pointer labelImg;
    
  try
    {
      readerLabel->Update();
      labelImg = readerLabel->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }


  // do seg
  SFLSRobustStatSegmentor3DLabelMap_c seg;
  seg.setImage(img);

  seg.setNumIter(10000); // a large enough number, s.t. will not be stoped by this creteria.
  seg.setMaxVolume(expectedVolume);
  seg.setInputLabelImage(labelImg);

  //seg.setNumIter(numOfIteration);
  seg.setMaxRunningTime(maxRunningTime);


  seg.setIntensityHomogeneity(intensityHomogeneity);
  seg.setCurvatureWeight(curvatureWeight/1.5);

  seg.doSegmenation();



//   typedef int PixelType;
//   typedef itk::Image< PixelType, 3 > ImageType;

//   typedef itk::ImageFileReader< ImageType > ImageReaderType;
//   ImageReaderType::Pointer reader = ImageReaderType::New();
//   reader->SetFileName(originalImageFileName.c_str());

//   ImageType::Pointer img;
    
//   try
//     {
//       reader->Update();
//       img = reader->GetOutput();
//     }
//   catch ( itk::ExceptionObject &err)
//     {
//       std::cerr<< "ExceptionObject caught !" << std::endl; 
//       std::cerr<< err << std::endl; 
//       raise(SIGABRT);
//     }




// //   std::vector<std::vector<long> > seedListIJK;
// //   seedsPreprocess<ImageType>(img, seed, seedListIJK);
  
//   CSFLSRobustStatSegmentor3D< PixelType > seg;
//   seg.setImage(img);

//   seg.setSeeds(seedListIJK);
//   seg.setMaxRunningTime(maxRunningTime);
//   seg.setMaxVolume(expectedVolume);

//   seg.setIntensityHomogeneity(intensityHomogeneity);

//   //seg.setNumIter(numOfIteration);

//   seg.setCurvatureWeight(curvatureWeight/2.0); // in the interface, it's 0~1 scale, internally, it's 0~0.5

//   seg.doSegmenation();


  typedef itk::Image< short, 3 > MaskImageType;

  MaskImageType::Pointer finalMask = getFinalMask<float>(seg.mp_phi, labelValue, 2.0);
  finalMask->CopyInformation(img);

  typedef itk::ImageFileWriter< MaskImageType > WriterType;
  WriterType::Pointer outputWriter = WriterType::New();
  outputWriter->SetFileName(segmentedImageFileName.c_str());
  outputWriter->SetInput(finalMask);
  outputWriter->Update();
  
  try
    {
      outputWriter->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      raise(SIGABRT);   
    }

  
  return EXIT_SUCCESS;
}

// template< typename TImage >
// void 
// seedsPreprocess(typename TImage::Pointer img,                           \
//                 const std::vector<std::vector<float> >& seedListLPS,    \
//                 std::vector<std::vector<long> >& seedListIJK)
// {
//   long n = seedListLPS.size();
//   if (n <= 0)
//     {
//       std::cerr << "Error: No seeds specified." << std::endl;
//       raise(SIGABRT);
//     }
  

//   seedListIJK.resize(n);


//   typename TImage::PointType lpsPoint;
//   typename TImage::IndexType index;
//   for (long i = 0; i < n; ++i)
//     {
//       // seeds come in ras, convert to lps
//       lpsPoint[0] = -seedListLPS[i][0];  
//       lpsPoint[1] = -seedListLPS[i][1];
//       lpsPoint[2] = seedListLPS[i][2];

//       img->TransformPhysicalPointToIndex(lpsPoint, index);
          
//       std::vector<long> s(3);
//       s[0] = index[0];
//       s[1] = index[1];
//       s[2] = index[2];

//       seedListIJK[i] = s;

//       //           f << "LPS: " << lpsPoint << std::endl;
//       //           f << "IJK: " << index << std::endl;
//     }

//   return;
// }


template< typename TPixel >
itk::Image< short, 3 >::Pointer 
getFinalMask(typename itk::Image< TPixel, 3 >::Pointer img, unsigned char l, TPixel thod)
{
  typedef itk::Image< short, 3 > MaskType;

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
              
              mask->SetPixel(idx, v<=thod?l:0);
            }
        }
    }



  return mask;
}
