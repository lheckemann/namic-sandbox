#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "vtkXMLImageDataWriter.h"

int main(int argc, char * argv [])
{  

  // Load an image using ITK and write it out as a VTK image
  if( argc != 3 )
  {
    std::cerr<<"USAGE: "<<argv[0]<<" inputImageFilename outputImageFilename"<<std::endl;
    return 1;
  }
  
  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;

  typedef itk::Image< PixelType, Dimension > ImageType;
    
  typedef itk::ImageFileReader< ImageType > ReaderType;

  typedef itk::ImageToVTKImageFilter< ImageType >  ConnectorFilterType;

  ReaderType::Pointer reader  = ReaderType::New();
  ConnectorFilterType::Pointer connector = ConnectorFilterType::New();

  reader->SetFileName(argv[1]);
  reader->Update();

  connector->SetInput(reader->GetOutput());

  vtkXMLImageDataWriter *w = vtkXMLImageDataWriter::New();
  w->SetInput(connector->GetOutput());
  w->SetFileName(argv[2]);
  w->Write();
  w->Delete();
  
  return 0;
}
