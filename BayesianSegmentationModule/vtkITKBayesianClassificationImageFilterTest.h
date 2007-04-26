// Disable warning for long symbol names in this file only
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

//
// VTK headers
//
#include "vtkPNGReader.h"
#include "vtkPNGWriter.h"
#include "vtkITKBayesianClassificationImageFilter.h"
