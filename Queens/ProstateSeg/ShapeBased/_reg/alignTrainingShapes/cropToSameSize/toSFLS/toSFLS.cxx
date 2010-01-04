// itk
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


// douher
#include "SFLSChanVeseSegmentor3D.h"


int main(int argc, char** argv)
{
  if (argc != 3)
    {
      std::cerr<<"InputBinName OutputSFLSName\n";
      exit(0);
    }

  const char* binName = argv[1];
  const char* SFLSName = argv[2];

  typedef itk::Image<unsigned char, 3> ucharImage_t;
  ucharImage_t::Pointer inputBin;



  typedef itk::ImageFileReader< ucharImage_t > imageReader_t;
  imageReader_t::Pointer reader = imageReader_t::New();
  reader->SetFileName(binName);

  try
    {
      reader->Update();
      inputBin = reader->GetOutput();
    }
  catch ( itk::ExceptionObject &err)
    {
      std::cerr<< "ExceptionObject caught !" << std::endl; 
      std::cerr<< err << std::endl; 
      raise(SIGABRT);
    }




  CSFLSChanVeseSegmentor3D< unsigned char > cv;
  cv.setImage(inputBin);
  cv.setMask(inputBin);
  cv.initializeSFLSFromMask();


  typedef itk::Image<float, 3> floatImage_t;


  typedef itk::ImageFileWriter< floatImage_t > writer_t;
  writer_t::Pointer outputWriter = writer_t::New();
  outputWriter->SetFileName(SFLSName);
  outputWriter->SetInput(cv.mp_phi);
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
