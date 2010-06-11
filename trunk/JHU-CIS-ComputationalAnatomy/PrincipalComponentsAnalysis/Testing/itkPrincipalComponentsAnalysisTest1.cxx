/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef _WINDOWS
#include <getoptwin32.h>
#else
#include <getopt.h>
#endif

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshVTKPolyDataReader.h"
#include "itkVectorFieldPCA.h"

void showUsage(const char* programName)
{
  printf("USAGE: %s [OPTIONS] <vtk_mesh_file> <outputName> ", programName);
  printf("<vectorFieldSetFile>\n");
  printf("\t\t-h --help : print this message\n");
  printf("\t\t-c --pcaCount : number of principal components to calculate (def = 3)\n");
  printf("\t\t-s --kernelSigma : KernelSigma ( def = 6.25)\n");
  exit(EXIT_FAILURE);
}

int main( int argc, char *argv[] )
{
    double kernelSigma = 6.5;
    unsigned int pcaCount = 3;

    typedef float              PixelType;
    typedef double             PointDataType;
    typedef double             DDataType;
    typedef double             CoordRep;
    typedef double             InterpRep;
    const   unsigned int       Dimension = 3;

//    typedef float              PCAResultsType;
    typedef double             PCAResultsType;

    // Declare the type of the input and output mesh
    typedef itk::QuadEdgeMeshTraits<PixelType, Dimension, PointDataType,
        DDataType, CoordRep, InterpRep> MeshTraits;
    typedef itk::QuadEdgeMesh<float,Dimension,MeshTraits> InMeshType;

    typedef itk::GaussianDistanceKernel KernelType;
    typedef itk::VectorFieldPCA< PointDataType, PCAResultsType, 
                                 KernelType, PixelType, InMeshType > PCACalculatorType;
    int c = 0;
    while(c != EOF)
    {
        static struct option long_options[] =
        {
            {"help",              no_argument,       NULL, 'h'},
            {"pcaCount",          required_argument, NULL, 'c'},
            {"kernelSigma",       required_argument, NULL, 's'},
            {                  0,                 0,    0,   0},
        };

        int option_index = 0;

        switch((c = 
            getopt_long(argc, argv, "hc:s:",
                        long_options, &option_index)))
        {
            case 'h':
                showUsage(argv[0]);
                break;
            case 'c':
                pcaCount = atoi(optarg);
                break;
            case 's':
                kernelSigma = atof(optarg);
                break;
            default:
                if (c > 0)
                    showUsage(argv[0]);
                break;
        }
    }

// Required arguments

#define ARG_COUNT 4
    if((argc - optind + 1) < ARG_COUNT)
    {
      showUsage(argv[0]);
      exit(1);
    }

    // Here we recover the file names from the command line arguments
    const char* inMeshFile = argv[optind++];
    const char* outFileNameBase = argv[optind++];

    //  We can now instantiate the types of the reader.
    typedef itk::QuadEdgeMeshVTKPolyDataReader< InMeshType >  ReaderType;

    // create readers
    ReaderType::Pointer meshReader = ReaderType::New();

    //  The name of the file to be read or written is passed with the
    //  SetFileName() method.
    meshReader->SetFileName( inMeshFile  );
    
    try
      {
      meshReader->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Error reading mesh file " << inMeshFile << std::endl;
      std::cerr << excp << std::endl;
      }

    const char* vectorFieldName = argv[optind];
    PCACalculatorType::VectorFieldType vectorField;
    std::ifstream vectFieldFile(vectorFieldName);
    if (!vectFieldFile)
    {
      std::cerr << "Couldn't open in vector field file "  << vectorFieldName << std::endl;
      exit(1);
    }

    if (!vectorField.read_ascii(vectFieldFile))
    {
      std::cerr << "Couldn't read in vector field file "  << vectorFieldName << std::endl;
      exit(1);
    }
    // should know vector field dimensions now
    unsigned int vectorFieldDim = vectorField.cols();
    unsigned int vectorFieldCount = vectorField.rows();
    unsigned int fieldSetCount = argc - optind;

    PCACalculatorType::VectorFieldSetTypePointer vectorFieldSet =
        PCACalculatorType::VectorFieldSetType::New();

    vectorFieldSet->Reserve(fieldSetCount);

    vectorFieldSet->SetElement(0, vectorField);
    optind++;
    vectFieldFile.close();

    for (unsigned int i = 1; i < fieldSetCount; i++)
      {
      vectorFieldName = argv[optind++];
      vectFieldFile.open(vectorFieldName);
      if (!vectFieldFile)
        {
        std::cerr << "Couldn't open in vector field file "  << vectorFieldName << std::endl;
        exit(1);
        }
      if (!vectorField.read_ascii(vectFieldFile))
        {
        std::cerr << "Couldn't read in vector field file "  << vectorFieldName << std::endl;
        exit(1);
        }
      if (vectorFieldDim != vectorField.cols() ||
                                vectorFieldCount != vectorField.rows())
        {
        std::cerr << "Unexpected dimensions in vector field file "  << vectorFieldName << std::endl;
        std::cerr << "\tExpected "  << vectorFieldCount << " x " << vectorFieldDim;
        std::cerr << "\t, got "  << vectorField.rows() << " x " << vectorField.cols() << std::endl;
        exit(1);
        }
        vectorFieldSet->SetElement(i, vectorField);
        vectFieldFile.close();
      }

    // get the objects
    InMeshType::Pointer mesh = meshReader->GetOutput();

    std::cout << "Vertex Count:  " << 
        mesh->GetNumberOfPoints() << std::endl;
    std::cout << "Cell Count:  " << 
        mesh->GetNumberOfCells() << std::endl;

  PCACalculatorType::Pointer pcaCalc = PCACalculatorType::New();

  std::cout << "Name of Class = " << pcaCalc->GetNameOfClass() << std::endl;

  std::cout << "Test Print() = " << std::endl;
  pcaCalc->Print( std::cout );

  // set user variables
  KernelType::Pointer distKernel =
                                itk::GaussianDistanceKernel::New();
  distKernel->SetKernelSigma( kernelSigma );
  pcaCalc->SetKernelFunction( distKernel );

  pcaCalc->SetComponentCount( pcaCount );

  //
  //  Now connect the input and verify that it runs fine.
  // 
  pcaCalc->SetPointSet( mesh );
  pcaCalc->SetVectorFieldSet( vectorFieldSet );

  // set vector fields

  try
    {
    pcaCalc->Compute();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

char fName[1024];
std::ofstream debugOut;
debugOut.precision(15);

  // get the output and write to files
sprintf(fName, "%s_BasisVectors.txt", outFileNameBase);
debugOut.open(fName);
for (unsigned int dbgk = 0; dbgk < pcaCalc->GetComponentCount(); dbgk++)
{
(pcaCalc->GetBasisVectors()->GetElement(dbgk)).print(debugOut);
}
debugOut.close();

sprintf(fName, "%s_PCAEigenValues.txt", outFileNameBase);
debugOut.open(fName);
debugOut << pcaCalc->GetPCAEigenValues();
debugOut.close();

sprintf(fName, "%s_Res.txt", outFileNameBase);
debugOut.open(fName);
pcaCalc->GetProjection().print(debugOut);
debugOut.close();

sprintf(fName, "%s_AveVectorField.txt", outFileNameBase);
debugOut.open(fName);
pcaCalc->GetAveVectorField().print(debugOut);
debugOut.close();

  return EXIT_SUCCESS;
}
