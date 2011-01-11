/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFEBioWriter.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "DefRegEval.h"

#include "vtkFEBioWriter.h"

#include "vtkByteSwap.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGrid.h"
#include "vtkIdList.h"

#if !defined(_WIN32) || defined(__CYGWIN__)
# include <unistd.h> /* unlink */
#else
# include <io.h> /* unlink */
#endif

vtkCxxRevisionMacro(vtkFEBioWriter, "$Revision: 1.43 $");
vtkStandardNewMacro(vtkFEBioWriter);

static const double END_TIME=1.0;
static const double UNIT_LOAD=1.0;


vtkFEBioWriter::vtkFEBioWriter() 
{
  this->Title=NULL;
  SetTitle("untitled");
  this->MaxRefs=15;
  this->MaxUps=10;
  this->DTol=0.001;
  this->ETol=0.01;
  this->RTol=0;
  this->LsTol=0.9;
  this->PressureStiffness=1;
  this->NumberOfTimeSteps=5;
  this->DtMin=0.01;
  this->TMax=1.0;
  this->MaxRetries=5;
  this->OptIter=10;

  this->ArrayNameMaterial=NULL;
  this->SetArrayNameMaterial("material");

  this->OrganMaterialId=0;
  this->SupportMaterialId=1;
}

vtkFEBioWriter::~vtkFEBioWriter() 
{
}

void vtkFEBioWriter::WriteData()
{
  ostream *fp;
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());

  vtkDebugMacro(<<"Writing vtk unstructured grid data...");

  if ( !(fp=this->OpenVTKFile()))
    {
    if (fp)
      {
      vtkErrorMacro("Ran out of disk space; deleting file: "<< this->FileName);
      this->CloseVTKFile(fp);
      _unlink(this->FileName);
      }
    return;
    }
  
  *fp << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << vtkstd::endl;
  *fp << "<febio_spec version=\"1.0\">" << vtkstd::endl;

  WriteControl(fp);
  WriteMaterial(fp); // material properties
  WriteGeometry(fp); // nodes, elements
  WriteBoundary(fp); // boundary conditions (fix, force, etc.)
  WriteLoadData(fp); // curves for loads

  *fp << "</febio_spec>" << vtkstd::endl;

  this->CloseVTKFile(fp);  
}

int vtkFEBioWriter::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
  return 1;
}

vtkUnstructuredGrid* vtkFEBioWriter::GetInput()
{
  return vtkUnstructuredGrid::SafeDownCast(this->Superclass::GetInput());
}

vtkUnstructuredGrid* vtkFEBioWriter::GetInput(int port)
{
  return vtkUnstructuredGrid::SafeDownCast(this->Superclass::GetInput(port));
}

void vtkFEBioWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


bool vtkFEBioWriter::WriteControl(ostream* fp)
{
  *fp << "<Control>" << vtkstd::endl;  
  *fp << "  <title>" << this->Title << "</title>" << vtkstd::endl;  
  *fp << "  <time_steps>"<<this->NumberOfTimeSteps<<"</time_steps>" << vtkstd::endl;  
  *fp << "  <step_size>"<<END_TIME/this->NumberOfTimeSteps<<"</step_size>" << vtkstd::endl;  
  *fp << "  <max_refs>"<<this->MaxRefs<<"</max_refs>" << vtkstd::endl;  
  *fp << "  <max_ups>"<<this->MaxUps<<"</max_ups>" << vtkstd::endl;  
  *fp << "  <dtol>"<<this->DTol<<"</dtol>" << vtkstd::endl;  
  *fp << "  <etol>"<<this->ETol<<"</etol>" << vtkstd::endl;  
  *fp << "  <rtol>"<<this->RTol<<"</rtol>" << vtkstd::endl;  
  *fp << "  <lstol>"<<this->LsTol<<"</lstol>" << vtkstd::endl;  
  *fp << "  <pressure_stiffness>"<<this->PressureStiffness<<"</pressure_stiffness>" << vtkstd::endl;  
  *fp << "  <time_stepper>" << vtkstd::endl;  
  *fp << "    <dtmin>"<<this->DtMin<<"</dtmin>" << vtkstd::endl;  
  *fp << "    <dtmax>"<<this->TMax/double(this->NumberOfTimeSteps)<<"</dtmax>" << vtkstd::endl;  
  *fp << "    <max_retries>"<<this->MaxRetries<<"</max_retries>" << vtkstd::endl;  
  *fp << "    <opt_iter>"<<this->OptIter<<"</opt_iter>" << vtkstd::endl;  
  *fp << "  </time_stepper>" << vtkstd::endl;  
  //*fp << "  <plot_level>PLOT_DEFAULT</plot_level>" << vtkstd::endl;  
  *fp << "  <plot_level>PLOT_MAJOR_ITRS</plot_level>" << vtkstd::endl;   // plot only major iterations (don't plot more time steps if the solver had to insert additional steps)
  *fp << "</Control>" << vtkstd::endl;
  return true;
}

