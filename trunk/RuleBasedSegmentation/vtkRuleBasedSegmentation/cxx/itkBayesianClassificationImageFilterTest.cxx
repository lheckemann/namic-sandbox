#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBayesianClassificationImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkRescaleIntensityImageFilter.h"

int main(int argc, char *argv[])
{
  typedef unsigned long VoxelType;
  const unsigned int Dimension = 3;
  if( argc < 4 )
    {
    std::cerr << "Usage arguments: InputImage ClassifiedImage numberOfClasses numberOfSmoothingIterations [Optional: MaskImage MaskValue]" << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::Image< VoxelType, Dimension > InputImageType;
  typedef itk::Image< VoxelType, Dimension > LabelImageType;
  typedef itk::Image< VoxelType, Dimension > MaskImageType;

  typedef itk::BayesianClassificationImageFilter<
      InputImageType, LabelImageType, MaskImageType > ClassifierType;

  ClassifierType::Pointer filter = ClassifierType::New();
  filter->SetNumberOfClasses( atoi(argv[3]) );
  filter->SetNumberOfSmoothingIterations( atoi(argv[4]) );
    
  typedef itk::ImageFileReader< InputImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    std::cerr << "Successfully read the input image!" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  
  filter->SetInput(reader->GetOutput());

  typedef itk::ImageFileReader< MaskImageType > MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  if( argc == 7 )
    {
    maskReader->SetFileName( argv[5] );

    try
      {
      maskReader->Update();
      std::cerr << "Successfully read the mask image!" << std::endl;
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
      }
  
    // DEBUGGING: Count number of ON pixels
    typedef itk::ImageRegionConstIterator< MaskImageType > MaskIteratorType;
    MaskIteratorType maskIt( maskReader->GetOutput(), maskReader->GetOutput()->GetRequestedRegion() );
    int count = 0;
    for( maskIt.GoToBegin(); !maskIt.IsAtEnd(); ++maskIt )
      {
      if( maskIt.Get() == atoi(argv[6]) )
        {
        count = count + 1;
        }
      }
    std::cerr << "The Mask Value is:  " << atoi(argv[6]) << std::endl;
    std::cerr << "The Number of Masked Pixels are:  " << count << std::endl;
    // End DEBUGGING

    filter->SetMaskImage( maskReader->GetOutput() );
    filter->SetMaskValue( atoi(argv[6]) );
    }

  try
    {
    filter->Update();
    std::cerr << "Successfully executed the filter!" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  
  //
  // Setup writer.. Rescale the label map to the dynamic range of the 
  // datatype and write it 
  //
  typedef ClassifierType::OutputImageType            ClassifierOutputImageType;
  typedef itk::Image< unsigned char, Dimension >     OutputImageType;
  typedef itk::RescaleIntensityImageFilter< 
    ClassifierOutputImageType, OutputImageType >     RescalerType;
  RescalerType::Pointer rescaler = RescalerType::New();
  rescaler->SetInput( filter->GetOutput() );
  rescaler->SetOutputMinimum( 0 );
  rescaler->SetOutputMaximum( 255 );

  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( rescaler->GetOutput() );
  writer->SetFileName( argv[2] );
  
  try
    {
    writer->Update();
    std::cerr << "Successfully wrote the output image!" << std::endl;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

