#include "raw2mesh.h"

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
  ConnectedComponentType::Pointer connectedComponentFilter = ConnectedComponentType::New();
  connectedComponentFilter->SetInput( reader->GetOutput() );
  connectedComponentFilter->SetFullyConnected( 0 );
  std::cerr << "  GetFullyConnected = " << connectedComponentFilter->GetFullyConnected() << std::endl;

  /* Relabel Filter */
  RelabelType::Pointer relabelFilter = RelabelType::New();
  relabelFilter->SetInput( connectedComponentFilter->GetOutput() );
  relabelFilter->InPlaceOn();
  relabelFilter->Update();
  std::cerr << "  Total # of connected components = " << relabelFilter->GetNumberOfObjects() << std::endl;

  /* Compute Label Statistics on the Original Labels */
  std::ofstream labelStatistics ("labelStatistics.txt");
  std::cerr << "Computing label statistics... " << std::endl;
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
  BinaryThresholdFilterType::Pointer binaryThresholdFilter = BinaryThresholdFilterType::New();
  binaryThresholdFilter->SetInput( relabelFilter->GetOutput() );
  binaryThresholdFilter->SetOutsideValue( 0 );
  binaryThresholdFilter->SetInsideValue( 1 );
  binaryThresholdFilter->SetLowerThreshold( 1 );
  binaryThresholdFilter->SetUpperThreshold( 1 );
  binaryThresholdFilter->Update();

  /* Generate an ITK Mesh from the Largest Connected Component */
  /* NOTE: Can probably skip the binary threshold image filter */
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
  
  //Begin convert from ITKMesh to vtkPolyData
  vtkPolyData* polyData = ITKMeshToVtkPolyData( meshSource->GetOutput() );

  //Display the new polydata
  Display( polyData );


//   /* Create the Mesh Spatial Object */
//   typedef itk::MeshSpatialObject<MeshType> MeshSpatialObjectType;
//   MeshSpatialObjectType::Pointer meshSO = MeshSpatialObjectType::New();
//   meshSO->SetMesh(meshSource->GetOutput());
//   meshSO->SetId(3);
//   std::cout<<"[PASSED]"<<std::endl;

//   /* Writing the Mesh to File */
//   std::cout<<"Testing Writing MeshSpatialObject: ";
//   typedef itk::DefaultDynamicMeshTraits< float , 3, 3 > MeshTrait;
//   typedef itk::SpatialObjectWriter<3,float,MeshTrait> SOWriterType;
//   SOWriterType::Pointer soWriter = SOWriterType::New();
//   soWriter->SetInput(meshSO);
//   soWriter->SetFileName("metamesh.vtk");
//   soWriter->Update();
//   std::cout<<"[PASSED]"<<std::endl;

  return 1;
}
