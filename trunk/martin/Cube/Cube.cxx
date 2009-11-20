#include <iostream>
#include "CubeCLP.h"
#include "vtkCubeSource.h"
#include "vtkXMLPolyDataWriter.h"

#include "vtkPluginFilterWatcher.h"

int main(int argc, char * argv [])
{
  PARSE_ARGS;
  std::cout << "--------------------Displaying Cube--------------------" << std::endl;
  
  //the parameters of the cube
  double bounds[6] = {-36,-13.1875, -71.5, -26.5, -70.5, 0.5};
  
  //creats a cube
  vtkCubeSource * cubeSource = vtkCubeSource::New();
  // set the cube parameters
  cubeSource->SetBounds(bounds);
      
  std::cout << "Cube created: " << bounds[0] << " " << bounds[1] << " " << bounds[2] << " " << bounds[3] << " " << bounds[4] << " " << bounds[5] << std::endl;

  //shows the cube in the 3d layout
  vtkXMLPolyDataWriter * writer = NULL;
  writer = vtkXMLPolyDataWriter::New();
  
  vtkPluginFilterWatcher watchWriter(writer, "Writing the cube", CLPProcessInformation, 1.0/7.0, 0.0);
  
  writer->SetInput(cubeSource->GetOutput());
  writer->SetFileName(MyOutputGeometry.c_str());

  writer->Write();

  cubeSource->Delete();
  writer->Delete();

  return EXIT_SUCCESS;
}
