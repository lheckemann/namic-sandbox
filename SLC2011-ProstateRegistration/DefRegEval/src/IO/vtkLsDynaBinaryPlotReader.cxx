/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkLsDynaBinaryPlotReader.cxx,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Disable fopen deprecation warning
#define  _CRT_SECURE_NO_WARNINGS

#include "vtkLsDynaBinaryPlotReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkErrorCode.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"

vtkCxxRevisionMacro(vtkLsDynaBinaryPlotReader, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkLsDynaBinaryPlotReader);

//----------------------------------------------------------------------------
vtkLsDynaBinaryPlotReader::vtkLsDynaBinaryPlotReader()
{
  this->FileName = NULL;

  this->RequestedTimeStep=-1;
  this->NumberOfTimeSteps=0;

  this->ApplyDeformationToPoints=1;

  this->SetNumberOfInputPorts(0);

  this->ArrayNameTime=NULL;
  this->SetArrayNameTime("time");
  this->ArrayNameMaterial=NULL;
  this->SetArrayNameMaterial("material");
  this->ArrayNameDisplacement=NULL;
  this->SetArrayNameDisplacement("displacement");
  this->ArrayNameVelocity=NULL;
  this->SetArrayNameVelocity("velocity");
  this->ArrayNameAcceleration=NULL;
  this->SetArrayNameAcceleration("acceleration");
  this->ArrayNameTemperature=NULL;
  this->SetArrayNameTemperature("temperature");
}

//----------------------------------------------------------------------------
vtkLsDynaBinaryPlotReader::~vtkLsDynaBinaryPlotReader()
{
  SetFileName(NULL);
}

//----------------------------------------------------------------------------
int vtkLsDynaBinaryPlotReader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDebugMacro( << "Reading LS-DYNA binary plot file");

  FILE* fp= fopen(this->FileName,"rb");
  if (!fp)
  {
    vtkErrorMacro("Specified filename not found");
    this->SetErrorCode(vtkErrorCode::FileNotFoundError);
    return 0;
  }

  // Read header
  FileHeaderType fh;
  fread(&fh, sizeof(fh), 1, fp);
  if (ferror(fp))
  {
    vtkErrorMacro("Error reading file header");
    this->SetErrorCode(vtkErrorCode::PrematureEndOfFileError);
  }   

  this->ReadNodePoints(output, fp, fh);
  this->ReadGeometry(output, fp, fh);

  vtkSmartPointer<vtkFloatArray> timeStepTimes = vtkSmartPointer<vtkFloatArray>::New();
  timeStepTimes->SetNumberOfComponents(1);
  timeStepTimes->SetNumberOfTuples(0); // will add timesteps one by one
  timeStepTimes->SetName(this->ArrayNameTime);

  // get total number of timesteps
  this->NumberOfTimeSteps=0;
  long timeStepsStartPos=ftell(fp);
  fseek(fp, 0, SEEK_END);
  long fileLength=ftell(fp);
  fseek(fp,timeStepsStartPos,SEEK_SET);
  long curPos=0;
  while (ftell(fp)<fileLength)
  {
    this->SkipTimeStep(fp, fh);
    this->NumberOfTimeSteps++;
  }
  fseek(fp,timeStepsStartPos,SEEK_SET);

  int timeStepToLoad=this->RequestedTimeStep;
  if (timeStepToLoad<0)
  {
    // get the last timestep
    timeStepToLoad=NumberOfTimeSteps-1;
  }

  // Contains real time of the timestep(s)
  output->GetFieldData()->AddArray(timeStepTimes);

  if (timeStepToLoad>=0)
  {
    for (int i=0; i<timeStepToLoad; i++)
    {
      this->SkipTimeStep(fp, fh);
    }
    if (this->ReadTimeStep(output, fp, fh))
    {
      // success

      // replace node points with deformed node points (optional), and set deformation into the displacement variable (instead of node position)
      vtkFloatArray* deformedPointCoordinates = vtkFloatArray::SafeDownCast(output->GetPointData()->GetArray(this->ArrayNameDisplacement));
      vtkFloatArray* originalPoints=vtkFloatArray::SafeDownCast(output->GetPoints()->GetData());
      if (deformedPointCoordinates!=NULL && originalPoints!=NULL)
      {
        float* orig=originalPoints->GetPointer(0);
        float* deformed=deformedPointCoordinates->GetPointer(0);
        const int numberOfComponents=3; // coordinates are always in 3D
        int numValues=fh.nump*numberOfComponents;
        for (int i=0; i<numValues; i++)
        {
          float diff=*deformed-*orig;
          if (this->ApplyDeformationToPoints)
          {
            *orig=*deformed;
          }
          *deformed=diff;
          orig++;
          deformed++;
        }
      }

    }
    else
    {
      vtkErrorMacro("Cannot load time step "<<timeStepToLoad);
    }
  }

  fclose(fp);

  return 1;
}

