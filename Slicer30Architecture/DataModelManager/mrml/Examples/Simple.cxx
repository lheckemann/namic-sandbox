/* Simple example to show typical use of mrml -> vtkmrml -> vtk -> vtkmrml -> mrml 
 * transitions */

#include "mrmlVolumeNode.h"
#include "mrmlVolume.h"
#include "mrmlScene.h"
#include "vtkmrmlVolume.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageData.h"

int main (int argc, char *argv[])
{
  const char *url = "file://data.xml";
  if( argc > 1 )
    {
    url = argv[1];
    }
  // get mrml tree
  mrml::Scene::Pointer mrml = mrml::Scene::New();
  //std::cerr << "URL:" << url << std::endl;
  mrml->SetURL(url);
  mrml->Connect();
  mrml->Print(std::cout);

  // get input image in vtk format
  mrml::Node *node = mrml->GetNthNode(0); // GetNthVolume

  mrml::vtkVolume::Pointer inData = mrml::vtkVolume::New();

  mrml::VolumeNode *volNode = dynamic_cast<mrml::VolumeNode*>(node);

  inData->SetSourceNode(volNode);
  vtkImageData *imgData = inData->GetImageData(); // converts data from internal format to vtk

  // vtk pipeline
  vtkImageGaussianSmooth *igs = vtkImageGaussianSmooth::New();
  igs->SetInput(imgData);
  igs->GetOutput()->Update();

  // put output volume in a new mrml volume node
  mrml::VolumeNode::Pointer volNodeOut = mrml::VolumeNode::New();

  mrml::vtkVolume::Pointer outData = mrml::vtkVolume::New();

  outData->SetTargetNode(volNodeOut);
  outData->SetSourceImage(igs->GetOutput()); 
  //outData->Update();   // converts data fom vtkImageData into internal format

  // add node to the mrml tree
  mrml->AddNode(volNodeOut);

  // save new file
  //mrml->Commit("file://data1.xml");
  mrml->Commit("data1.xml");

  igs->Delete();
  // Smart pointer don't need to be deleted
  return 0;
}
