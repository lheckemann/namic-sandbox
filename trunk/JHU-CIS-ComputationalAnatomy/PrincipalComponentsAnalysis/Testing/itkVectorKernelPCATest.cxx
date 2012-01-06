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
#include "itkMesh.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include "itkVectorFieldPCA.h"
//#include "itkGaussianKernelFunction.h"
#include "vnl/vnl_vector.h"

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

  typedef double             PointDataType;
  typedef itk::Array<PointDataType> PointDataVectorType;
  typedef PointDataVectorType PixelType;
  typedef double             DDataType;
  typedef double             CoordRep;
  typedef double             InterpRep;
  const   unsigned int       Dimension = 3;

//    typedef float              PCAResultsType;
  typedef double             PCAResultsType;

  // Declare the type of the input mesh
  typedef itk::QuadEdgeMeshTraits<PixelType, Dimension, PointDataVectorType,
      DDataType, CoordRep, InterpRep> MeshTraits;
//  typedef itk::QuadEdgeMesh<PixelType,Dimension,MeshTraits> InMeshType;
//  typedef itk::QuadEdgeMesh<PixelType,Dimension> InMeshType;
  typedef itk::Mesh<PixelType,Dimension> InMeshType;

  // Declare the type of the kernel function class
  // typedef itk::GaussianDistanceKernel KernelType;
  typedef itk::GaussianDistanceKernel<CoordRep> KernelType;

  // Declare the type of the PCA calculator
  typedef itk::VectorFieldPCA< PointDataType, PCAResultsType, 
                                PixelType, CoordRep, KernelType, InMeshType > PCACalculatorType;
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

  //  We can now instantiate the types of the reader/writer.
  typedef itk::MeshFileReader< InMeshType >  ReaderType;
  typedef itk::MeshFileWriter< InMeshType >  WriterType;

  // create readers/writers
  ReaderType::Pointer meshReader = ReaderType::New();
  WriterType::Pointer meshWriter = WriterType::New();

  //  The name of the file to be read or written is passed with the
  //  SetFileName() method.
  meshReader->SetFileName( inMeshFile  );

  try
    {
    meshReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    vcl_cerr << "Error reading mesh file " << inMeshFile << vcl_endl;
    vcl_cerr << excp << vcl_endl;
    }

  const char* vectorFieldName = argv[optind];
  PCACalculatorType::VectorFieldType vectorField;
  std::ifstream vectFieldFile(vectorFieldName);
  if (!vectFieldFile)
  {
    vcl_cerr << "Couldn't open in vector field file "  << vectorFieldName << vcl_endl;
    exit(1);
  }

  if (!vectorField.read_ascii(vectFieldFile))
  {
    vcl_cerr << "Couldn't read in vector field file "  << vectorFieldName << vcl_endl;
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
      vcl_cerr << "Couldn't open in vector field file "  << vectorFieldName << vcl_endl;
      exit(1);
      }
    if (!vectorField.read_ascii(vectFieldFile))
      {
      vcl_cerr << "Couldn't read in vector field file "  << vectorFieldName << vcl_endl;
      exit(1);
      }
    if (vectorFieldDim != vectorField.cols() ||
                              vectorFieldCount != vectorField.rows())
      {
      vcl_cerr << "Unexpected dimensions in vector field file "  << vectorFieldName << vcl_endl;
      vcl_cerr << "\tExpected "  << vectorFieldCount << " x " << vectorFieldDim;
      vcl_cerr << "\t, got "  << vectorField.rows() << " x " << vectorField.cols() << vcl_endl;
      exit(1);
      }
      vectorFieldSet->SetElement(i, vectorField);
      vectFieldFile.close();
    }

  // get the objects
  InMeshType::Pointer mesh = meshReader->GetOutput();

  vcl_cout << "Vertex Count:  " << 
      mesh->GetNumberOfPoints() << vcl_endl;
  vcl_cout << "Cell Count:  " << 
      mesh->GetNumberOfCells() << vcl_endl;

  PCACalculatorType::Pointer pcaCalc = PCACalculatorType::New();

  vcl_cout << "Name of Class = " << pcaCalc->GetNameOfClass() << vcl_endl;

  vcl_cout << "Test Print() = " << vcl_endl;
  pcaCalc->Print( vcl_cout );

  // set user variables
  KernelType::Pointer distKernel =
                                KernelType::New();
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
    vcl_cerr << excp << vcl_endl;
    return EXIT_FAILURE;
    }

  char fName[1024];
  std::ofstream debugOut;
  debugOut.precision(15);

    // get the output and write to files
  sprintf(fName, "%s_BasisVectors.txt", outFileNameBase);
  debugOut.open(fName);
  for (unsigned int dbgk = 0; dbgk < pcaCalc->GetComponentCount(); dbgk++)
    (pcaCalc->GetBasisVectors()->GetElement(dbgk)).print(debugOut);

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

  PCACalculatorType::MatrixType averages = pcaCalc->GetAveVectorField();

  PixelType oneAverage;
  for (unsigned int k = 0; k < averages.rows(); k++)
    {
    oneAverage = averages.get_row(k);
    mesh->SetPointData(k, oneAverage);
    }

  //  The name of the file to be read or written is passed with the
  //  SetFileName() method.
  char outFileName[2048];
  vcl_strcpy(outFileName, inMeshFile);
  char* extension = vcl_strrchr(outFileName, '.');
  if (extension)
    {
    vcl_strcpy(extension, "Ave");
    vcl_strcat(outFileName, ".vtk");
    }
  else
    {
    vcl_strcat(outFileName, "Ave.vtk");
    }

  meshWriter->SetFileName(outFileName);
  meshWriter->SetInput(mesh);

  try
    {
    meshWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    vcl_cerr << excp << vcl_endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
