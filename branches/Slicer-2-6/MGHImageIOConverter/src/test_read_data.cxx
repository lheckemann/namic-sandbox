
/*

simple exe file to test reading the MGH header info

*/

// STL includes
#include <iostream>

// ITK includes
#include "itkOrientedVectorImage.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkMetaDataObject.h"

#include "itkMetaDataObject.h"

#include "itkByteSwapper.h"
#include "itkExceptionObject.h"
#include "itkMGHImageIO.h"
#include "itkMGHImageIOFactory.h"


template<class T>
void
testIo(const char* infname,
       const char* outfname)
{
  typedef T PixelType;

  const unsigned int Dimension = 3;

  typedef typename itk::OrientedVectorImage<PixelType,Dimension> ImageType;
  typedef typename itk::ImageFileReader<ImageType> ReaderType;

  typename ReaderType::Pointer reader = ReaderType::New();
  
  reader->SetFileName(infname);

  try
    {
      reader->Update();
      std::cout << " done reading the volume\n";
    }
  catch (itk::ExceptionObject& excp)
    {
      std::cerr << " exception\n"
    << excp << std::endl;
      exit(1);
    }

  // see if tag exists
  itk::MetaDataDictionary &thisDic=reader->GetOutput()->GetMetaDataDictionary();
  float fbuf;
  if ( itk::ExposeMetaData<float>(thisDic, "TR", fbuf) )
    {
      std::cout << " found tr tag = " << fbuf << std::endl;
    }
  if ( itk::ExposeMetaData<float>(thisDic, "TE", fbuf) )
    {
      std::cout << " found te tag = " << fbuf << std::endl;
    }

  // try writing the data
  typedef typename itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  
  writer->SetInput( reader->GetOutput() );
  writer->SetFileName( outfname );

  try {
    writer->Update();
  } catch(itk::ExceptionObject& excp)
    {
      std::cerr << " Error writing object \n"
    << excp.what() << std::endl;
      exit(1);
    }
  catch(...)
    {
      std::cerr << " Unhandled exception caught\n";
      exit(1);
    }

}


int
main(int argc,
     char* argv[])
{
  itk::ObjectFactoryBase::RegisterFactory( itk::MGHImageIOFactory::New() );

  if(argc<3)
    {
      std::cerr << " error - need 2 arguments - in file and out file\n";
      exit(1);
    }
  std::string strInput(argv[1]);

  if ( strInput.find("uchar") != std::string::npos )
    testIo<unsigned char>(argv[1], argv[2]);
  else if ( strInput.find("short") != std::string::npos )
    testIo<short>(argv[1], argv[2]);
  else if ( strInput.find("int") != std::string::npos )
    testIo<int>(argv[1],argv[2]);
  else if (strInput.find("float") != std::string::npos )
    testIo<float>(argv[1],argv[2]);
  else
    {
      std::cerr << " Unrecognized file type -> stop\n";
      exit(1);
    }
  

  return 0;
}
