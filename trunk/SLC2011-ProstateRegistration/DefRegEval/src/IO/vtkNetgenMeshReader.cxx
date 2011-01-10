/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkNetgenMeshReader.cxx,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkNetgenMeshReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkErrorCode.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCharArray.h"
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkIdList.h"

vtkCxxRevisionMacro(vtkNetgenMeshReader, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkNetgenMeshReader);

static const int SUPPORTED_DIMENSION=3;
static const int MAX_LINE_LENGTH=2048;

//----------------------------------------------------------------------------
vtkNetgenMeshReader::vtkNetgenMeshReader()
{
  this->FileName = NULL;

  this->SetNumberOfInputPorts(0);

  this->ArrayNameMaterial=NULL;
  this->SetArrayNameMaterial("material");

  this->ArrayNameOnSurface=NULL;
  this->SetArrayNameOnSurface("on_surface");

}

//----------------------------------------------------------------------------
vtkNetgenMeshReader::~vtkNetgenMeshReader()
{
  SetFileName(NULL);
}

//----------------------------------------------------------------------------
int vtkNetgenMeshReader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDebugMacro( << "Reading LS-DYNA binary plot file");

  ifstream inputFile(this->FileName, ios::in);

  if (!ReadHeader(inputFile))
  {
    vtkErrorMacro("Cannot read file: invalid file header");
    this->SetErrorCode(vtkErrorCode::UnrecognizedFileTypeError);
    return 0;
  }

  vtkSmartPointer<vtkIdList> surfacePointIds=vtkSmartPointer<vtkIdList>::New();

  char lineBuffer[MAX_LINE_LENGTH+1];
  lineBuffer[MAX_LINE_LENGTH]='\0'; // make sure that the buffer is always zero terminated  
  while (!inputFile.eof())
  {
    inputFile.getline(lineBuffer, MAX_LINE_LENGTH);
    if (lineBuffer[0]==0)
    {
      // the string is empty
      continue;
    }    
    if (lineBuffer[0]=='#')
    {
      //comment line
      continue;
    }
    if (strncmp(lineBuffer,"surfaceelementsgi", MAX_LINE_LENGTH)==0)
    {
      ReadSurfaceElements(inputFile, surfacePointIds);
    }
    if (strncmp(lineBuffer,"volumeelements", MAX_LINE_LENGTH)==0)
    {
      ReadVolumeElements(inputFile, output);
    }
    if (strncmp(lineBuffer,"points", MAX_LINE_LENGTH)==0)
    {
      ReadPoints(inputFile, output);
    }
  }

  // Add "on surface" property to the points (=1 if a surface point, =0 otherwise)
  if (surfacePointIds->GetNumberOfIds()>0)
  {
    int numberOfPoints=output->GetNumberOfPoints();
    vtkSmartPointer<vtkCharArray> isSurfacePointArray=vtkSmartPointer<vtkCharArray>::New();
    isSurfacePointArray->SetName(this->ArrayNameOnSurface);
    isSurfacePointArray->SetNumberOfComponents(1);  
    isSurfacePointArray->SetNumberOfTuples(numberOfPoints);  
    // Clear surface property array
    for (int i=0; i<numberOfPoints; i++)
    {
      isSurfacePointArray->SetValue(i, 0);
    }
    // Set 1 in surface property array for each surface element
    int numberOfSurfaceElements=surfacePointIds->GetNumberOfIds();
    for (int i=0; i<numberOfSurfaceElements; i++)
    {
      isSurfacePointArray->SetValue(surfacePointIds->GetId(i), 1);
    }  
    output->GetPointData()->AddArray(isSurfacePointArray);
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkNetgenMeshReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "File Name: " 
    << (this->FileName ? this->FileName : "(none)") << "\n";

}

//----------------------------------------------------------------------------
int vtkNetgenMeshReader::RequestInformation(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *vtkNotUsed(outputVector))
{
  if ( !this->FileName )
  {
    vtkErrorMacro("No filename specified");
    this->SetErrorCode(vtkErrorCode::NoFileNameError);
    return 0;
  }  

  vtkDebugMacro( << "Reading LS-DYNA binary plot file");

  ifstream inputFile(this->FileName, ios::in);

  if (inputFile.fail())
    {
    this->SetErrorCode(vtkErrorCode::FileNotFoundError);
    vtkErrorMacro("Specified filename not found");
    return 0;
    }

  if (!ReadHeader(inputFile))
  {
    vtkErrorMacro("Cannot read file: invalid file header");
    this->SetErrorCode(vtkErrorCode::UnrecognizedFileTypeError);
    return 0;
  }

  

  return 1;
}

