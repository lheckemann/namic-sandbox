#include "SFLSMultiRobustStatSeg3DTestCLP.h"

#include <iostream>


// douher
#include "SFLSMultiRobustStatSegmentor3D.h"


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


int main(int argc, char** argv)
{
  PARSE_ARGS; 

  typedef int PixelType;
  typedef SFLSMultiRobustStatSegmentor3D_c< PixelType > SFLSMultiRobustStatSegmentor3D_t;

  // read input image
  //typedef SFLSMultiRobustStatSegmentor3D_t::TDoubleImage Image_t;
  typedef itk::Image<PixelType, 3> Image_t;

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
  typedef SFLSMultiRobustStatSegmentor3D_t::labelMap_t labelMap_t;

  typedef itk::ImageFileReader< labelMap_t > labelMapReader_t;
  labelMapReader_t::Pointer readerLabel = labelMapReader_t::New();
  readerLabel->SetFileName(labelImageFileName.c_str());
  labelMap_t::Pointer labelImg;
    
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
  SFLSMultiRobustStatSegmentor3D_t seg;
  seg.setImage(img);

  seg.setLabelMap(labelImg);

  seg.setNumIter(numOfIteration);

  seg.setIntensityHomogeneity(intensityHomogeneity);


  curvatureWeight /= 1.5;
  seg.setCurvatureWeight(curvatureWeight);

  seg.doSegmenation();



  typedef itk::ImageFileWriter< labelMap_t > WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( segmentedImageFileName.c_str() );
  writer->SetInput(seg.getFinalLabelMap());

  try
    {
      writer->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      raise(SIGABRT);   
    }


  
  return 0;
}

