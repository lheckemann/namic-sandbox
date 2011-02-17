#ifndef utilities_hxx_
#define utilities_hxx_

// itk
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include <csignal>

//namespace LAWallSegmenter
  /**
   * readImage
   */
  template< typename itkImage_t >
  typename itkImage_t::Pointer readImage(const char *fileName)
  {
    typedef itk::ImageFileReader< itkImage_t > ImageReaderType;
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(fileName);

    typename itkImage_t::Pointer image;
    
    try
      {
        reader->Update();
        image = reader->GetOutput();
      }
    catch ( itk::ExceptionObject &err)
      {
        std::cerr<< "ExceptionObject caught !" << std::endl; 
        std::cerr<< err << std::endl; 
        raise(SIGABRT);
      }

    return image;
  }


  /**
   * writeImage
   */
  template< typename itkImage_t > void writeImage(typename itkImage_t::Pointer img, const char *fileName)
  {
    typedef itk::ImageFileWriter< itkImage_t > WriterType;

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( fileName );
    writer->SetInput(img);
    writer->UseCompressionOn();

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
  }





#endif
