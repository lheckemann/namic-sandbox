#include <string>
#include "MyGrayscaleModelMakerCLP.h"
//ITK classes
#include "itkOrientedImage.h"
#include "itkImageFileReader.h"  //read ITK image
#include "itkImageFileWriter.h"  //write ITK image
#include "itkChangeInformationImageFilter.h"
#include "itkPluginUtilities.h"  //status report of caluculations
//VTK classes
#include "vtkImageClip.h"  //crops the image
#include "vtkImageThreshold.h"  //thresholds the image
#include "vtkImageConstantPad.h"  //fills the empty part of cropped image with 0
#include "vtkImageGaussianSmooth.h"  //Gaussian smooth of the thresholded image
#include "vtkMarchingCubes.h"  //marching cube
#include "vtkImageChangeInformation.h" //change position of marching cube result
#include "vtkPolyDataConnectivityFilter.h" //shows every component in a different color
#include "vtkXMLPolyDataWriter.h"  //write marching cube result in 3d layout
#include "vtkPluginFilterWatcher.h"  //status report of caluculations
//converter classes
#include "itkImageToVTKImageFilter.h"  //converts ITK image to VTK image
#include "itkVTKImageToImageFilter.h"  //converts ITK image to VTK image


int main(int argc, char * argv[])
{
  PARSE_ARGS;
  
  bool debug = true;
  
  if (debug) 
  {
    std::cout << "The input volume is: " << MyInputVolume << std::endl;
    std::cout << "The output geometry: " << MyOutputGeometry << std::endl;
    std::cout << "The output volume (label map) is: " << MyOutputVolume << std::endl;
  }
  
  //defines the image type using their pixel type (e.g. short) and dimension (e.g. 3 for 3D)
  typedef itk::OrientedImage<short, 3> OrientedImageType;
  
  //creates an image with the method New() and assigning the result to the Pointer class
  OrientedImageType::Pointer orientedImage = OrientedImageType::New();

  //defines the ITK reader type using their oriented image type
  typedef itk::ImageFileReader<OrientedImageType> ReaderType;

  //creates a ITK reader with the method New() and assigning the result to the Pointer class
  ReaderType::Pointer reader = ReaderType::New();

  //itk::PluginFilterWatcher watchReader(reader, "read itk image", CLPProcessInformation);
  
  //sets file name for reader (same names as in xml file)
  reader->SetFileName(MyInputVolume.c_str());
  
  //reads the ITK image
  reader->Update();
  
  //gets the ITK image information (origin, spacing, direction)
  const OrientedImageType::PointType inputOrigin = reader->GetOutput()->GetOrigin(); 
  const OrientedImageType::SpacingType inputSpacing = reader->GetOutput()->GetSpacing();
  const OrientedImageType::DirectionType inputDirection = reader->GetOutput()->GetDirection();
  
  if (debug)
  {
    std::cout << "Input image origin: " << inputOrigin << std::endl;
    std::cout << "Input image spacing: " << inputSpacing << std::endl;
    std::cout << "Input image direction: " << inputDirection << std::endl;
  }
  
  
  //defines and creats a convertor from ITK to VTK image 
  typedef itk::ImageToVTKImageFilter<OrientedImageType> ConvertorITKtoVTKType;
  ConvertorITKtoVTKType::Pointer convertorITKtoVTK = ConvertorITKtoVTKType::New();
  convertorITKtoVTK->SetInput(reader->GetOutput()); //now it's a VTK image (name: convertorITKtoVTK)
  convertorITKtoVTK->Update();
  
  //------------Crop the VTK image------------
  int wholeExtent[6];
  (convertorITKtoVTK->GetOutput())->GetWholeExtent(wholeExtent); //get extend information of file
  
  if (debug)
  {
    std::cout << "Input image whole extent: " << wholeExtent[0] << " " << wholeExtent[1] << " " << wholeExtent[2] << 
      " " << wholeExtent[3] << " " << wholeExtent[4] << " " << wholeExtent[5] << std::endl;
  }
  
  vtkImageClip * imageClip = vtkImageClip::New();
  imageClip->SetInput(convertorITKtoVTK->GetOutput());
//   imageClip->ClipDataOn();
  
  vtkPluginFilterWatcher watchCropping(imageClip,
                                      "crop the image",
                                      CLPProcessInformation,
                                      1.0, 0.0);  //status report of cropping
  
  int clipExtent[6]; //crop information from gui
  clipExtent[0] = wholeExtent[0] + MyXmin*0.01*(wholeExtent[1]-wholeExtent[0]);
  clipExtent[1] = wholeExtent[0] + MyXmax*0.01*(wholeExtent[1]-wholeExtent[0]);
  clipExtent[2] = wholeExtent[2] + MyYmin*0.01*(wholeExtent[3]-wholeExtent[2]);
  clipExtent[3] = wholeExtent[2] + MyYmax*0.01*(wholeExtent[3]-wholeExtent[2]);
  clipExtent[4] = wholeExtent[4] + MyZmin*0.01*(wholeExtent[5]-wholeExtent[4]);
  clipExtent[5] = wholeExtent[4] + MyZmax*0.01*(wholeExtent[5]-wholeExtent[4]);
  
  if (debug)
  {
    std::cout << "Cropped image clip extent: " << clipExtent[0]<< " " << clipExtent[1] << " " << clipExtent[2]<< 
      " " << clipExtent[3] << " " << clipExtent[4] << " " << clipExtent[5] << std::endl;
  }
  
  imageClip->SetOutputWholeExtent(clipExtent);  //crop the image
  imageClip->Update();
  
  //------------Thresholding of VTK image------------
  vtkImageThreshold * imageThreshold = vtkImageThreshold::New();
  imageThreshold->SetInput(imageClip->GetOutput());

  //gets the thresholding information from the gui (same names as in xml file)
  int lowerThreshold = MyLowerThreshold;
  int upperThreshold = MyUpperThreshold;

  if (debug)
  {
    std::cout << "Thresholding the cropped image with following parameters:" << std::endl;
    std::cout << "lowerThreshold: " << lowerThreshold <<std::endl;
    std::cout << "upperThreshold: " << upperThreshold <<std::endl;
  }
  
  vtkPluginFilterWatcher watchThresholding(imageThreshold,
                                           "thresholding the cropped image",
                                           CLPProcessInformation,
                                           1.0, 0.0);  //status report of thresholding
  
  imageThreshold->ThresholdBetween(MyLowerThreshold, MyUpperThreshold); 
  imageThreshold->SetInValue(255);
  imageThreshold->SetOutValue(0);
  imageThreshold->Update();  //now the image is thresholded
  
  double thresholdBounds[6];
  
  if (debug)
  {
    double thresholdOrigin[3];
    double thresholdSpacing[3];

    
    imageThreshold->GetOutput()->GetOrigin(thresholdOrigin);
    imageThreshold->GetOutput()->GetSpacing(thresholdSpacing);
    imageThreshold->GetOutput()->GetBounds(thresholdBounds);
    
    std::cout << "Threshold image origin: " << thresholdOrigin[0] << " " << thresholdOrigin[1] << " " << 
      thresholdOrigin[2] << std::endl;
    std::cout << "Threshold image spacing: " << thresholdSpacing[0] << " " << thresholdSpacing[1] << " " << 
      thresholdSpacing[2] << std::endl;
    std::cout << "Threshold image bounds: " << thresholdBounds[0] << " " << thresholdBounds[1] << " " << 
      thresholdBounds[2] << thresholdBounds[3] << " " << thresholdBounds[4] << " " << thresholdBounds[5] << std::endl;
  }
  
  //necessary to set the right extend information for the cropped image
  //must be after thresholding, because now threshold is only of the cropped image
  //and not of unuseful information (like after constant pad)
  vtkImageConstantPad * imageConstantPad = vtkImageConstantPad::New();
  imageConstantPad->SetInput(imageThreshold->GetOutput());
  imageConstantPad->SetOutputWholeExtent(wholeExtent);
  
  
  //------------gaussian smooth of thresholded image------------
  vtkImageGaussianSmooth * gaussianSmooth = vtkImageGaussianSmooth::New();
  
  vtkPluginFilterWatcher watchGaussianSmooth(gaussianSmooth,
                                    "Gaussian Smooth",
                                     CLPProcessInformation,
                                     1.0, 0.0);
  
  gaussianSmooth->SetInput(imageThreshold->GetOutput());
//gaussianSmooth->SetRadiusFactors(1,1,1);
//gaussianSmooth->SetStandardDeviations(1,1,1);
  
  if (debug)
  {
    double radiusFactors[3];
    double standardDeviations[3];
    
    gaussianSmooth->GetRadiusFactors(radiusFactors);
    gaussianSmooth->GetStandardDeviations(standardDeviations);
    
    std::cout << "Gaussian Smooth with parameters: " << std::endl;
    std::cout << "radius factors: " << radiusFactors[0] << " " << radiusFactors[1] << " " <<
      radiusFactors[2] << std::endl;
    std::cout << "standard deviations: " << standardDeviations[0] << " " << standardDeviations[1] << " " <<
      standardDeviations[2] << std::endl; 
  }
  
  
  vtkImageChangeInformation * imageChangeInformation = vtkImageChangeInformation::New();
  imageChangeInformation->SetInput(gaussianSmooth->GetOutput());
  
  //TODO change to the right origin
  imageChangeInformation->SetOutputOrigin(inputOrigin[0],inputOrigin[1],inputOrigin[2]);
  imageChangeInformation->Update();
  
  //------------marching cube of gaussian smoothed image------------
   
  vtkMarchingCubes * marchingCubes = NULL;
  marchingCubes = vtkMarchingCubes::New();
  vtkPluginFilterWatcher watchMarchingCubes(marchingCubes,
                                    "Marching Cubes",
                                     CLPProcessInformation,
                                     1.0, 0.0);

  marchingCubes->SetInput(imageChangeInformation->GetOutput());
  marchingCubes->SetValue(0,127.5);
  marchingCubes->ComputeScalarsOff();
  marchingCubes->ComputeGradientsOff();
  marchingCubes->ComputeNormalsOn();
  marchingCubes->GetOutput()->ReleaseDataFlagOn();
  marchingCubes->Update();

  if (debug)
  {
    std::cout << "marching cube: number of polygons = " << marchingCubes->GetOutput()->GetNumberOfPolys() << endl;
  }
  
  
  //to get different colors of the marching cubes
  vtkPolyDataConnectivityFilter *polyDataConnectivityFilter = vtkPolyDataConnectivityFilter::New();
  polyDataConnectivityFilter->SetInput(marchingCubes->GetOutput());
  polyDataConnectivityFilter->SetExtractionModeToAllRegions();
  polyDataConnectivityFilter->ColorRegionsOn();
 
  
  //------------write the result of marching cube ------------
  vtkXMLPolyDataWriter * writerPolyDataWriter = NULL;
  writerPolyDataWriter = vtkXMLPolyDataWriter::New();
  writerPolyDataWriter->SetInput(polyDataConnectivityFilter->GetOutput());
  writerPolyDataWriter->SetFileName(MyOutputGeometry.c_str());
  writerPolyDataWriter->Write();
  
  
  //------------write the result of thresholding ------------
  //first converting VTK to ITK image
  typedef itk::VTKImageToImageFilter<OrientedImageType> ConvertorVTKtoITKType;
  ConvertorVTKtoITKType::Pointer convertorVTKtoITK = ConvertorVTKtoITKType::New();
  convertorVTKtoITK->SetInput(imageConstantPad->GetOutput());
  
  convertorVTKtoITK->Update();
  
  //second set the same itk image information (origin, spacing, direction) in the output image --ONLY DIRECTION
  //ORIGIN AND SPACING ARE DIFFERENT AFTER CROPPING!!!
  typedef itk::ChangeInformationImageFilter<OrientedImageType> ChangeInformationImageFilterType;
  ChangeInformationImageFilterType::Pointer changeInformationImageFilter = ChangeInformationImageFilterType::New();
  changeInformationImageFilter->SetInput(convertorVTKtoITK->GetOutput());
  
//changeInformationImageFilter->ChangeOriginOn();  //now it is possible to change/ set the information
//changeInformationImageFilter->ChangeSpacingOn();
  changeInformationImageFilter->ChangeDirectionOn();
//changeInformationImageFilter->SetOutputOrigin(newOrigin);  //change/ set the information
//changeInformationImageFilter->SetOutputSpacing(inputSpacing);
  changeInformationImageFilter->SetOutputDirection(inputDirection);
  changeInformationImageFilter->Update();

  if (debug)
  {
    OrientedImageType::PointType changeOrigin = changeInformationImageFilter->GetOutput()->GetOrigin(); 
    OrientedImageType::SpacingType changeSpacing = changeInformationImageFilter->GetOutput()->GetSpacing(); 
    OrientedImageType::DirectionType changeDirection = changeInformationImageFilter->GetOutput()->GetDirection();
  
    std::cout << "Output image origin: " << changeOrigin << std::endl;
    std::cout << "Output image spacing: " << changeSpacing << std::endl;
    std::cout << "Output image direction: " << changeDirection << std::endl;
  }
  
  //third write the file
  typedef itk::ImageFileWriter<OrientedImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(MyOutputVolume.c_str());
//itk::PluginFilterWatcher watchWriter(writer, "write itk image", CLPProcessInformation);
  writer->SetInput(changeInformationImageFilter->GetOutput()); 
  writer->Update();
  writer->Write();
  
  
  //Cleanup
  if (imageClip) imageClip->Delete();
  if (imageThreshold) imageThreshold->Delete();
  if (imageConstantPad) imageConstantPad->Delete();
  if (gaussianSmooth) gaussianSmooth->Delete();
  if (marchingCubes) marchingCubes->Delete();
  if (polyDataConnectivityFilter) polyDataConnectivityFilter->Delete();
  if (writerPolyDataWriter) writerPolyDataWriter->Delete();

  return EXIT_SUCCESS;
}
