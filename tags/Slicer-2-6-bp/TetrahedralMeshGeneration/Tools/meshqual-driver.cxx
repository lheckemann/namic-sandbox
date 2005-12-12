#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkDataSet.h"
#include "vtkFieldData.h"
#include <iostream>
// this is because current VTK MeshQuality is different, and wrong
// TODO: submit this to VTK bugzilla
#include "vtkMeshQuality.h"

#include "vtkDebugLeaks.h"
#include "vtkTestUtilities.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkFieldData.h"
#include "vtkCellArray.h"

int DumpQualityStats( vtkMeshQuality* iq, const char *arrayname )
{
  double avg = iq->GetOutput()->GetFieldData()->GetArray( arrayname )->GetComponent( 0, 1 );

  cout << "  range: "
    << iq->GetOutput()->GetFieldData()->GetArray( arrayname )->GetComponent( 0, 0 )
    << "  -  "
    << iq->GetOutput()->GetFieldData()->GetArray( arrayname )->GetComponent( 0, 2 )
    << endl;
  cout << "  average: " << avg << "  , standard deviation: "
    << sqrt(fabs(iq->GetOutput()->GetFieldData()->GetArray( arrayname )->GetComponent( 0, 3 ) - avg * avg))
    << endl;

  return 0;
}

int main(int argc, char **argv){
  if(argc<2)
    return -1;
  
  vtkUnstructuredGridReader *vtk_mesh_reader = 
    vtkUnstructuredGridReader::New();
  vtkUnstructuredGrid* ug;
  vtkMeshQuality *iq = vtkMeshQuality::New();
  vtkDataObject *output_mesh;
  vtkCellArray *cells;
  
  vtk_mesh_reader->SetFileName(argv[1]);
  vtk_mesh_reader->Update();
  cout << "Mesh read" << endl;
  ug = vtk_mesh_reader->GetOutput();
  cells = ug->GetCells();
  cout << "Mesh size: " << cells->GetSize()/5 << endl;
  iq->SaveCellQualityOn();
  iq->SetInput(ug);
  iq->Update();
  cout << "Quality assessed" << endl;
 
  iq->SetTetQualityMeasureToEdgeRatio();
  iq->Update();
  cout << " Edge Ratio:"
    << endl;
  DumpQualityStats( iq, "Mesh Tetrahedron Quality" );

  iq->SetTetQualityMeasureToAspectRatio();
  iq->Update();
  cout << " Aspect Ratio:"
    << endl;
  DumpQualityStats( iq, "Mesh Tetrahedron Quality" );

  iq->SetTetQualityMeasureToRadiusRatio();
  iq->Update();
  cout << " Radius Ratio:"
    << endl;
  DumpQualityStats( iq, "Mesh Tetrahedron Quality" );

  iq->SetTetQualityMeasureToFrobeniusNorm();
  iq->Update();
  cout << " Frobenius Norm:"
    << endl;
  DumpQualityStats( iq, "Mesh Tetrahedron Quality" );

  iq->SetTetQualityMeasureToMinAngle();
  iq->Update();
  cout << " Minimal Dihedral Angle:"
    << endl;
  DumpQualityStats( iq, "Mesh Tetrahedron Quality" );
  
  /*
  iq->SetTetQualityMeasureToFerrantAR();
  iq->Update();
  cout << " Ferrant Aspect Ratio:"
    << endl;
  DumpQualityStats( iq, "Mesh Tetrahedron Quality" );
  cout << endl; 
  */

  return 0;
}