bool vtkNetgenMeshReader::ReadHeader(ifstream &inputFile)
{
  vtkstd::string fileType;
  inputFile >> fileType;
  if (fileType.compare("mesh3d")!=0)
  {
    // invalid header (should start with mesh3d)
    return 0;
  }

  vtkstd::string dimLabel;
  inputFile >> dimLabel;
  if (dimLabel.compare("dimension")!=0)
  {
    // invalid header
    return 0;
  }

  int dimension=0;
  inputFile >> dimension;
  if (dimension!=SUPPORTED_DIMENSION)
  {
    vtkErrorMacro("File dimension must be "<<SUPPORTED_DIMENSION);
    this->SetErrorCode(vtkErrorCode::FileFormatError);
    return 0;
  }
  
  return 1;
}

bool vtkNetgenMeshReader::ReadSurfaceElements(ifstream &inputFile, vtkIdList *surfacePointIds)
{ 
  surfacePointIds->SetNumberOfIds(0);

  int numberOfSurfaceElements=0;
  inputFile >> numberOfSurfaceElements;
  
  if (numberOfSurfaceElements<1)
  {
    vtkWarningMacro("No surface elements");    
    return false;
  }

  int surfnr=0; // surface number
  int bcnr=0; // boundary condition number
  int domin;
  int domout;
  int numberOfPoints;
  int elem=0;
  int pointId=0;
  for (elem=0; elem<numberOfSurfaceElements || inputFile.eof(); elem++)
  {    
    inputFile >> surfnr;
    inputFile >> bcnr;
    inputFile >> domin;
    inputFile >> domout;
    inputFile >> numberOfPoints;
    // read surface local point ids
    for (int i=0; i<numberOfPoints; i++)
    {
      inputFile >> pointId; // skip, don't store      

        surfacePointIds->InsertUniqueId(pointId-1); // id-s are 0-based in VTK and 1-based in the file

    }
    // read global point ids
    for (int i=0; i<numberOfPoints; i++)
    {
      inputFile >> pointId;
    
    }    
  }

  if (elem<numberOfSurfaceElements)
  {
    vtkErrorMacro("Not enough surface elements in the file");
    return false;
  }

  return true;
}

bool vtkNetgenMeshReader::ReadVolumeElements(ifstream &inputFile, vtkUnstructuredGrid *output)
{
  int numberOfVolumeElements=0;
  inputFile >> numberOfVolumeElements;

  if (numberOfVolumeElements<1)
  {
    vtkWarningMacro("No volume elements");
    output->Allocate(0);
    return false;
  }
  
  output->Allocate(numberOfVolumeElements);

  vtkSmartPointer<vtkIntArray> materialArray=vtkSmartPointer<vtkIntArray>::New();
  materialArray->SetName(this->ArrayNameMaterial);
  materialArray->SetNumberOfComponents(1);
  materialArray->SetNumberOfTuples(numberOfVolumeElements);
  output->GetCellData()->AddArray(materialArray);
  
  int materialId=0;
  int numberOfPoints=0;
  int pointId=0;
  vtkIdType newCellId=0;
  vtkSmartPointer<vtkIdList> pointIdList=vtkSmartPointer<vtkIdList>::New();
  int elem=0;
  for (elem=0; elem<numberOfVolumeElements || inputFile.eof(); elem++)
  {    
    inputFile >> materialId;
    inputFile >> numberOfPoints;
    pointIdList->SetNumberOfIds(numberOfPoints);
    for (int i=0; i<numberOfPoints; i++)
    {
      inputFile >> pointId;
      pointIdList->SetId(i, pointId-1); // id-s are 0-based in VTK and 1-based in the file
    }
    switch (numberOfPoints)
    {
      case 4:
        newCellId=output->InsertNextCell(VTK_TETRA, pointIdList);
        materialArray->SetValue(newCellId, materialId-1); // material id-s are 0-based in VTK and 1-based in the file
        break;
      default:
        vtkErrorMacro("Unsupported element type ("<<numberOfPoints<<" nodes)");
    }
  }

  if (elem<numberOfVolumeElements)
  {
    vtkErrorMacro("Not enough volume elements in the file");
    return false;
  }

  return true;
}

bool vtkNetgenMeshReader::ReadPoints(ifstream &inputFile, vtkUnstructuredGrid *output)
{
  // clear points
  output->SetPoints(NULL);

  int numberOfPoints=0;
  inputFile >> numberOfPoints;
  if (numberOfPoints<1)
  {
    vtkWarningMacro("No points");    
    return false;
  }
  
  vtkSmartPointer<vtkDoubleArray> pointCoords=vtkSmartPointer<vtkDoubleArray>::New();
  pointCoords->SetNumberOfComponents(SUPPORTED_DIMENSION);
  pointCoords->SetNumberOfTuples(numberOfPoints);

  double* coordPtr=pointCoords->GetPointer(0);
  int numberOfCoords=numberOfPoints*SUPPORTED_DIMENSION;
  int elem=0;
  for (elem=0; elem<numberOfCoords || inputFile.eof(); elem++)
  {    
    inputFile >> *coordPtr;
    coordPtr++;
  }

  // Save into output
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->SetData(pointCoords);
  output->SetPoints(points);

  if (elem<numberOfCoords)
  {
    vtkErrorMacro("Not enough data in the file");
    return false;
  }

  return true;
}
