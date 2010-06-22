#ifndef cArrayOp_txx_
#define cArrayOp_txx_

#include "cArrayOp.h"

#include <iostream>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <cassert>
#include <csignal>


namespace douher
{
  /* --------------------------------------------------
     readImage2
  */
  template< typename T > typename itk::Image< T, 2 >::Pointer readImage2(const char *fileName)
  {
    return readImage<T, 2>(fileName);
  }

  /* --------------------------------------------------
     readImage3
  */
  template< typename T > typename itk::Image< T, 3 >::Pointer readImage3(const char *fileName)
  {
    return readImage<T, 3>(fileName);
  }


  /* --------------------------------------------------
     readImage
  */
  template< typename T, unsigned int dim > typename itk::Image< T, dim >::Pointer readImage(const char *fileName)
  {
    typedef itk::Image< T, dim > ImageType;
    typedef itk::ImageFileReader< ImageType > ImageReaderType;
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(fileName);

    typename ImageType::Pointer image;
    
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


  /*--------------------------------------------------
    writeImage2
  */
  template< typename T > void writeImage2(typename itk::Image< T, 2 >::Pointer img, const char *fileName)
  {
    writeImage<T, 2>(img, fileName);
  }


  /*--------------------------------------------------
    writeImage3
  */
  template< typename T > void writeImage3(typename itk::Image< T, 3 >::Pointer img, const char *fileName)
  {
    writeImage<T, 3>(img, fileName);
  }


  /*--------------------------------------------------
    writeImage
  */
  template< typename T, unsigned int dim > void writeImage(typename itk::Image< T, dim >::Pointer img, const char *fileName)
  {
    typedef itk::Image< T, dim > ImageType;
    typedef itk::ImageFileWriter< ImageType > WriterType;

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( fileName );
    writer->SetInput(img);

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




} // douher

////////////////////////////////////////////////////////////
#endif


