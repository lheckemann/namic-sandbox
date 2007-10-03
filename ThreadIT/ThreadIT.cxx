#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkGDCMImageIO.h>
#include <itkImageSeriesReader.h>
#include <itkLogger.h>
#include <itkLoggerBase.h>
#include <itkStdStreamLogOutput.h>
#include <itkSimpleFilterWatcher.h>
#include <itkBilateralImageFilter.h>
#include <itkTimeProbesCollectorBase.h>
#include "itkBilateralZThreadImageFilter.h"
#include "itkMedianZThreadImageFilter.h"
#include <itkMedianImageFilter.h>

typedef itk::Image<short, 3> Volume;

Volume::Pointer ReadInput ( std::string Filename ) {
  typedef itk::ImageFileReader<Volume> FileReaderType;
  Volume::Pointer volume = NULL;

  // If this is a DICOM image, read that way.
  itk::GDCMImageIO::Pointer DicomIO = itk::GDCMImageIO::New();
  // std::cout << "Checking GDCM->CanReadFile" << std::endl;
  // DicomIO->CanReadFile ( Filename.c_str() ) )
  if ( itksys::SystemTools::FileIsDirectory( Filename.c_str() ) )
    {
      std::cout << "Reading Dicom: " << Filename << std::endl;
      itk::GDCMSeriesFileNames::Pointer names = itk::GDCMSeriesFileNames::New();  
      std::string fileNamePath = Filename;
      std::cout << "fileNamePath: " << fileNamePath << std::endl;
      typedef itk::ImageSeriesReader<Volume> Reader;
      Reader::Pointer inputReader = Reader::New();
      names->SetDirectory( fileNamePath );
      inputReader->SetFileNames ( names->GetInputFileNames() );
      inputReader->Update();
      volume = inputReader->GetOutput();
      }
    else
      {
      std::cout << "Reading using standard readers: " << Filename << std::endl;
      // Use the standard readers
      typedef itk::ImageFileReader<Volume> Reader;
      Reader::Pointer reader = Reader::New();
      reader->SetFileName ( Filename.c_str() );
      reader->Update();
      volume = reader->GetOutput();
      }
  return volume;
}

int main ( int argc, const char* argv[] ) 
{
  itk::OStringStream msg;
  itk::StdStreamLogOutput::Pointer coutput = itk::StdStreamLogOutput::New();
  coutput->SetStream(std::cout);
  itk::Logger::Pointer logger = itk::Logger::New();
  logger->SetName ( "ThreadIT" );
  logger->AddLogOutput ( coutput );

  Volume::Pointer input, output;

  if ( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0] << " <input> <output>" << "\n"
              << "\t<input> may be a directory filled with DICOM files, or any fileformat ITK understands" << "\n"
              << "\t<output> is the name of the file to overwrite" << std::endl;
    return EXIT_FAILURE;
    }
  std::string inputFilename ( argv[1] );
  std::string outputFilename ( argv[2] );
  try
    {
    input = ReadInput ( inputFilename.c_str() );
    }
  catch( itk::ExceptionObject & err )
    {
    msg.str(""); msg << "Error Reading images: " << err;
    std::cerr << msg.str() << std::endl;
    return EXIT_FAILURE;
    }

  itk::TimeProbesCollectorBase probe;

  // Process here
  typedef itk::BilateralZThreadImageFilter<Volume,Volume> BilateralZThreadType;
  BilateralZThreadType::Pointer bilateralZ = BilateralZThreadType::New();
  // bilateralZ->SetInput ( input );

  typedef itk::MedianZThreadImageFilter<Volume,Volume> MedianZThreadType;

  MedianZThreadType::InputSizeType sz;
  sz[0] = 3;
  sz[1] = 3;
  sz[2] = 3;

  ZThread::PoolExecutor executor((size_t)bilateralZ->GetMultiThreader()->GetNumberOfThreads());
  char buffer[128];
  for ( int i = 1; i < bilateralZ->GetMultiThreader()->GetNumberOfThreads() * 2; i++ )
    {
    MedianZThreadType::Pointer medianZ = MedianZThreadType::New();
    medianZ->executor = &executor;
    medianZ->SetRadius ( sz );
    medianZ->SetInput ( input );
    medianZ->SetNumberOfPieces ( i );

    sprintf ( buffer, "MedianZ %02d Chunk", i );
    probe.Start ( buffer );
    // itk::SimpleFilterWatcher watcher ( medianZ, "MedianZ" );
    medianZ->Update();
    probe.Stop ( buffer );
    std::cout << "Finished: " << buffer << std::endl;
    
    typedef itk::MedianImageFilter<Volume,Volume> MedianType;
    MedianType::Pointer median = MedianType::New();
    median->SetRadius ( sz );
    median->SetInput ( input );
    median->GetMultiThreader()->SetNumberOfThreads ( i );
    
    sprintf ( buffer, "Median %02d Threads", i );
    probe.Start ( buffer );
    // itk::SimpleFilterWatcher watcher2 ( median, "Median" );
    median->Update();
    probe.Stop ( buffer );
    }


  probe.Report();
  
  typedef itk::ImageFileWriter<Volume> WriterType;
  WriterType::Pointer Writer = WriterType::New();
  Writer->SetFileName ( outputFilename.c_str()  );
  // Writer->SetInput ( medianZ->GetOutput() );
    try
      {
      // Writer->Write();
      }
    catch( itk::ExceptionObject & err )
      { 
      std::cerr << "Failed to write file " << outputFilename << "\n" << err << std::endl;
      return EXIT_FAILURE;
      }

  return EXIT_SUCCESS;

}
