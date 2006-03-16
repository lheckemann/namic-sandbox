/* Simple example to show typical use of mrml -> vtkmrml -> vtk -> vtkmrml -> mrml 
 * transitions */

#include "mrmlVolumeNode.h"
#include "mrmlVolume.h"
#include "mrmlScene.h"
#include "vtkmrmlVolume.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageData.h"

int main (int /*argc*/, char * /*argv*/[])
{
  // get mrml tree
  mrml::Scene *mrml = mrml::Scene::New();
  mrml->SetURL("file://data.xml");
  mrml->Connect();

  // get input image in vtk format
  mrml::Node *node = mrml->GetNthNode(0); // GetNthVolume

  mrml::vtkVolume *inData = mrml::vtkVolume::New();

  mrml::VolumeNode *volNode = dynamic_cast<mrml::VolumeNode*>(node);

  inData->SetSourceNode(volNode);
  vtkImageData *imgData = inData->GetImageData(); // converts data from internal format to vtk

  // vtk pipeline
  vtkImageGaussianSmooth *igs = vtkImageGaussianSmooth::New();
  igs->SetInput(imgData);
  igs->GetOutput()->Update();

  // put output volume in a new mrml volume node
  mrml::VolumeNode *volNodeOut = mrml::VolumeNode::New();

  mrml::vtkVolume *outData = mrml::vtkVolume::New();

  outData->SetTargetNode(volNodeOut);
  outData->SetSourceImage(igs->GetOutput()); 
  //outData->Update();   // converts data fom vtkImageData into internal format

  // add node to the mrml tree
  mrml->AddNode(volNodeOut);

  // save new file
  mrml->Commit("file://data1.xml");

  // Smart pointer don't need to be deleted
  return 0;
}
