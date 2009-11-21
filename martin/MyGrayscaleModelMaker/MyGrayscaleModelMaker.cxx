#include <string>
#include "MyGrayscaleModelMakerCLP.h"
//ITK classes
#include "itkImage.h"
#include "itkImageFileReader.h"  //read ITK image
#include "itkImageFileWriter.h"  //write ITK image
//VTK classes
#include "vtkImageThreshold.h"  //thresholds the image
#include "vtkMarchingCubes.h"  //marching cube
#include "vtkXMLPolyDataWriter.h"  //write marching cube result in 3d layout
#include "vtkPluginFilterWatcher.h"  //status report of caluculations
//converter classes
#include "itkImageToVTKImageFilter.h"  //converts ITK image to VTK image
#include "itkVTKImageToImageFilter.h"  //converts ITK image to VTK image


int main(int argc, char * argv[])
{
  PARSE_ARGS;
    
  //defines the image type using their pixel type (e.g. short) and dimension (e.g. 3 for 3D)
  typedef itk::Image<float,3> ImageType;
  
  //creates an image with the method New() and assigning the result to the Pointer class
  ImageType::Pointer image = ImageType::New();

  //defines the ITK reader type using their image type
  typedef itk::ImageFileReader<ImageType> ReaderType;

  //creates a ITK reader with the method New() and assigning the result to the Pointer class
  ReaderType::Pointer reader = ReaderType::New();

  //sets file name for reader (same names as in xml file)
  reader->SetFileName(MyInputVolume.c_str() );
  
  //reads the ITK image
  reader->Update();
  
  //defines and creats a convertor from ITK to VTK image 
  typedef itk::ImageToVTKImageFilter<ImageType> ConvertorITKtoVTKType;
  ConvertorITKtoVTKType::Pointer convertorITKtoVTK = ConvertorITKtoVTKType::New();
  convertorITKtoVTK->SetInput(reader->GetOutput()); //now it's a VTK image (name: convertorITKtoVTK)
  
  //------------Thresholding of VTK image------------
  std::cout << "-------------start thresholding-------------" << std::endl;

  vtkImageThreshold * thresholdFilter = vtkImageThreshold::New();
  thresholdFilter->SetInput(convertorITKtoVTK->GetOutput());
   
  //gets the thresholding information from the gui (same names as in xml file)
  int lowerThreshold = MyLowerThreshold;
  int upperThreshold = MyUpperThreshold;
  
  std::cout << "Threshold the image with:" << std::endl;
  std::cout << "lowerThreshold: " << lowerThreshold <<std::endl;
  std::cout << "upperThreshold: " << upperThreshold <<std::endl;
   
  vtkPluginFilterWatcher watchThresholding(thresholdFilter,
                                           "thresholding the image",
                                           CLPProcessInformation,
                                           1.0, 0.0);  //status report of thresholding
  
  thresholdFilter->ThresholdBetween(MyLowerThreshold, MyUpperThreshold); 
//thresholdFilter->SetReplaceIn(1);
  thresholdFilter->SetInValue(255);
//thresholdFilter->SetReplaceOut(1);
  thresholdFilter->SetOutValue(0); 
  thresholdFilter->Update();  //now the image is thresholded
   
   
  //------------marching cube of thresholded image------------
  std::cout << "-------------start marching cube-------------" << std::endl;
   
  vtkMarchingCubes * mcubes = NULL;
  mcubes = vtkMarchingCubes::New();
  vtkPluginFilterWatcher watchMCubes(mcubes,
                                    "Marching Cubes",
                                     CLPProcessInformation,
                                     1.0, 0.0);

  mcubes->SetInput(thresholdFilter->GetOutput());
  mcubes->SetValue(0,127.5);
  mcubes->ComputeScalarsOff();
  mcubes->ComputeGradientsOff();
  mcubes->ComputeNormalsOn();
  (mcubes->GetOutput())->ReleaseDataFlagOn();
  mcubes->Update();

  std::cout << "Number of polygons = " << mcubes->GetOutput()->GetNumberOfPolys() << endl;
  
  std::cout << "-------------start writing-------------" << std::endl;
  
 
  //------------write the result of marching cube ------------
  vtkXMLPolyDataWriter * writerPolyDataWriter = NULL;
  writerPolyDataWriter = vtkXMLPolyDataWriter::New();
  writerPolyDataWriter->SetInput(mcubes->GetOutput());
  writerPolyDataWriter->SetFileName(MyOutputGeometry.c_str());
  writerPolyDataWriter->Write();

  //------------write the result of thresholding ------------
  //first converting VTK to ITK image
  typedef itk::VTKImageToImageFilter< ImageType> ConvertorVTKtoITKType;
  ConvertorVTKtoITKType::Pointer convertorVTKtoITK = ConvertorVTKtoITKType::New();
  convertorVTKtoITK->SetInput(thresholdFilter->GetOutput()); 
  
  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(MyOutputVolume.c_str());
  writer->SetInput(convertorVTKtoITK->GetOutput());
  writer->Write();
    
    
  //Cleanup
  if (thresholdFilter) thresholdFilter->Delete();
  if (mcubes) mcubes->Delete();
  if (writerPolyDataWriter) writerPolyDataWriter->Delete();

  return EXIT_SUCCESS;
}
