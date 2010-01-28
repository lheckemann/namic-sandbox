/*
 */

#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"

#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"

#include "vtkDataSet.h"
#include "vtkFieldData.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"

#include <fstream>
#include <vector>
#include <algorithm>

#define N_SURF_VERTICES 1026

int main(int argc, char** argv){

  if(argc<3){
    std::cerr << "Usage: " << argv[0] << " input_volume output_volume" << std::endl;
    return -1;
  }

  vtkUnstructuredGrid *vol;
  vtkUnstructuredGridReader *volReader = vtkUnstructuredGridReader::New();
  volReader->SetFileName(argv[1]);
  volReader->Update();
  vol = volReader->GetOutput();

  vtkDataArray *meshDeformation = vol->GetPointData()->GetArray("ResultDisplacement");

  vtkPoints *volPts = vol->GetPoints();

  for(int i=0;i<vol->GetNumberOfPoints();i++){
    
    double displ[3], pt[3];
      
    memcpy(&displ[0], meshDeformation->GetTuple3(i), sizeof(double)*3);
    memcpy(&pt[0], volPts->GetPoint(i), sizeof(double)*3);

    pt[0] = pt[0]+displ[0];
    pt[1] = pt[1]+displ[1];
    pt[2] = pt[2]+displ[2];

    std::cout << pt[0] << " " << pt[1] << " " << pt[2] << " --> ";
    std::cout << displ[0] << " " << displ[1] << " " << displ[2] << std::endl;

    displ[0] *= -1;
    displ[1] *= -1;
    displ[2] *= -1;

    volPts->SetPoint(i, pt[0], pt[1], pt[2]);
    meshDeformation->SetTuple3(i,displ[0],displ[1],displ[2]);
  }

  vol->SetPoints(volPts);
  
  vtkUnstructuredGridWriter *volWriter = vtkUnstructuredGridWriter::New();
  volWriter->SetFileName(argv[2]);
  volWriter->SetInput(vol);
  volWriter->Update();

  /*
//  vtkPolyDataWriter *surfWriter = vtkPolyDataWriter::New();
//  surfWriter->SetFileName(argv[3]);
  vtkUnstructuredGridWriter *volWriter = vtkUnstructuredGridWriter::New();
  volWriter->SetFileName(argv[3]);

  std::ofstream displFile(argv[4]);
  std::ofstream loadedFile(argv[5]);

  vtkFloatArray *displ = vtkFloatArray::New();
  displ->SetNumberOfComponents(3);
  displ->SetNumberOfTuples(N_SURF_VERTICES);
  displ->SetName("Displacement");
  vol->GetPointData()->AddArray(displ);
  displ->Delete();

  int i, fixedNode = -1;
  float magnMin = 100, magnMax = 0;
  
  loadedFile << "*NSET, NSET=Loaded_Nodes" << std::endl;

  for(i=0;i<N_SURF_VERTICES;i++){
    double surfPt[3], volPt[3], displVal[3];
    loadedFile << " " << i+1;
    if(i<N_SURF_VERTICES-1)
      loadedFile << ",";
    if(i%8==0 && i)
      loadedFile << std::endl;
    surf->GetPoint(i, &surfPt[0]);
    vol->GetPoint(i, &volPt[0]);
    displVal[0] = surfPt[0]-volPt[0];
    displVal[1] = surfPt[1]-volPt[1];
    displVal[2] = surfPt[2]-volPt[2];

    float magn = sqrt(displVal[0]*displVal[0]+displVal[1]*displVal[1]+displVal[2]*displVal[2]);
    displFile << i+1 << " " << displVal[0] << " " << displVal[1] << " " << displVal[2] << std::endl;
    if(magnMin>magn){
      magnMin = magn;
      fixedNode = i;
    }
    if(magnMax<magn)
      magnMax = magn;

    displ->InsertTuple3(i, displVal[0], displVal[1], displVal[2]);
  }

  for(i=N_SURF_VERTICES;i<vol->GetNumberOfPoints();i++)
    displ->InsertTuple3(i,0,0,0);

  loadedFile << std::endl << "*NSET, NSET=Fixed_Nodes" << std::endl << fixedNode+1 << std::endl;

  std::cout << std::endl;

  std::cout << "Min displ: " << magnMin << ", Max displ: " << magnMax << std::endl;
  std::cout << "Fixed node: " << fixedNode << std::endl;

  volWriter->SetInput(vol);
  volWriter->Update();
  */
  return 0;
}
