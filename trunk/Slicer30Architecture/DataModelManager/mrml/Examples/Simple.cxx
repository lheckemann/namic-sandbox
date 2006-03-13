/* Simple example to show typical use of mrml -> vtkmrml -> vtk -> vtkmrml -> mrml 
 * transitions */

#include "mrmlTree.h"
//#include "vtkmrml.h"

int main (int /*argc*/, char * /*argv*/[])
{
  // get mrml tree
  mrml::Tree *mrml = mrml::Tree::New();
  mrml->Connect("file://data.xml");

  // get input image in vtk format
  mrml::VolumeNode *volNode = mrml->GetNthVolume(0);

  vtkmrml::VolumeData *inData = vtkmrml::VolumeData::New();

  inData->SetSourceNode(volNode);
  vtkImageData *imgData = inData->GetImageData(); // converts data from internal format to vtk

  // vtk pipeline
  vtkImageGaussianSmooth *igs = vtkImageGaussianSmooth::New();
  igs->SetInput(imgData);
  igs->GetOutput()->Update();

  // put output volume in a new mrml volume node
  mrml::VolumeNode *volNodeOut = mrml::VolumeNode::New();

  vtkmrml::VolumeData *outData = vtkmrml::VolumeData::New();

  outData->SetTargetNode(volNodeOut);
  outData->SetSourceImage(igs->GetOutput()); 
  outData->Update();   // converts data fom vtkImage into internal format

  // add node to the mrml tree
  mrml->AddNode(vol);

  // save new file
  mrml->Save("file://data1.xml");

  // Smart pointer don't need to be deleted
  return 0;
}
