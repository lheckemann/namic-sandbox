#include "reg_3d_similarity_mse.hpp"
#include "thld3.hpp"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
      std::cerr << "Missing Parameters " << std::endl;
      std::cerr << "Usage: " << argv[0];
      std::cerr << " fixedImageFile  movingImageFile ";
      std::cerr << " outputImagefile [fillInValue = 0]" << std::endl;

      return EXIT_FAILURE;
    }

  typedef float pixel_t;
  typedef itk::Image<pixel_t, 3> image_t;


  typedef itk::ImageFileReader< image_t  > image_reader_t;

  image_reader_t::Pointer  fixedImageReader  = image_reader_t::New();
  image_reader_t::Pointer movingImageReader = image_reader_t::New();

  fixedImageReader->SetFileName(  argv[1] );
  movingImageReader->SetFileName( argv[2] );

  fixedImageReader->Update();
  image_t::Pointer fixImg = fixedImageReader->GetOutput();


  movingImageReader->Update();
  image_t::Pointer movingImg = movingImageReader->GetOutput();

  pixel_t fillInValue = static_cast<pixel_t>(0.0);
  if (argc >= 5)
    {
      fillInValue = static_cast<pixel_t>(atof(argv[4]));
    }


  image_t::Pointer registeredMovingImg = newProstate::reg_3d_similarity_mse<image_t, image_t, image_t>(fixImg, movingImg, fillInValue);


  typedef itk::ImageFileWriter< image_t >  WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( registeredMovingImg );
  writer->UseCompressionOn();
  writer->Update();

  return 0;
}