//----------------------------------------------------------------------------
void vtkLsDynaBinaryPlotReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "File Name: " 
    << (this->FileName ? this->FileName : "(none)") << "\n";

}

//----------------------------------------------------------------------------
int vtkLsDynaBinaryPlotReader::RequestInformation(
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

  FILE* fp= fopen(this->FileName,"rb");
  if (!fp)
  {
    vtkErrorMacro("Specified filename not found");
    this->SetErrorCode(vtkErrorCode::FileNotFoundError);
    return 0;
  }

  // Read header
  FileHeaderType fh;
  fread(&fh, sizeof(fh), 1, fp);
  if (ferror(fp))
  {
    vtkErrorMacro("Error reading file header");
    this->SetErrorCode(vtkErrorCode::PrematureEndOfFileError);
  }   

  fclose(fp);

  // Check header
  // TODO: fill other properties as well, from the header
  if (fh.nump<1)
  {
    vtkWarningMacro(<<"No nodes in the file");
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkLsDynaBinaryPlotReader::ReadNodePoints(vtkUnstructuredGrid *output, FILE* fp, const FileHeaderType &fh)
{  
  // read the node coordinates  
  const int numberOfComponents=3; // coordinates are always in 3D
  int numValues=fh.nump*numberOfComponents;

  // Read from file
  vtkSmartPointer<vtkFloatArray> floats=vtkSmartPointer<vtkFloatArray>::New();
  floats->SetNumberOfComponents(numberOfComponents);
  floats->SetNumberOfTuples(fh.nump);
  fread(floats->GetPointer(0), sizeof(float), numValues, fp);  

  // Save double[] buffer into the dataset
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->SetData(floats);
  output->SetPoints(points);
}

//----------------------------------------------------------------------------
void vtkLsDynaBinaryPlotReader::ReadGeometry(vtkUnstructuredGrid *output, FILE* fp, const FileHeaderType &fh)
{
  output->Allocate();

  vtkSmartPointer<vtkIntArray> materialArray=vtkSmartPointer<vtkIntArray>::New();
  materialArray->SetName(this->ArrayNameMaterial);
  materialArray->SetNumberOfComponents(1);
  materialArray->SetNumberOfTuples(fh.nel8+fh.nel2+fh.nel4);
  output->GetCellData()->AddArray(materialArray);

  // Hexahedron
  {
    const int numberOfNodesPerElement=8;
    vtkIdType nodeIds[numberOfNodesPerElement];
    int materialId;
    int n=0;
    vtkIdType newCellId=0;
    for (int elem=0; elem<fh.nel8; ++elem)
    {
      for (int node=0; node<numberOfNodesPerElement; node++)
      {
        fread(&n, sizeof(n), 1, fp);  
        nodeIds[node]=n-1; // -1 because node ids in the file are 1-based, node ids in VTK are 0-based
      }
      fread(&materialId, sizeof(materialId), 1, fp);  

      // Detect cell type (heuristic, tailored to FEBio writer, it may detect all elements as HEXAHEDRON if used with an other writer)
      if (nodeIds[2]==nodeIds[3] &&
        nodeIds[4]==nodeIds[5] &&
        nodeIds[4]==nodeIds[6] &&
        nodeIds[4]==nodeIds[7])
      {
        // FEBio-written tetrahedron (node order: 0, 1, 2, 2, 3, 3, 3, 3)
        nodeIds[3]=nodeIds[4]; // reorder nodes to have the first 4 nodes in the first 4 places in the list
        newCellId=output->InsertNextCell(VTK_TETRA, 4, nodeIds);
      }
      else
      {
        newCellId=output->InsertNextCell(VTK_HEXAHEDRON, 8, nodeIds);
      }
      materialArray->SetValue(newCellId, materialId);
    }
  }

  // Truss
  {
    const int numberOfNodesPerElement=5;
    vtkIdType nodeIds[numberOfNodesPerElement];
    int materialId;
    int n=0;
    vtkIdType newCellId=0;
    for (int elem=0; elem<fh.nel2; ++elem)
    {
      for (int node=0; node<numberOfNodesPerElement; node++)
      {
        fread(&n, sizeof(n), 1, fp);  
        nodeIds[node]=n-1; // -1 because node ids in the file are 1-based, node ids in VTK are 0-based
      }
      fread(&materialId, sizeof(materialId), 1, fp);  
      // only the first two elements are used and they form a line
      newCellId=output->InsertNextCell(VTK_LINE, 2, nodeIds);
      materialArray->SetValue(newCellId, materialId);
    }
  }

  // Shell
  {
    const int numberOfNodesPerElement=4;
    vtkIdType nodeIds[numberOfNodesPerElement];
    int materialId;
    int n=0;
    vtkIdType newCellId=0;
    for (int elem=0; elem<fh.nel2; ++elem)
    {
      for (int node=0; node<numberOfNodesPerElement; node++)
      {
        fread(&n, sizeof(n), 1, fp);  
        nodeIds[node]=n-1; // -1 because node ids in the file are 1-based, node ids in VTK are 0-based
      }
      fread(&materialId, sizeof(materialId), 1, fp);  

      // Detect cell type (heuristic, tailored to FEBio writer)
      if (nodeIds[2]==nodeIds[3])
      {
        newCellId=output->InsertNextCell(VTK_TRIANGLE, 3, nodeIds);
      }
      else
      {
        newCellId=output->InsertNextCell(VTK_QUAD, 4, nodeIds);
      }
      materialArray->SetValue(newCellId, materialId);
    }
  }
}

//----------------------------------------------------------------------------
int vtkLsDynaBinaryPlotReader::ReadTimeStep(vtkUnstructuredGrid *output, FILE* fp, const FileHeaderType &fh)
{ 

  // Read timestep time
  float timeStepTime=0;
  if (fread(&timeStepTime, sizeof(timeStepTime), 1, fp)==0)
  {
    // end of file
    return 0;
  }

  vtkFloatArray* timeStepTimes = vtkFloatArray::SafeDownCast(output->GetFieldData()->GetArray(this->ArrayNameTime));
  if (timeStepTimes!=NULL)
  {
    timeStepTimes->InsertNextValue(timeStepTime);
  }  

  if (fh.flagU)
  {
    ReadPointFieldData(output, fp, fh, 3, this->ArrayNameDisplacement);
  }
  if (fh.flagV)
  {
    ReadPointFieldData(output, fp, fh, 3, this->ArrayNameVelocity);
  }
  if (fh.flagA)
  {
    ReadPointFieldData(output, fp, fh, 3, this->ArrayNameAcceleration);
  }
  if (fh.flagT)
  {
    ReadPointFieldData(output, fp, fh, 1, this->ArrayNameTemperature);
  }

  // Skip stress information  
  fseek(fp, fh.nel8*fh.nv3d*sizeof(float), SEEK_CUR); // Hexahedron
  fseek(fp, fh.nel2*fh.nv1d*sizeof(float), SEEK_CUR); // Truss
  fseek(fp, fh.nel4*fh.nv2d*sizeof(float), SEEK_CUR); // Shell

  return 1;
}

//----------------------------------------------------------------------------
int vtkLsDynaBinaryPlotReader::SkipTimeStep(FILE* fp, const FileHeaderType &fh)
{ 

  // Skip timestep time
  if (fseek(fp, 1*sizeof(float), SEEK_CUR)!=0)
  {
    // nonzero return value means failed, probably end of file
    return 0;
  }

  if (fh.flagU)
  {
    SkipPointFieldData(fp, fh, 3); // ARRAY_NAME_DISPLACEMENT
  }
  if (fh.flagV)
  {
    SkipPointFieldData(fp, fh, 3); // ARRAY_NAME_VELOCITY
  }
  if (fh.flagA)
  {
    SkipPointFieldData(fp, fh, 3); // ARRAY_NAME_ACCELERATION
  }
  if (fh.flagT)
  {
    SkipPointFieldData(fp, fh, 1); // ARRAY_NAME_TEMPERATURE
  }

  // Skip stress information  
  fseek(fp, fh.nel8*fh.nv3d*sizeof(float), SEEK_CUR); // Hexahedron
  fseek(fp, fh.nel2*fh.nv1d*sizeof(float), SEEK_CUR); // Truss
  fseek(fp, fh.nel4*fh.nv2d*sizeof(float), SEEK_CUR); // Shell

  return 1;
}

//----------------------------------------------------------------------------
void vtkLsDynaBinaryPlotReader::ReadPointFieldData(vtkUnstructuredGrid *output, FILE* fp, const FileHeaderType &fh, int numberOfComponents, const char* fieldName)
{
  // Create new field
  vtkSmartPointer<vtkFloatArray> values=vtkSmartPointer<vtkFloatArray>::New();
  values->SetName(fieldName);
  values->SetNumberOfComponents(numberOfComponents);
  values->SetNumberOfTuples(fh.nump);

  // Read data
  int numValues=fh.nump*numberOfComponents;
  fread(values->GetPointer(0), sizeof(float), numValues, fp);  

  // Save data in output
  output->GetPointData()->AddArray(values);
}

//----------------------------------------------------------------------------
void vtkLsDynaBinaryPlotReader::SkipPointFieldData(FILE* fp, const FileHeaderType &fh, int numberOfComponents)
{
  fseek(fp, fh.nump*numberOfComponents*sizeof(float), SEEK_CUR);
}
