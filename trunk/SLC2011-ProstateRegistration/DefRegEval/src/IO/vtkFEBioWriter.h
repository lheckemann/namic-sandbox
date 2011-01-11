/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFEBioWriter.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkFEBioWriter - write vtk unstructured grid data file
// .SECTION Description
// vtkFEBioWriter is a source object that writes ASCII 
// unstructured grid data files in FEBio format.

#ifndef __vtkFEBioWriter_h
#define __vtkFEBioWriter_h

#include "vtkDataWriter.h"
#include "vtkSmartPointer.h"
#include <vtkstd/list>

class vtkUnstructuredGrid;
class vtkIdList;

class vtkFEBioWriter : public vtkDataWriter
{
public:
  static vtkFEBioWriter *New();
  vtkTypeRevisionMacro(vtkFEBioWriter,vtkDataWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the input to this writer.
  vtkUnstructuredGrid* GetInput();
  vtkUnstructuredGrid* GetInput(int port);

  void AddPointFixBoundaryCondition(vtkIdList* pointList, const char* direction);
  void AddPointForceBoundaryCondition(vtkIdList* pointList, const char* direction, double magnitude);
  void AddContactSurfaceBoundaryCondition(vtkIdList* masterTrianglePointList, vtkIdList* slaveTrianglePointList);

  vtkSetMacro(NumberOfTimeSteps, int);
  vtkGetMacro(NumberOfTimeSteps, int);

  // Description:
  // Title of the solution
  vtkSetStringMacro(Title);
  vtkGetStringMacro(Title);

  vtkGetStringMacro(ArrayNameMaterial);
  vtkSetStringMacro(ArrayNameMaterial);

  vtkSetMacro(OrganMaterialId, int);
  vtkGetMacro(OrganMaterialId, int);

  vtkSetMacro(SupportMaterialId, int);
  vtkGetMacro(SupportMaterialId, int);

protected:
  vtkFEBioWriter();
  virtual ~vtkFEBioWriter();

  // BTX
  class BoundaryCondition
  {
  public:
    enum ConditionType
    {
      FIX,
      FORCE,
      CONTACT
    };
    BoundaryCondition()
    {
      pointList=vtkSmartPointer<vtkIdList>::New();
      pointList2=vtkSmartPointer<vtkIdList>::New();
      magnitude=0;
    }
    virtual ~BoundaryCondition()
    {
    }
    vtkstd::string direction;
    double magnitude;
    vtkSmartPointer<vtkIdList> pointList;
    vtkSmartPointer<vtkIdList> pointList2;
    ConditionType condType;
  };
  vtkstd::list<BoundaryCondition> BoundaryConditions;
  // ETX

  void WriteData();

  virtual int FillInputPortInformation(int port, vtkInformation *info);

  // solver time
  bool WriteControl(ostream* fp);
  
  // material properties
  bool WriteMaterial(ostream* fp); 
  
  // nodes, elements
  bool WriteGeometry(ostream* fp); 

  // boundary conditions (fix, force, etc.)
  bool WriteBoundary(ostream* fp); 
  
  // curves for loads
  bool WriteLoadData(ostream* fp);

  char* Title;
  double MaxRefs;
  double MaxUps;
  double DTol;
  double ETol;
  double RTol;
  double LsTol;
  double PressureStiffness;
  int NumberOfTimeSteps;
  double DtMin;
  double TMax;
  double MaxRetries;
  double OptIter;

  char* ArrayNameMaterial;

  int OrganMaterialId;
  int SupportMaterialId;

private:
  vtkFEBioWriter(const vtkFEBioWriter&);  // Not implemented.
  void operator=(const vtkFEBioWriter&);  // Not implemented.
};

#endif


