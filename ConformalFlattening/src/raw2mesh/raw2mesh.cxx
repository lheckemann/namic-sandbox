#pragma warning (disable: 4786)

#include <iostream>
#include <fstream>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkMesh.h"

int main(int argc, char *argv[])
{
  /* Check for right number of command-line arguments */
  if( argc < 2 )
    {
    std::cerr << "Usage arguments: InputImage OutputMesh" << std::endl;
    return EXIT_FAILURE;
    }
  
  std::cerr << "#####################################################" << std::endl;
  std::cerr << "## CONVERT SKULL-STRIPPED BINARY IMAGE TO ITK MESH ##" << std::endl;
  std::cerr << "#####################################################" << std::endl;
  std::cerr << std::endl;

  /* Readin the Skull-Stripped Binary Image from File */
  typedef itk::Image< unsigned char, 3 > InputImageType;
  typedef itk::ImageFileReader< InputImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  /* Connected Component Filter */
  std::cerr << "Executing the Connected Component and Relabel Filter... " << std::endl;
  typedef itk::ConnectedComponentImageFilter< InputImageType, InputImageType > ConnectedComponentType;
  ConnectedComponentType::Pointer connectedComponentFilter = ConnectedComponentType::New();
  connectedComponentFilter->SetInput( reader->GetOutput() );
  connectedComponentFilter->SetFullyConnected( 1 );
  std::cerr << "  GetFullyConnected = " << connectedComponentFilter->GetFullyConnected() << std::endl;

  /* Relabel Filter */
  typedef itk::RelabelComponentImageFilter< InputImageType, InputImageType > RelabelType;
  RelabelType::Pointer relabelFilter = RelabelType::New();
  relabelFilter->SetInput( connectedComponentFilter->GetOutput() );
  relabelFilter->InPlaceOn();
  relabelFilter->Update();
  std::cerr << "  Total # of connected components = " << relabelFilter->GetNumberOfObjects() << std::endl;

  /* Compute Label Statistics on the Original Labels */
  std::ofstream labelStatistics ("labelStatistics.txt");
  std::cerr << "Computing label statistics... " << std::endl;
  typedef itk::LabelStatisticsImageFilter<InputImageType, InputImageType> LabelStatisticsImageFilterType;
  LabelStatisticsImageFilterType::Pointer statisticsFilterOrig = LabelStatisticsImageFilterType::New();
  statisticsFilterOrig->SetInput( reader->GetOutput() );
  statisticsFilterOrig->SetLabelInput( relabelFilter->GetOutput() );
  //statisticsFilterOrig->SetUseHistograms(false);
  //statisticsFilterOrig->SetHistogramParameters(300,-1024,1024);
  statisticsFilterOrig->Update();
  labelStatistics << "Label, Count" << "\n";
  for (unsigned long i = 0; i < relabelFilter->GetNumberOfObjects() ; i++)
    {
    labelStatistics << i << ", " << statisticsFilterOrig->GetCount((InputImageType::PixelType) i) << "\n";
    }
  labelStatistics.close();

  /* Write Relabeled Image to File (to help with debugging) */
  std::cerr << "Writing relabeled image to file... " << std::endl;
  itk::ImageFileWriter<RelabelType::OutputImageType>::Pointer relabelWriter;
  relabelWriter = itk::ImageFileWriter<RelabelType::OutputImageType>::New();
  relabelWriter->SetFileName( "relabeledImage.mhd" );
  relabelWriter->SetInput( relabelFilter->GetOutput() );
  relabelWriter->Update();
  relabelWriter = 0;

  /* Extract the Largest Connected Component (ignore all others) */
  typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > BinaryThresholdFilterType;
  BinaryThresholdFilterType::Pointer binaryThresholdFilter = BinaryThresholdFilterType::New();
  binaryThresholdFilter->SetInput( relabelFilter->GetOutput() );
  binaryThresholdFilter->SetOutsideValue( 0 );
  binaryThresholdFilter->SetInsideValue( 1 );
  binaryThresholdFilter->SetLowerThreshold( 1 );
  binaryThresholdFilter->SetUpperThreshold( 1 );
  binaryThresholdFilter->Update();

  /* Generate an ITK Mesh from the Largest Connected Component */
  /* NOTE: Can probably skip the binary threshold image filter */
  typedef itk::Mesh<double> MeshType;
  typedef itk::BinaryMask3DMeshSource< InputImageType, MeshType > MeshSourceType;
  MeshSourceType::Pointer meshSource = MeshSourceType::New();
  const InputImageType::PixelType objectValue = static_cast<InputImageType::PixelType>( 1 );
  meshSource->SetObjectValue( objectValue );
  meshSource->SetInput( binaryThresholdFilter->GetOutput() );
  try
    {
    meshSource->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown during Update() on meshSource " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }
  std::cerr << "Nodes = " << meshSource->GetNumberOfNodes() << std::endl;
  std::cerr << "Cells = " << meshSource->GetNumberOfCells() << std::endl;
  
  return 1;
}