bool vtkFEBioWriter::WriteMaterial(ostream* fp)
{
  *fp << "<Material>" << vtkstd::endl;  

  // material id-s are 1-based in the file and 0-based in VTK
  *fp << "  <material id=\"" << this->OrganMaterialId+1 << "\" name=\"OrganTissue\" type=\"linear elastic\">" << vtkstd::endl;
  *fp << "    <E>21</E>" << vtkstd::endl;
  *fp << "    <v>0.45</v>" << vtkstd::endl;
  *fp << "  </material>" << vtkstd::endl;

  // material id-s are 1-based in the file and 0-based in VTK
  *fp << "  <material id=\"" << this->SupportMaterialId+1 << "\" name=\"SupportTissue\" type=\"linear elastic\">" << vtkstd::endl;
  *fp << "    <E>11</E>" << vtkstd::endl;
  *fp << "    <v>0.3</v>" << vtkstd::endl;
  *fp << "  </material>" << vtkstd::endl;

  *fp << "</Material>" << vtkstd::endl;

  return true;
}

bool vtkFEBioWriter::WriteGeometry(ostream* fp)
{
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());
  if (input==NULL)
  {
    vtkErrorMacro("Invalid input");
    return false;
  }
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

  *fp << "<Geometry>" << vtkstd::endl;

  // Write nodes  
  *fp << "<Nodes>" << vtkstd::endl;
  int numberOfComponents=pointCoords->GetNumberOfComponents();
  int numberOfPoints=pointCoords->GetNumberOfTuples(); 
  double* coordPtr=pointCoords->GetPointer(0);
  fp->setf(ios::scientific,ios::floatfield);   // floatfield set to scientific  
  for (int p=0; p<numberOfPoints; p++)
  {
    *fp << "  <node id=\"" << p+1 << "\"> "; // node id-s are 1-based in the file and 0-based in VTK
    *fp << *coordPtr << ", ";
    coordPtr++;
    *fp << *coordPtr << ", ";
    coordPtr++;
    *fp << *coordPtr;
    coordPtr++;
    *fp << "</node>" << vtkstd::endl;    
  }
  *fp << "</Nodes>" << vtkstd::endl;
  
  // Write elements  
  *fp << "<Elements>" << vtkstd::endl;

  vtkIntArray* materialArray=NULL;
  if (input->GetCellData()!=NULL)
  {
    materialArray=vtkIntArray::SafeDownCast(input->GetCellData()->GetArray(DefRegEvalGlobal::ArrayNameMaterial));
  }
  else
  {
    vtkWarningMacro("No material data found");
  } 
  int numberOfCells=input->GetNumberOfCells();
  int numberOfNodes=0; 
  vtkCell* cell=NULL;
  int materialId=-1; // no material
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
      case VTK_TETRA:
        *fp << "  <tet4 id=\"" << c+1 << "\" mat=\"" << materialId+1 <<"\"> "; // xml cell id is 1-based in the file; material id-s are 1-based in the file and 0-based in VTK
        *fp << cell->GetPointId(0)+1 << ", " << cell->GetPointId(1)+1 << ", " << cell->GetPointId(2)+1 << ", " << cell->GetPointId(3)+1; // cell ids are 1-based in the file, 0-based in VTK
        *fp << "</tet4>" << vtkstd::endl;
        break;
      default:
        vtkWarningMacro("Unsupported element type ("<<cell->GetCellType()<<"), ignore it");
    }
  }
  *fp << "</Elements>" << vtkstd::endl;

  *fp << "</Geometry>" << vtkstd::endl;

  return true;
}

