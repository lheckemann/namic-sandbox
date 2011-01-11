/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSMeshWriter.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMeshWriter.h"

#include "vtkByteSwap.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"

#if !defined(_WIN32) || defined(__CYGWIN__)
# include <unistd.h> /* unlink */
#else
# include <io.h> /* unlink */
#endif

static const int SUPPORTED_DIMENSION=3;

vtkCxxRevisionMacro(vtkSMeshWriter, "$Revision: 1.43 $");
vtkStandardNewMacro(vtkSMeshWriter);

vtkSMeshWriter::vtkSMeshWriter() 
{
  this->ArrayNameMaterial=NULL;
  this->SetArrayNameMaterial("material");
}

vtkSMeshWriter::~vtkSMeshWriter() 
{
}

void vtkSMeshWriter::WriteData()
{
  ostream *fp;
  vtkPolyData *input= vtkPolyData::SafeDownCast(this->GetInput());

  vtkDebugMacro(<<"Writing vtk polydata...");

  if ( !(fp=this->OpenVTKFile()))
    {
    if (fp)
      {
      vtkErrorMacro("Ran out of disk space; deleting file: "<< this->FileName);
      this->CloseVTKFile(fp);
#if defined(_WIN32)
      _unlink(this->FileName);
#else
      unlink(this->FileName);
#endif
      }
    return;
    }
  
  *fp << "#smesh multi-surface mesh file for tetgen volumetric mesh generation" << vtkstd::endl;

  WriteGeometry(fp);

  *fp << "#smesh file end." << vtkstd::endl;

  this->CloseVTKFile(fp);  
}

int vtkSMeshWriter::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}

vtkPolyData* vtkSMeshWriter::GetInput()
{
  return vtkPolyData::SafeDownCast(this->Superclass::GetInput());
}

vtkPolyData* vtkSMeshWriter::GetInput(int port)
{
  return vtkPolyData::SafeDownCast(this->Superclass::GetInput(port));
}

void vtkSMeshWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

bool vtkSMeshWriter::WriteGeometry(ostream* fp)
{
  vtkPolyData *input = this->GetInput();
  if (input==NULL)
  {
    vtkErrorMacro("Invalid input");
    return false;
  }

  vtkCellArray *polys = input->GetPolys();
  vtkPoints *pts = input->GetPoints();
  if (pts == NULL || polys == NULL )
  {
    vtkErrorMacro(<<"No data to write!");
    return false;
  }

/*

  vtkPolyData *input= vtkPolyData::SafeDownCast(this->GetInput());
  
  if (input->GetPoints()==NULL)
  {
    vtkErrorMacro("No point data");
    return false;
  }
  if (input->GetPoints()->GetData()==NULL)
  {
    vtkErrorMacro("No point vector");
    return false;
  }
  vtkDoubleArray* pointCoords=vtkDoubleArray::SafeDownCast(input->GetPoints()->GetData());
  if (pointCoords==NULL)
  {
    vtkErrorMacro("Point vector type shall be double");
    return false;
  }
*/

  // Write nodes
  *fp << "#Part 1 - node list" << vtkstd::endl;
  const int numberOfComponents=3;
  int numberOfPoints=pts->GetNumberOfPoints(); 
  *fp << numberOfPoints << " " << numberOfComponents << " 0 0" << vtkstd::endl;
  fp->setf(ios::scientific,ios::floatfield);   // floatfield set to scientific  
  double coord[3];
  for (int p=0; p<numberOfPoints; p++)
  {
    pts->GetPoint(p,coord);
    *fp << p << " " << coord[0] << " " << coord[1] << " " << coord[2] << vtkstd::endl;    
  }

  // Write facets
  *fp << "#Part 2 - facet list" << vtkstd::endl;
  vtkIntArray* materialArray=NULL;
  if (input->GetCellData()!=NULL)
  {
    materialArray=vtkIntArray::SafeDownCast(input->GetCellData()->GetArray(this->ArrayNameMaterial));
  }
  else
  {
    vtkWarningMacro("No material data found");
  } 
  
  int numberOfCells=input->GetNumberOfCells();
  const int numberOfAttributes=1; // material ID
  *fp << numberOfCells << " " << numberOfAttributes << vtkstd::endl;
  int materialId=-1; // no material
  /*
  vtkCell* cell=NULL;  
  for (int c=0; c<numberOfCells; c++)
  {
    cell=input->GetCell(c);
    if (cell==NULL)
    {
      vtkWarningMacro("Invalid cell, ignore it");
      continue;
    }
    if (materialArray!=NULL)
    {
      materialId=materialArray->GetValue(c);
    }
    switch (cell->GetCellType())
    {
    case VTK_TRIANGLE:
      *fp << "3 " << cell->GetPointId(0) << " " << cell->GetPointId(1) << " " << cell->GetPointId(2);
      *fp << " " << materialId;
      *fp << vtkstd::endl;
      break;
    default:
      vtkWarningMacro("Unsupported element type ("<<cell->GetCellType()<<"), ignore it");
    }
  }
*/
  const int numberOfExpectedPoints=3;
  vtkIdType npts = 0;
  vtkIdType *indx = 0;
  int cellId=0;
  for (polys->InitTraversal(); polys->GetNextCell(npts,indx); cellId++)
  {
    if (materialArray!=NULL)
    {
      materialId=materialArray->GetValue(cellId);
    }
    if (npts==numberOfExpectedPoints)
    {
      *fp << numberOfExpectedPoints << " " << indx[0] << " " << indx[1] << " " << indx[2];
      *fp << " " << materialId;
      *fp << vtkstd::endl;
    }
    else
    {
      vtkWarningMacro("Unsupported number of nodes ("<<npts<<"), ignore polygon");
    }
  }

  // Write holes
  *fp << "#Part 3 - (volume) hole list" << vtkstd::endl;
  *fp << "0" << vtkstd::endl; // no holes

  // Write regions
  *fp << "#Part 4 - region attributes list" << vtkstd::endl;
  *fp << this->MeshRegionList.size() << vtkstd::endl; // no holes
  for (unsigned int i=0; i<this->MeshRegionList.size(); i++)
  {
    *fp << i+1;
    *fp << " " << this->MeshRegionList[i].position[0] << " " << this->MeshRegionList[i].position[1] << " " << this->MeshRegionList[i].position[2];
    *fp << " " << this->MeshRegionList[i].materialId << " " << this->MeshRegionList[i].maxElementVolume;
    *fp << " # inside " << this->MeshRegionList[i].name;
    *fp << vtkstd::endl;
  }

  return true;
}

void vtkSMeshWriter::AddRegion(const char* name, double *pos, int materialId, double maxElementVolume/*=-1*/)
{
  MeshRegion region;
  region.name=name;
  region.position[0]=pos[0];
  region.position[1]=pos[1];
  region.position[2]=pos[2];
  region.maxElementVolume=maxElementVolume;
  region.materialId=materialId;

  this->MeshRegionList.push_back(region);
}
