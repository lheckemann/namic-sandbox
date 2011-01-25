/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSMeshWriter.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMeshWriter - write multi-material surface data file
// .SECTION Description
// vtkSMeshWriter is a source object that writes data files in smesh format for
// tetgen volumetric mesher.

#ifndef __vtkSMeshWriter_h
#define __vtkSMeshWriter_h

#include "vtkDataWriter.h"
#include "vtkSmartPointer.h"
#include <vtkstd/vector>

class vtkPolyData;
class vtkIdList;

class vtkSMeshWriter : public vtkDataWriter
{
public:
  static vtkSMeshWriter *New();
  vtkTypeRevisionMacro(vtkSMeshWriter,vtkDataWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the input to this writer.
  vtkPolyData* GetInput();
  vtkPolyData* GetInput(int port);

  void AddRegion(const char* name, double *pos, int materialId, double maxElementVolume=-1);

  vtkSetStringMacro(ArrayNameMaterial);
  vtkGetStringMacro(ArrayNameMaterial);

protected:
  vtkSMeshWriter();
  virtual ~vtkSMeshWriter();

  void WriteData();

  virtual int FillInputPortInformation(int port, vtkInformation *info);
  
  // nodes, elements
  bool WriteGeometry(ostream* fp); 

  // BTX
  struct MeshRegion
  {
    vtkstd::string name;
    double position[3];
    double maxElementVolume;
    int materialId;    
  };
  typedef vtkstd::vector<MeshRegion> MeshRegionListType;
  MeshRegionListType MeshRegionList;
  // ETX

  char* ArrayNameMaterial;

private:
  vtkSMeshWriter(const vtkSMeshWriter&);  // Not implemented.
  void operator=(const vtkSMeshWriter&);  // Not implemented.
};

#endif