bool vtkFEBioWriter::WriteBoundary(ostream* fp)
{
  *fp << "<Boundary>" << vtkstd::endl;

  fp->setf(ios::fixed,ios::floatfield);   // floatfield set to fixed

  int loadCurveId=0;

  for (vtkstd::list<BoundaryCondition>::iterator it = this->BoundaryConditions.begin(); it != this->BoundaryConditions.end(); ++it)
  {
    switch (it->condType)
    {
    case BoundaryCondition::FIX:
      *fp << "<fix>" << vtkstd::endl;
      for (int i=0; i<it->pointList->GetNumberOfIds(); i++)
      {
        *fp << "<node id=\"" << it->pointList->GetId(i)+1 <<"\" bc=\"" << it->direction << "\"> </node>" << vtkstd::endl; // file is 1-based, VTK is 0-based
      }
      *fp << "</fix>" << vtkstd::endl;
      break;
    }
  }
  *fp << "<force>" << vtkstd::endl;
  for (vtkstd::list<BoundaryCondition>::iterator it = this->BoundaryConditions.begin(); it != this->BoundaryConditions.end(); ++it)
  {
    switch (it->condType)
    {
    case BoundaryCondition::FORCE:

      for (int i=0; i<it->pointList->GetNumberOfIds(); i++)
      {
        // use always lc=1 (standard load curve from 0 to 1)
        *fp << "<node id=\"" << it->pointList->GetId(i)+1 <<"\" bc=\"" << it->direction << "\" lc=\""<<loadCurveId+1<<"\">" << it->magnitude <<"</node>" << vtkstd::endl; // file is 1-based, VTK is 0-based        
      }
      loadCurveId++;
      break;
    }
  }
  *fp << "</force>" << vtkstd::endl;

  for (vtkstd::list<BoundaryCondition>::iterator it = this->BoundaryConditions.begin(); it != this->BoundaryConditions.end(); ++it)
  {
    switch (it->condType)
    {
    case BoundaryCondition::CONTACT:
      /*
      *fp << "<contact type=\"sliding_with_gaps\">" << vtkstd::endl;
      *fp << "<laugon>0</laugon>" << vtkstd::endl;
      *fp << "<tolerance>0.2</tolerance>" << vtkstd::endl;
      *fp << "<two_pass>0</two_pass>" << vtkstd::endl;
      *fp << "<penalty>1</penalty>" << vtkstd::endl;
      */

      /*
      *fp << "<contact type=\"facet-to-facet sliding\">" << vtkstd::endl;
      *fp << "<laugon>0</laugon>" << vtkstd::endl;
      *fp << "<tolerance>0.2</tolerance>" << vtkstd::endl;
      *fp << "<two_pass>0</two_pass>" << vtkstd::endl;
      *fp << "<penalty>1</penalty>" << vtkstd::endl;
*/
      *fp << "<contact type=\"tied\">" << vtkstd::endl;
      *fp << "<laugon>0</laugon>" << vtkstd::endl;
      *fp << "<tolerance>0.2</tolerance>" << vtkstd::endl;
      *fp << "<penalty>1</penalty>" << vtkstd::endl;

      *fp << "<surface type=\"master\">" << vtkstd::endl;
      int id=1;
      for (int i=0; i<it->pointList->GetNumberOfIds()/3; i++)
      {
        // use always lc=1 (standard load curve from 0 to 1)
        *fp << "<tri3 id=\"" << id++ <<"\"> ";
        *fp << it->pointList->GetId(i*3+0)+1 << ", "; // file is 1-based, VTK is 0-based        
        *fp << it->pointList->GetId(i*3+1)+1 << ", "; // file is 1-based, VTK is 0-based        
        *fp << it->pointList->GetId(i*3+2)+1 << "</tri3>" << vtkstd::endl; // file is 1-based, VTK is 0-based        
      }    
      *fp << "</surface>" << vtkstd::endl;

      vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());
      vtkDoubleArray* pointCoords=vtkDoubleArray::SafeDownCast(input->GetPoints()->GetData());
      int supportMeshPointOffset=pointCoords->GetNumberOfTuples();

      *fp << "<surface type=\"slave\">" << vtkstd::endl;
      id=1;
      for (int i=0; i<it->pointList2->GetNumberOfIds()/3; i++)
      {
        // use always lc=1 (standard load curve from 0 to 1)
        *fp << "<tri3 id=\"" << id++ <<"\"> ";
        *fp << it->pointList2->GetId(i*3+0)+1+supportMeshPointOffset << ", "; // file is 1-based, VTK is 0-based        
        *fp << it->pointList2->GetId(i*3+1)+1+supportMeshPointOffset << ", "; // file is 1-based, VTK is 0-based        
        *fp << it->pointList2->GetId(i*3+2)+1+supportMeshPointOffset << "</tri3>" << vtkstd::endl; // file is 1-based, VTK is 0-based        
      }    
      *fp << "</surface>" << vtkstd::endl;

      *fp << "</contact>" << vtkstd::endl;
      break;
    }
  }
  
  *fp << "</Boundary>" << vtkstd::endl;  
  return true;
}

