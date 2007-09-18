#include "itkDanielssonDistanceMapImageFilter.h"
#include "WhiteMatterMaskBlurCLP.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"

int main( int argc, char* argv[] ){
  PARSE_ARGS;
  typedef unsigned short InputPixelType;
  typedef float OutputPixelType;
  typedef itk::Image< InputPixelType, 3 > InputImageType;
  typedef itk::Image< OutputPixelType, 3 > OutputImageType;
  
  typedef itk::DanielssonDistanceMapImageFilter<
    InputImageType, OutputImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  
  typedef itk::ImageFileReader< InputImageType > ReaderType;
  typedef itk::ImageFileWriter< OutputImageType > WriterType;
  
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  
  reader->SetFileName( whitemattermaskfilename.c_str() );
  writer->SetFileName( blurredwhitemattermaskfilename.c_str() );
  
  filter->SetInput( reader->GetOutput() );
  writer->SetInput( filter->GetOutput() );
  
  filter->InputIsBinaryOn();
  
  filter->Update();
  
  typedef itk::ImageRegionIterator< OutputImageType > OutputImageIteratorType;
  
  OutputImageIteratorType outputimageIt( filter->GetOutput(),
    filter->GetOutput()->GetRequestedRegion() );
    
  for(outputimageIt.GoToBegin(); !outputimageIt.IsAtEnd(); ++outputimageIt){
    if(outputimageIt.Value() <= 0 ){
      outputimageIt.Value() = 1;
    }
    else if(outputimageIt.Value() > threshold){
      outputimageIt.Value() = 0;
    }
    else{
      outputimageIt.Value() = 1/(outputimageIt.Value() + 1);
    }
  }
  //set a threshold
  //normalize to this threshold
    //set all voxels larger than threshold to zero
    //normalize remaining voxels by adding 1 and dividing by threshold
  writer->Update();
  
  return EXIT_SUCCESS;
}
