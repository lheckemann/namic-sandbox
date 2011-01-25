/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkNetgenMeshReader.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkNetgenMeshReader - reads a dataset in Fluent GAMBIT neutral file format
// .SECTION Description
// vtkNetgenMeshReader creates an unstructured grid dataset. It reads ASCII files
// stored in netgen neutral format, with optional data stored at the nodes or
// at the cells of the model. A cell-based fielddata stores the material id.

#ifndef __vtkNetgenMeshReader_h
#define __vtkNetgenMeshReader_h

#include "vtkUnstructuredGridAlgorithm.h"
class vtkIdList;
class vtkDoubleArray;
class vtkNetgenMeshReader : public vtkUnstructuredGridAlgorithm
{
protected:

public:
  static vtkNetgenMeshReader *New();
  vtkTypeRevisionMacro(vtkNetgenMeshReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the file name of the data file to read.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  vtkSetStringMacro(ArrayNameMaterial);
  vtkGetStringMacro(ArrayNameMaterial);

  vtkSetStringMacro(ArrayNameOnSurface);
  vtkGetStringMacro(ArrayNameOnSurface);

protected:

  bool ReadHeader(ifstream &inputFile);

  bool ReadSurfaceElements(ifstream &inputFile, vtkIdList *surfacePointIds);
  bool ReadVolumeElements(ifstream &inputFile, vtkUnstructuredGrid *output);
  bool ReadPoints(ifstream &inputFile, vtkUnstructuredGrid *output);

  vtkNetgenMeshReader();
  ~vtkNetgenMeshReader();
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  char *FileName;

  char* ArrayNameMaterial;
  char* ArrayNameOnSurface;

private:

  vtkNetgenMeshReader(const vtkNetgenMeshReader&);  // Not implemented.
  void operator=(const vtkNetgenMeshReader&);  // Not implemented.
};

#endif 
