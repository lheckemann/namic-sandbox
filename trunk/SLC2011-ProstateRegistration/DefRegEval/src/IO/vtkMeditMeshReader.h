/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMeditMeshReader.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMeditMeshReader - reads a dataset in Fluent GAMBIT neutral file format
// .SECTION Description
// vtkMeditMeshReader creates an unstructured grid dataset. It reads ASCII files
// stored in netgen neutral format, with optional data stored at the nodes or
// at the cells of the model. A cell-based fielddata stores the material id.

#ifndef __vtkMeditMeshReader_h
#define __vtkMeditMeshReader_h

#include "vtkUnstructuredGridAlgorithm.h"
class vtkIdList;
class vtkDoubleArray;
class vtkMeditMeshReader : public vtkUnstructuredGridAlgorithm
{
protected:

public:
  static vtkMeditMeshReader *New();
  vtkTypeRevisionMacro(vtkMeditMeshReader,vtkUnstructuredGridAlgorithm);
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

  vtkMeditMeshReader();
  ~vtkMeditMeshReader();
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  char* FileName;

  char* ArrayNameMaterial;
  char* ArrayNameOnSurface;

private:

  vtkMeditMeshReader(const vtkMeditMeshReader&);  // Not implemented.
  void operator=(const vtkMeditMeshReader&);  // Not implemented.
};

#endif 
