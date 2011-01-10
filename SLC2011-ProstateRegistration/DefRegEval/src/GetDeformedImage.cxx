#include "DefRegEval.h"

#include "vtkSmartPointer.h"
#include "vtkXMLUnstructuredGridWriter.h" 
#include "vtkUnstructuredGridWriter.h"
#include "vtkMath.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkImageStencil.h"
#include "vtkTriangleFilter.h"
#include "vtkStripper.h"
#include "vtkSmartPointer.h"
#include "vtkXMLUnstructuredGridWriter.h"
#include "vtkProbeFilter.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkExtractCells.h"
#include "itkWarpImageFilter.h"
#include "itkVTKImageToImageFilter.h"

#include "IO/vtkLsDynaBinaryPlotReader.h"

void ExtractOrganData(vtkUnstructuredGrid* organGrid, vtkUnstructuredGrid* ugrid, vtkLsDynaBinaryPlotReader *plotReader)
{
  vtkPoints *points = ugrid->GetPoints();
  //organGrid->SetPoints(points); // copy the point data
  //organGrid->Allocate();
  vtkCellData* cd=ugrid->GetCellData();
  vtkIntArray* materialArray=vtkIntArray::SafeDownCast(cd->GetArray(plotReader->GetArrayNameMaterial()));

  // copy only organ material cells
  vtkCellArray *cells = ugrid->GetCells();
  cells->InitTraversal();  
  vtkIdType npts, *pts;
  int cell=-1;
  vtkSmartPointer<vtkIdList> organCells=vtkSmartPointer<vtkIdList>::New();    
  while (cells->GetNextCell(npts, pts))
  {
    cell++;
    if (npts != 4 || materialArray->GetValue(cell)!=1)
    {
      // only extract tetrahedra, skip non-organ material
      continue;
    }
    //organGrid->InsertNextCell(VTK_TETRA, npts, pts);      
    organCells->InsertNextId(cell);      
  }

  // Add only dislocation point data field
  //organGrid->GetPointData()->AddArray(ugrid->GetPointData()->GetArray(plotReader->GetArrayNameDisplacement()));

  vtkSmartPointer<vtkExtractCells> cellExtractor=vtkSmartPointer<vtkExtractCells>::New();    
  cellExtractor->SetInput(ugrid);
  cellExtractor->SetCellList(organCells);
  cellExtractor->Update();
  organGrid->ShallowCopy(cellExtractor->GetOutput());

}