bool vtkFEBioWriter::WriteLoadData(ostream* fp)
{
  *fp << "<LoadData>" << vtkstd::endl;

  int loadCurveId=0;
  for (vtkstd::list<BoundaryCondition>::iterator it = this->BoundaryConditions.begin(); it != this->BoundaryConditions.end(); ++it)
  {
    switch (it->condType)
    {
    case BoundaryCondition::FORCE:
      *fp << "<loadcurve id=\""<<loadCurveId+1<<"\">" << vtkstd::endl;
      *fp << "<loadpoint>0,0</loadpoint>" << vtkstd::endl;
      *fp << "<loadpoint>" << END_TIME <<"," << UNIT_LOAD << "</loadpoint>" << vtkstd::endl;
      *fp << "</loadcurve>" << vtkstd::endl;  
      loadCurveId++;
      break;
    }
  }

  *fp << "</LoadData>" << vtkstd::endl;
  return true;
}


void vtkFEBioWriter::AddPointFixBoundaryCondition(vtkIdList* pointList, const char* direction)
{
  BoundaryCondition bc;
  bc.condType=BoundaryCondition::FIX;
  bc.direction=direction;
  bc.pointList->DeepCopy(pointList);
  this->BoundaryConditions.push_back(bc);
}

void vtkFEBioWriter::AddPointForceBoundaryCondition(vtkIdList* pointList, const char* direction, double magnitude)
{
  BoundaryCondition bc;
  bc.condType=BoundaryCondition::FORCE;
  bc.direction=direction;
  bc.pointList->DeepCopy(pointList);
  bc.magnitude=magnitude;
  this->BoundaryConditions.push_back(bc);
}

void vtkFEBioWriter::AddContactSurfaceBoundaryCondition(vtkIdList* masterTrianglePointList, vtkIdList* slaveTrianglePointList)
{
  BoundaryCondition bc;
  bc.condType=BoundaryCondition::CONTACT;
  bc.pointList->DeepCopy(masterTrianglePointList);

  // slave points are in the support mesh
  bc.pointList2->DeepCopy(slaveTrianglePointList);

  this->BoundaryConditions.push_back(bc);
}
