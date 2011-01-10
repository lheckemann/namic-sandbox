/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkLsDynaBinaryPlotReader.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkLsDynaBinaryPlotReader - reads a dataset in LS-DYNA binary plot file format
// .SECTION Description
// vtkLsDynaBinaryPlotReader creates an unstructured grid dataset. It reads binary files
// stored in LS-DYNA plot format, with optional data stored at the nodes or
// at the cells of the model. A cell-based fielddata stores the material id.


#ifndef __vtkLsDynaBinaryPlotReader_h
#define __vtkLsDynaBinaryPlotReader_h

#include "vtkUnstructuredGridAlgorithm.h"

class vtkDoubleArray;
class vtkLsDynaBinaryPlotReader : public vtkUnstructuredGridAlgorithm
{
protected:

  struct FileHeaderType
  {
    char  Title[40];    //!< title of the problem
    int    UnUsed0[5];    //!< blanks (unused)
    int    ndim;      //!< number of dimensions    
    int    nump;      //!< number of nodal points
    int    icode;      //!< code descriptor
    int    nglbv;      //!< number of global state variables
    int    flagT;      //!< state nodal temperatures included ?
    int    flagU;      //!< state nodal coordinates included ?
    int    flagV;      //!< state nodal velocities included ?
    int    flagA;      //!< state nodal accelerations included ?
    int    nel8;      //!< number of 8-node hexahedral elements
    int    nummat8;    //!< number of materials used by hexahedral elements
    int    UnUsed1[2];    //!< blanks (unused)
    int    nv3d;      //!< number of variables for hexahedral elements
    int    nel2;      //!< number of 2-node beam elements
    int    nummat2;    //!< number of materials used by beam elements
    int    nv1d;      //!< number of variables for beam elements
    int    nel4;      //!< number of 4-node shell elements
    int    nummat4;    //!< number of materials used by shell elements
    int    nv2d;      //!< number of variables for shell elements
    int    neiph;      //!< number of additional variables per solid element
    int    neips;      //!< number of additional variables per shell integration point
    int    maxint;      //!< number of integration points dumped for each shell
    int    UnUsed3[7];    //!< blank (unused)
    int    ioshl1;      //!< 6 stress component flag for shells
    int    ioshl2;      //!< plastic strain flag for shells
    int    ioshl3;      //!< shell force resultant flag
    int    ioshl4;      //!< shell thickness, energy + 2 more
    int    UnUsed4[16];  //!< blank (unused)
  };

public:
  static vtkLsDynaBinaryPlotReader *New();
  vtkTypeRevisionMacro(vtkLsDynaBinaryPlotReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the file name of the data file to read.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Get/Set the timestep to be read
  // TODO: implement multiblock dataset reading
  vtkGetMacro(RequestedTimeStep, int);
  vtkSetMacro(RequestedTimeStep, int);

  // Get/Set if deformation shall be applied to the point data
  // If enabled, then extracted mesh will be deformed.
  // If disabled, the deformation data is stored in the point data, but the original point coordinates are kept.
  vtkSetMacro(ApplyDeformationToPoints,int);
  vtkGetMacro(ApplyDeformationToPoints,int);
  vtkBooleanMacro(ApplyDeformationToPoints,int);

  vtkGetStringMacro(ArrayNameTime);
  vtkGetStringMacro(ArrayNameMaterial);
  vtkGetStringMacro(ArrayNameDisplacement);
  vtkGetStringMacro(ArrayNameVelocity);
  vtkGetStringMacro(ArrayNameAcceleration);
  vtkGetStringMacro(ArrayNameTemperature);
  vtkSetStringMacro(ArrayNameTime);
  vtkSetStringMacro(ArrayNameMaterial);
  vtkSetStringMacro(ArrayNameDisplacement);
  vtkSetStringMacro(ArrayNameVelocity);
  vtkSetStringMacro(ArrayNameAcceleration);
  vtkSetStringMacro(ArrayNameTemperature);

 
protected:
  vtkLsDynaBinaryPlotReader();
  ~vtkLsDynaBinaryPlotReader();
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  char *FileName;
  int RequestedTimeStep;
  int NumberOfTimeSteps;
  int ApplyDeformationToPoints;

  char* ArrayNameTime;
  char* ArrayNameMaterial;
  char* ArrayNameDisplacement;
  char* ArrayNameVelocity;
  char* ArrayNameAcceleration;
  char* ArrayNameTemperature;

private:
  void ReadNodePoints(vtkUnstructuredGrid *output, FILE* fp, const FileHeaderType &fh);
  void ReadGeometry(vtkUnstructuredGrid *output, FILE* fp, const FileHeaderType &fh);
  
  int ReadTimeStep(vtkUnstructuredGrid *output, FILE* fp, const FileHeaderType &fh);
  int SkipTimeStep(FILE* fp, const FileHeaderType &fh);

  void ReadPointFieldData(vtkUnstructuredGrid *output, FILE* fp, const FileHeaderType &fh, int numberOfComponents, const char* fieldName);
  void SkipPointFieldData(FILE* fp, const FileHeaderType &fh, int numberOfComponents);

  vtkLsDynaBinaryPlotReader(const vtkLsDynaBinaryPlotReader&);  // Not implemented.
  void operator=(const vtkLsDynaBinaryPlotReader&);  // Not implemented.
};

#endif 
