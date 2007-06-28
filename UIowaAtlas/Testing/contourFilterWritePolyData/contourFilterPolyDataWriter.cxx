#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataWriter.h"

int main(int argc, char * argv [] )
{  

  // Load a color image using ITK and write it out 
  // as VTK Poly Data using a vtkContour Filter
  if( argc != 3 )
  {
    std::cerr<<"USAGE: "<<argv[0]<<" inputImageFileName outputPolyDataFileName"<<std::endl;
    exit(-1);
  }
  
  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;

  typedef itk::Image< PixelType, Dimension > ImageType;
    
  typedef itk::ImageFileReader< ImageType > ReaderType;

  typedef itk::ImageToVTKImageFilter< ImageType >  ConnectorFilterType;

  ReaderType::Pointer reader  = ReaderType::New();
  ConnectorFilterType::Pointer connector = ConnectorFilterType::New();

  reader->SetFileName( argv[1] );
  reader->Update();

  connector->SetInput(reader->GetOutput());

  vtkContourFilter *contour = vtkContourFilter::New();

  contour->SetInput( connector->GetOutput() );

  contour->SetValue(0, 1); // edges of a binary image with values 0,255

  vtkPolyDataWriter * writer = vtkPolyDataWriter::New();
  writer->SetFileName(argv[2]);
  writer->SetInput( contour->GetOutput() );
  writer->Write();
  writer->Delete();

  contour->Delete();

  return 0;
}
