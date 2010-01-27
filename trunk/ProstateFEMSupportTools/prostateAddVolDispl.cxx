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

  if(argc<4){
    std::cerr << "Usage: " << argv[0] << " input_volume correspondence displacements output_volume" << std::endl;
    return -1;
  }

  vtkUnstructuredGrid *vol;
  vtkUnstructuredGridReader *volReader = vtkUnstructuredGridReader::New();
  volReader->SetFileName(argv[1]);
  volReader->Update();
  vol = volReader->GetOutput();

  std::vector<int> corr(vol->GetNumberOfPoints());
  std::vector<float> displ(3*vol->GetNumberOfPoints());

  std::cout << "Correspondence file: " << argv[2] << std::endl;
  std::cout << "Displacements file: " << argv[3] << std::endl;

  std::ifstream corrReader(argv[2]);
  std::ifstream displReader(argv[3]);

  int i = 0;
  for(i=0;i<vol->GetNumberOfPoints();i++){
    corrReader >> corr[i];
    corr[i] = corr[i]-1;
  }

  i = 0;
  for(i=0;i<vol->GetNumberOfPoints();i++){
    displReader >> displ[i*3] >> displ[i*3+1] >> displ[i*3+2];
  }

  vtkFloatArray *displArray = vtkFloatArray::New();
  displArray->SetNumberOfComponents(3);
  displArray->SetNumberOfTuples(vol->GetNumberOfPoints());
  displArray->SetName("ResultDisplacement");
  vol->GetPointData()->AddArray(displArray);
  displArray->Delete();
 
  for(i=0;i<vol->GetNumberOfPoints();i++){
    float ptDispl[3];

//    ptDispl[0] = displ[pos*3];
//    ptDispl[1] = displ[pos*3+1];
//    ptDispl[2] = displ[pos*3+2];

    ptDispl[0] = displ[i*3];
    ptDispl[1] = displ[i*3+1];
    ptDispl[2] = displ[i*3+2];

    displArray->InsertTuple3(corr[i],ptDispl[0], ptDispl[1], ptDispl[2]);
  }

  
  vtkUnstructuredGridWriter *volWriter = vtkUnstructuredGridWriter::New();
  volWriter->SetFileName(argv[4]);
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
