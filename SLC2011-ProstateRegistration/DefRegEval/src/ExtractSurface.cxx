// Convert binary metaimage to smoothed STL mesh

#include "DefRegEval.h"

#include "vtkOutputWindow.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkContourFilter.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkSTLWriter.h"
#include "vtkMetaImageWriter.h"
#include "vtkImageStencil.h"
#include "vtkTriangleFilter.h"
#include "vtkStripper.h"
#include "vtkImageData.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkCellData.h"
#include "vtkDecimatePro.h"

#include "IO/vtkSMeshWriter.h"

/////////////////////

void SetMaterial(vtkPolyData *poly, int materialId)
{
  int numberOfCells=poly->GetNumberOfCells();
  vtkSmartPointer<vtkIntArray> materialArray=vtkSmartPointer<vtkIntArray>::New();
  materialArray->SetName(DefRegEvalGlobal::ArrayNameMaterial);
  materialArray->SetNumberOfComponents(1);
  materialArray->SetNumberOfTuples(numberOfCells);
  poly->GetCellData()->AddArray(materialArray);  
  for (int cid=0; cid<numberOfCells; cid++)
  {
    materialArray->SetValue(cid, materialId);
  }
}

int main(int argc, char *argv[])
{
  VTK_LOG_TO_CONSOLE;

  std::string outputCombinedSurfaceMeshFilename;
  std::string inputImageFilename;
  std::string outputObjectSurfaceFilename;
  std::string outputObjectImageFilename;
  std::string outputSupportSurfaceFilename;
  vtkstd::vector<double> supportPosition; // LPS
  double supportRadius=40; // mm
  double objectSegmentationThreshold=0; // Threshold for surface extraction


  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--OrganImageInputFn", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageFilename, "Input image filename (mha)");
  args.AddArgument("--OrganImageThreshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &objectSegmentationThreshold, "Segmentation threshold value");
  args.AddArgument("--OrganSurfMeshOutputFn", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputObjectSurfaceFilename, "Output surface filename of the object (stl) [optional]");
  args.AddArgument("--OrganImageOutputFn", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputObjectImageFilename, "Smoothed image corresponding to the extracted surface (stl)");  

  args.AddArgument("--SupportPosition", vtksys::CommandLineArguments::MULTI_ARGUMENT, &supportPosition, "Center of the support material sphere"); 
  args.AddArgument("--SupportRadius", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &supportRadius, "Radius of the support material sphere"); 
  args.AddArgument("--SupportSurfMeshOutputFn", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputSupportSurfaceFilename, "Output surface filename of the support (stl) [optional]");

  args.AddArgument("--CombinedSurfMeshOutputFn", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputCombinedSurfaceMeshFilename, "Output combined surface filename for tetgen mesher (smesh)");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (supportPosition.size()!=3)
  {
    std::cerr << "Invalid supportPosition " << std::endl;
    exit(EXIT_FAILURE);
  }

  if (inputImageFilename.empty())
  {
    std::cerr << "OrganImageInputFn parameter is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Parsing OK, input image filename: " << inputImageFilename << std::endl;

  /////////////

  vtkSmartPointer<vtkMetaImageReader> reader=vtkSmartPointer<vtkMetaImageReader>::New();
  reader->SetFileName(inputImageFilename.c_str());

  vtkSmartPointer<vtkContourFilter> objectSurfaceExtractor=vtkSmartPointer<vtkContourFilter>::New();
  objectSurfaceExtractor->SetInputConnection(reader->GetOutputPort());
  objectSurfaceExtractor->SetValue(0, objectSegmentationThreshold);

  vtkSmartPointer<vtkWindowedSincPolyDataFilter> objectSurfaceSmoother=vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
  objectSurfaceSmoother->SetInputConnection(objectSurfaceExtractor->GetOutputPort()); 
  objectSurfaceSmoother->NormalizeCoordinatesOn();

  // Empirical values for organ atlas images
  objectSurfaceSmoother->SetPassBand(0.05);
  objectSurfaceSmoother->SetNumberOfIterations(80);


  vtkSmartPointer<vtkDecimatePro> decimator=vtkSmartPointer<vtkDecimatePro>::New();
  decimator->SetInputConnection(objectSurfaceSmoother->GetOutputPort());
  decimator->SetTargetReduction(0.9);

  vtkSmartPointer<vtkPolyDataNormals> objectSurfaceNormals=vtkSmartPointer<vtkPolyDataNormals>::New();
  objectSurfaceNormals->SetInputConnection(decimator->GetOutputPort());
  objectSurfaceNormals->SetFeatureAngle(60.0);

  if (!outputObjectSurfaceFilename.empty())
  {
    vtkSmartPointer<vtkSTLWriter> writer=vtkSmartPointer<vtkSTLWriter>::New();
    writer->SetInputConnection(objectSurfaceNormals->GetOutputPort());
    writer->SetFileName(outputObjectSurfaceFilename.c_str());
    writer->Update();
  }

  vtkSmartPointer<vtkSphereSource> supportVolume=vtkSmartPointer<vtkSphereSource>::New();
  supportVolume->SetCenter(&(supportPosition[0]));
  supportVolume->SetRadius(supportRadius);
  supportVolume->SetPhiResolution(30);
  supportVolume->SetThetaResolution(30);

  vtkSmartPointer<vtkPolyDataNormals> supportVolumeNormals=vtkSmartPointer<vtkPolyDataNormals>::New();
  supportVolumeNormals->SetInputConnection(supportVolume->GetOutputPort());
  supportVolumeNormals->SetFeatureAngle(60.0);
  supportVolumeNormals->Update();

  if (!outputSupportSurfaceFilename.empty())
  {
    vtkSmartPointer<vtkSTLWriter> writer=vtkSmartPointer<vtkSTLWriter>::New();
    writer->SetInput(supportVolumeNormals->GetOutput());
    writer->SetFileName(outputSupportSurfaceFilename.c_str());
    writer->Update();
  }  

  // Write combined mesh
  if (!outputCombinedSurfaceMeshFilename.empty())
  {
    cout << "Writing combined mesh to " << outputCombinedSurfaceMeshFilename << std::endl;
    vtkPolyData *organPoly=vtkPolyData::SafeDownCast(objectSurfaceNormals->GetOutput());
    vtkPolyData *supportPoly=vtkPolyData::SafeDownCast(supportVolumeNormals->GetOutput());

    vtkSmartPointer<vtkSMeshWriter> writer=vtkSmartPointer<vtkSMeshWriter>::New();
    writer->SetFileName(outputCombinedSurfaceMeshFilename.c_str());

    vtkSmartPointer<vtkAppendPolyData> polyAppender=vtkSmartPointer<vtkAppendPolyData>::New();

    SetMaterial(organPoly, DefRegEvalGlobal::OrganMaterialId);
    polyAppender->AddInput( organPoly ); 
    // need to specify a position within the organ, we assuem that the center of the support
    // is within the organ
    // :TODO: compute point from the organPoly
    double organPoint[3]={supportPosition[0],supportPosition[1],supportPosition[2]};
    writer->AddRegion("organ", organPoint, DefRegEvalGlobal::OrganMaterialId);

    SetMaterial(supportPoly, DefRegEvalGlobal::SupportMaterialId);
    polyAppender->AddInput( supportPoly ); 
    // need to specify a position within support region (not within the organ)
    double supportPoint[3]={supportPosition[0],supportPosition[1]+supportRadius*0.95,supportPosition[2]};
    writer->AddRegion("support", supportPoint, DefRegEvalGlobal::SupportMaterialId);

    writer->SetInputConnection(polyAppender->GetOutputPort());      
    writer->Update();
    writer->Write();
  }

  if (!outputObjectImageFilename.empty())
  {
    // Make sure that we have a clean triangle polydata
    vtkSmartPointer<vtkTriangleFilter> triangle=vtkSmartPointer<vtkTriangleFilter>::New();
    triangle->SetInputConnection(objectSurfaceNormals->GetOutputPort());
    triangle->Update();

    // Convert to triangle strip
    vtkSmartPointer<vtkStripper> stripper=vtkSmartPointer<vtkStripper>::New();
    stripper->SetInputConnection(triangle->GetOutputPort());
    stripper->Update();

    // blank reference image
    reader->Update();
    vtkSmartPointer<vtkImageData> refImg=vtkSmartPointer<vtkImageData>::New();
    refImg->DeepCopy(vtkImageData::SafeDownCast(reader->GetOutput()));
    // fill the image with zeros
    vtkIdType outIncX, outIncY, outIncZ;
    int *outExt=refImg->GetExtent();
    refImg->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);  
    int rowLength = (outExt[1] - outExt[0]+1)*refImg->GetNumberOfScalarComponents()*refImg->GetScalarSize();
    outIncY = outIncY*refImg->GetScalarSize() + rowLength;
    outIncZ *= refImg->GetScalarSize();
    char *outPtr = static_cast<char *>(refImg->GetScalarPointerForExtent(outExt));
    for (int z = outExt[4]; z <= outExt[5]; z++)
    {
      for (int y = outExt[2]; y<=outExt[3]; y++)
      {
        memset(outPtr,0,rowLength);
        outPtr += outIncY;
      }
      outPtr += outIncZ;
    }

    // Note: image orientation is not taken into account
    // Convert polydata to stencil
    vtkSmartPointer<vtkPolyDataToImageStencil> polyToImage=vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    polyToImage->SetInputConnection(stripper->GetOutputPort());
    polyToImage->SetOutputSpacing(refImg->GetSpacing());
    polyToImage->SetOutputOrigin(refImg->GetOrigin());
    polyToImage->SetOutputWholeExtent(refImg->GetExtent());
    polyToImage->Update();  

    // Convert stencil to image
    vtkSmartPointer<vtkImageStencil> stencil=vtkSmartPointer<vtkImageStencil>::New();
    stencil->SetInput(refImg);
    stencil->SetStencil(polyToImage->GetOutput());
    stencil->ReverseStencilOn();
    stencil->SetBackgroundValue(DefRegEvalGlobal::OrganIntensity);
    stencil->Update();

    // Write to file
    vtkSmartPointer<vtkMetaImageWriter> outputImageWriter=vtkSmartPointer<vtkMetaImageWriter>::New();
    outputImageWriter->SetFileName(outputObjectImageFilename.c_str()); 
    outputImageWriter->SetInputConnection(stencil->GetOutputPort());
    outputImageWriter->Write();
  }

  return EXIT_SUCCESS;
}