int main(int argc, char *argv[])
{
  std::string inputModelFilename = "femmodel.plt";
  std::string referenceImageFilename = "shape-image.mha";
  std::string outputImageFilename = "shape-image-deformed.mha";
  std::string outputFullImageFilename;
  std::string outputFullDeformationFieldImageFilename;
  std::string outputVolumeMeshFilename;
  std::string outputDeformationFieldImageFilename;
  int solverTimeStep=-1;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--inputModel", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputModelFilename, "Input FEM result LS-DYNA plot file");
  args.AddArgument("--referenceImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &referenceImageFilename, "Reference image, the output image will have the same origin, spacing and orientation");
  args.AddArgument("--outputImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageFilename, "Output deformed image corresponding to the specified solver timestep");
  args.AddArgument("--outputVolumeMesh", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeMeshFilename, "Output deformed grid (vtu) corresponding to the specified solver timestep. (optional)"); 
  args.AddArgument("--outputDeformationFieldImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputDeformationFieldImageFilename, "Deformation field for the organ region, sampled in the reference image space"); 
  args.AddArgument("--outputFullImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFullImageFilename, "Full image transformed by the deformation field"); 
  args.AddArgument("--outputFullDeformationFieldImage", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFullDeformationFieldImageFilename, "Deformation field for the full image, sampled in the reference image space"); 
  args.AddArgument("--solverTimeStep", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &solverTimeStep, "The plot file usually contains deformations for several timesteps. Solver timestep defines which one to extract (optional, default is the last timestep)."); 

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  /////////////

  vtkSmartPointer<vtkLsDynaBinaryPlotReader> plotReader=vtkSmartPointer<vtkLsDynaBinaryPlotReader>::New();
  plotReader->SetFileName(inputModelFilename.c_str());  
  plotReader->SetRequestedTimeStep(solverTimeStep);

  if (!outputVolumeMeshFilename.empty())
  {
    // write *.vtu file
    // keep the original point coordinates (deformation is in the point data)
    plotReader->ApplyDeformationToPointsOff();
    plotReader->Update();

    vtkSmartPointer<vtkUnstructuredGrid> organGrid=vtkSmartPointer<vtkUnstructuredGrid>::New();
    ExtractOrganData(organGrid, plotReader->GetOutput(), plotReader);

    vtkSmartPointer<vtkXMLUnstructuredGridWriter> xmlGridWriter=vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
    xmlGridWriter->SetInput(organGrid);  
    xmlGridWriter->SetFileName(outputVolumeMeshFilename.c_str());    
    xmlGridWriter->Update();
  }

  plotReader->ApplyDeformationToPointsOn();
  plotReader->Update();
  vtkUnstructuredGrid* ugrid=vtkUnstructuredGrid::SafeDownCast(plotReader->GetOutput());
  if (ugrid==NULL)
  {
    std::cerr<<"Unstructured grid data info not found";
    exit(EXIT_FAILURE);
  }
  ugrid->Update();

  // Copy the unstructured grid, but only the organ cells and the dislocation vector
  vtkSmartPointer<vtkUnstructuredGrid> organGrid=vtkSmartPointer<vtkUnstructuredGrid>::New();
  ExtractOrganData(organGrid, ugrid, plotReader);

  // Extract all the triangles from the polyhedra elements
  vtkSmartPointer<vtkPolyData> surface=vtkSmartPointer<vtkPolyData>::New();

  vtkPoints *points = organGrid->GetPoints();
  surface->SetPoints(points); // copy the point data
  surface->Allocate();

  vtkCellArray *cells = organGrid->GetCells();
  cells->InitTraversal();  
  vtkIdType tri1[3];
  vtkIdType tri2[3];
  vtkIdType tri3[3];
  vtkIdType tri4[3];
  vtkIdType npts, *pts;
  int cell=-1;
  while (cells->GetNextCell(npts, pts))
  {
    cell++;
    if (npts != 4 /*|| materialArray->GetValue(cell)!=1*/)
    {
      // only extract tetrahedra
      // only extract material 1 (organ)
      continue;
    }
    // Ordering: counter-clockwise (http://www.vtk.org/VTK/img/file-formats.pdf)
    tri1[0]=pts[0]; tri1[1]=pts[2]; tri1[2]=pts[1];
    tri2[0]=pts[0]; tri2[1]=pts[1]; tri2[2]=pts[3];
    tri3[0]=pts[0]; tri3[1]=pts[3]; tri3[2]=pts[2];
    tri4[0]=pts[1]; tri4[1]=pts[2]; tri4[2]=pts[3];
    surface->InsertNextCell(VTK_TRIANGLE, 3, tri1);
    surface->InsertNextCell(VTK_TRIANGLE, 3, tri2);
    surface->InsertNextCell(VTK_TRIANGLE, 3, tri3);
    surface->InsertNextCell(VTK_TRIANGLE, 3, tri4);
  }

  // Make sure that we have a clean triangle polydata
  vtkSmartPointer<vtkTriangleFilter> triangle=vtkSmartPointer<vtkTriangleFilter>::New();
  triangle->SetInput(surface);
  triangle->Update();

  // Convert to triangle strip
  vtkSmartPointer<vtkStripper> stripper=vtkSmartPointer<vtkStripper>::New();
  stripper->SetInputConnection(triangle->GetOutputPort());
  stripper->Update();

  // Get output image parameters
  // Note: image orientation is not taken into account
  vtkSmartPointer<vtkMetaImageReader> refImageReader=vtkSmartPointer<vtkMetaImageReader>::New();
  refImageReader->SetFileName(referenceImageFilename.c_str()); 
  refImageReader->Update();
  // copy the reference image
  vtkSmartPointer<vtkImageData> refImg=vtkSmartPointer<vtkImageData>::New();
  refImg->DeepCopy(vtkImageData::SafeDownCast(refImageReader->GetOutput()));  
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
  outputImageWriter->SetFileName(outputImageFilename.c_str()); 
  outputImageWriter->SetInputConnection(stencil->GetOutputPort());
  outputImageWriter->Write();

  if (!outputDeformationFieldImageFilename.empty())
  {
    // write a 3-component image file with the deformation data
    // keep the original point coordinates (deformation is in the point data)
    plotReader->ApplyDeformationToPointsOff();
    plotReader->Update();
    vtkUnstructuredGrid* ugrid=vtkUnstructuredGrid::SafeDownCast(plotReader->GetOutput());

    // Copy the unstructured grid, but only the organ cells and the dislocation vector
    vtkSmartPointer<vtkUnstructuredGrid> organGrid=vtkSmartPointer<vtkUnstructuredGrid>::New();
    ExtractOrganData(organGrid, ugrid, plotReader);

    //remove all cell data
    vtkCellData* cd=organGrid->GetCellData();
    while (cd->GetNumberOfArrays()>0)
    {
      cd->RemoveArray(cd->GetArrayName(0));
    }

    // keep only displacement point data
    vtkPointData* pd=organGrid->GetPointData();
    while(pd->GetNumberOfArrays()>1)
    {
      if (strcmp(pd->GetArrayName(0),plotReader->GetArrayNameDisplacement())!=0)
      {        
        pd->RemoveArray(pd->GetArrayName(0));
      }
      if (strcmp(pd->GetArrayName(1),plotReader->GetArrayNameDisplacement())!=0)
      {        
        pd->RemoveArray(pd->GetArrayName(1));
      }
    }

    pd->SetActiveScalars(plotReader->GetArrayNameDisplacement());
    refImg->SetNumberOfScalarComponents(3);
    refImg->SetScalarTypeToFloat();
    refImg->AllocateScalars();

    //fill the ref image with zeros
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

    // Create a smaller image that contains only the region where there is no ugrid data.
    // Outside this region the displacement will be all zero, so it would be a waste of time to try to go
    // through those voxels (it can take about 1-2 minutes for a 256x256x26 image).
    vtkSmartPointer<vtkImageData> nonZeroImg=vtkSmartPointer<vtkImageData>::New();
    nonZeroImg->DeepCopy(refImg);
    double *gridExtent=organGrid->GetBounds();
    int *refImgExtent=refImg->GetExtent();
    int nonZeroImgExtent[6];
    for (int i=0; i<3; i++)
    {
      nonZeroImgExtent[i*2]=(gridExtent[i*2]-refImg->GetOrigin()[i])/refImg->GetSpacing()[i];
      if (nonZeroImgExtent[i*2]<refImgExtent[i*2])
      {
        nonZeroImgExtent[i*2]=refImgExtent[i*2];
      }
      nonZeroImgExtent[i*2+1]=(gridExtent[i*2+1]-refImg->GetOrigin()[i])/refImg->GetSpacing()[i];
      if (nonZeroImgExtent[i*2+1]>refImgExtent[i*2+1])
      {
        nonZeroImgExtent[i*2+1]=refImgExtent[i*2+1];
      }
    }    
    nonZeroImg->SetExtent(nonZeroImgExtent);

    vtkSmartPointer<vtkProbeFilter> probeFilter=vtkSmartPointer<vtkProbeFilter>::New();
    probeFilter->SetInput(nonZeroImg);
    probeFilter->SetSource(organGrid);  
    probeFilter->Update();

    refImg->CopyAndCastFrom(vtkImageData::SafeDownCast(probeFilter->GetOutput()), nonZeroImgExtent);

    vtkSmartPointer<vtkMetaImageWriter> defFieldWriter=vtkSmartPointer<vtkMetaImageWriter>::New();
    defFieldWriter->SetFileName(outputDeformationFieldImageFilename.c_str()); 
    defFieldWriter->SetInput(refImg);
    defFieldWriter->Write();
  }

  if (!outputFullImageFilename.empty())
  {
    // write a full image, by applying the deformation field computed by FEM 

    plotReader->ApplyDeformationToPointsOn(); // deformation is defined in the output image space
    plotReader->Update();
    vtkUnstructuredGrid* ugrid=vtkUnstructuredGrid::SafeDownCast(plotReader->GetOutput());

    //remove all cell data
    vtkCellData* cd=ugrid->GetCellData();
    while (cd->GetNumberOfArrays()>0)
    {
      cd->RemoveArray(cd->GetArrayName(0));
    }

    // keep only displacement point data
    vtkPointData* pd=ugrid->GetPointData();
    while(pd->GetNumberOfArrays()>1)
    {
      if (strcmp(pd->GetArrayName(0),plotReader->GetArrayNameDisplacement())!=0)
      {        
        pd->RemoveArray(pd->GetArrayName(0));
      }
      if (strcmp(pd->GetArrayName(1),plotReader->GetArrayNameDisplacement())!=0)
      {        
        pd->RemoveArray(pd->GetArrayName(1));
      }
    }

    pd->SetActiveScalars(plotReader->GetArrayNameDisplacement());
    refImg->SetNumberOfScalarComponents(3);
    refImg->SetScalarTypeToFloat();
    refImg->AllocateScalars();

    //fill the ref image with zeros
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

    vtkSmartPointer<vtkProbeFilter> probeFilter=vtkSmartPointer<vtkProbeFilter>::New();
    probeFilter->SetInput(refImg);
    probeFilter->SetSource(ugrid);  
    probeFilter->Update();

    // probeFilter output is the deformation field        

    typedef float  PixelType;
    static const unsigned int  Dimension = 3;
    typedef itk::Image <PixelType, Dimension> InternalImageType;

    typedef itk::Vector< float, Dimension >  VectorType;
    typedef itk::Image< VectorType, Dimension >  DeformationFieldType;

    typedef itk::VTKImageToImageFilter< DeformationFieldType > vtkToItkType;
    vtkToItkType::Pointer itkDeformField = vtkToItkType::New();
    itkDeformField->SetInput( vtkImageData::SafeDownCast(probeFilter->GetOutput()) );
    itkDeformField->Update();

    if (!outputFullImageFilename.empty() || !outputFullDeformationFieldImageFilename.empty())
    {
      // Warp image filter uses inverse mapping, so we need to invert the displacement vector direction
      // (the vector points to the original voxel position)
      DeformationFieldType::Pointer invDeformationField = DeformationFieldType::New();
      invDeformationField->SetRegions( itkDeformField->GetOutput()->GetBufferedRegion() );
      invDeformationField->SetOrigin( itkDeformField->GetOutput()->GetOrigin() );
      invDeformationField->SetSpacing( itkDeformField->GetOutput()->GetSpacing() );
      invDeformationField->SetDirection( itkDeformField->GetOutput()->GetDirection() );
      invDeformationField->Allocate();
      typedef itk::ImageRegionIterator< DeformationFieldType > FieldIterator;
      typedef itk::ImageRegionIterator< const DeformationFieldType > ConstFieldIterator;
      FieldIterator fi_dst( invDeformationField, invDeformationField->GetBufferedRegion() );
      ConstFieldIterator fi_src( itkDeformField->GetOutput(), itkDeformField->GetOutput()->GetBufferedRegion() );
      VectorType displacement;
      fi_src.GoToBegin();
      fi_dst.GoToBegin();
      while( ! fi_dst.IsAtEnd() )
      {
        displacement=fi_src.Get();
        displacement[0]*=-1.0;
        displacement[1]*=-1.0;
        displacement[2]*=-1.0;
        fi_dst.Set( displacement );
        ++fi_src;
        ++fi_dst;
      }

      //create the filter that applies the deformation
      typedef itk::WarpImageFilter<InternalImageType, InternalImageType, DeformationFieldType>  WarpImageFilterType;
      WarpImageFilterType::Pointer warpFilter = WarpImageFilterType::New();

      //use linear interpolation method for float-valued outputs of the deformation
      typedef itk::LinearInterpolateImageFunction<InternalImageType, double>  InterpolatorType;
      InterpolatorType::Pointer interpolator = InterpolatorType::New();
      warpFilter->SetInterpolator(interpolator);

      warpFilter->SetOutputSpacing(invDeformationField->GetSpacing());
      warpFilter->SetOutputOrigin(invDeformationField->GetOrigin());
      warpFilter->SetDeformationField(invDeformationField);

      typedef itk::ImageFileReader< InternalImageType  > ImageReaderType;
      ImageReaderType::Pointer inputImageReader = ImageReaderType::New();
      inputImageReader->SetFileName( referenceImageFilename );
      try
      {
        inputImageReader->Update();
      }
      catch( itk::ExceptionObject & excp )
      {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
      }

      warpFilter->SetInput(inputImageReader->GetOutput());

      try
      {
        warpFilter->Update();
      }
      catch( itk::ExceptionObject & err ) 
      { 
        std::cerr << "ExceptionObject caught !" << std::endl; 
        std::cerr << err << std::endl; 
        exit(-1);
      }

      typedef   itk::ImageFileWriter< InternalImageType >  MovingWriterType;
      MovingWriterType::Pointer movingWriter = MovingWriterType::New();
      movingWriter->SetFileName( outputFullImageFilename );
      movingWriter->SetInput(warpFilter->GetOutput());

      try
      {
        movingWriter->Update();
      }
      catch( itk::ExceptionObject & excp )
      {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
      }
    }

    if (!outputFullDeformationFieldImageFilename.empty())
    {
      vtkSmartPointer<vtkMetaImageWriter> defFieldWriter=vtkSmartPointer<vtkMetaImageWriter>::New();
      defFieldWriter->SetFileName(outputFullDeformationFieldImageFilename.c_str()); 
      defFieldWriter->SetInput(vtkImageData::SafeDownCast(probeFilter->GetOutput()));
      defFieldWriter->Write();
    }

  } 

  return EXIT_SUCCESS;
}
