#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>

#ifdef _WINDOWS
#include <getoptwin32.h>
#else
#include <getopt.h>
#endif

#include "itkQuadEdgeMesh.h"
#include "itkLaplaceBeltramiFilter.h"
#include "itkQuadEdgeMeshVTKPolyDataReader.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"

void showUsage()
{
  printf("USAGE: LaplaceBeltrami [OPTIONS] <vtk_mesh_file> <first_harmonic_surface>\n");
  printf("\t\t-h --help : print this message\n");
  printf("\t\t-e --eigenvalueCount : number of principal eigenvalues to calculate\n");
  exit(1);
}


int main(int argc, char *argv[])
{
    unsigned int eCount = 0;

    int c = 0;
    while(c != EOF)
    {
        static struct option long_options[] =
        {
            {"help",            no_argument, NULL, 'h'},
            {"eigenvalueCount", no_argument, NULL, 'e'},
            {                0,           0,    0,   0},
        };

        int option_index = 0;

        switch((c = 
            getopt_long(argc, argv, "he:",
                        long_options, &option_index)))
        {
            case 'h':
                showUsage();
                break;
            case 'e':
                eCount = atoi(optarg);
                break;
            default:
                if (c > 0)
                    showUsage();
                break;
        }
    }

// Required arguments

#define ARG_COUNT 3
    if((argc - optind + 1) != ARG_COUNT)
    {
        showUsage();
        exit(1);
    }

    // Here we recover the file names from the command line arguments
    const char* inFile = argv[optind];
    const char* firstHarmonicOutFile = argv[optind + 1];

    typedef float              PixelType;
    typedef double             PointDataType;
    typedef double             DDataType;
    typedef double             CoordRep;
    typedef double             InterpRep;
    const   unsigned int       Dimension = 3;

    // Declare the type of the input and output mesh
    typedef itk::QuadEdgeMeshTraits<PixelType, Dimension, PointDataType,
        DDataType, CoordRep, InterpRep> MeshTraits;
    typedef itk::QuadEdgeMesh<float,Dimension,MeshTraits> InMeshType;
    typedef itk::QuadEdgeMesh<double,Dimension,MeshTraits> OutMeshType;

    //  We can now instantiate the types of the reader.
    typedef itk::QuadEdgeMeshVTKPolyDataReader< InMeshType >  ReaderType;

    // create readers
    ReaderType::Pointer meshReader = ReaderType::New();

    //  The name of the file to be read or written is passed with the
    //  SetFileName() method.
    meshReader->SetFileName( inFile  );
    
    try
        {
        meshReader->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        std::cerr << "Error during Update() " << std::endl;
        std::cerr << excp << std::endl;
        }

    // get the objects
    InMeshType::Pointer mesh = meshReader->GetOutput();

    std::cout << "Vertex Count:  " << 
        mesh->GetNumberOfPoints() << std::endl;
    std::cout << "Cell Count:  " << 
        mesh->GetNumberOfCells() << std::endl;

    typedef itk::LaplaceBeltramiFilter< InMeshType, OutMeshType >
                                    LbFilterType;
    LbFilterType::Pointer lbFilter = LbFilterType::New();

    lbFilter->SetInput(mesh);
    lbFilter->SetEigenValueCount(eCount);
    lbFilter->Update();

    OutMeshType::Pointer outMesh = lbFilter->GetOutput();

    //  We can now instantiate the types of the write.
    typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< OutMeshType >
            WriterType;

    // create readers
    WriterType::Pointer meshWriter = WriterType::New();

    // write out first harmonic
    meshWriter->SetInput(outMesh);
    meshWriter->SetFileName(firstHarmonicOutFile);
    meshWriter->Update();

    std::string outfile;
    std::string outfileBase("SurfaceHarmonic");
#define VTKPDEXT ".vtk"

    for (unsigned int i = 0; i < eCount; i++)
      {   
      if (lbFilter->SetSurfaceHarmonic(i))
        {
        WriterType::Pointer harmonicWriter = WriterType::New();
        outMesh = lbFilter->GetOutput();
        harmonicWriter->SetInput(outMesh);
        char countStr[8];
        sprintf(countStr, "_%d", i);
        outfile = outfileBase + countStr + VTKPDEXT;
        harmonicWriter->SetFileName(outfile.c_str());
        harmonicWriter->Update();
        }
      else
        std::cerr << "Couldn't get harmonic #" << eCount << std::endl;

      }
}
