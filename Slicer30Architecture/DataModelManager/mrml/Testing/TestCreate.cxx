#include "mrmlScene.h"
#include "mrmlVolumeNode.h"

int TestCreate(int /*argc*/, char * /*argv*/[])
{
  mrml::Scene::Pointer sc = mrml::Scene::New();
  mrml::VolumeNode::Pointer vn = mrml::VolumeNode::New();
  sc->RegisterNodeClass (vn);
  std::string filename = 
    "/home/mathieu/Dashboards/MyTests/NAMICSandBox/trunk/Slicer30Architecture/"
    "DataModelManager/mrml/Testing/scene1.xml";
  sc->SetURL( filename.c_str() );
  sc->Connect();
/*  std::cout <<  "GetNumberOfNodesByClass";
  sc.GetNumberOfNodesByClass ( "vtkMRMLVolumeNode" );
  std::cout << "GetNodeClasses";
  sc.GetNodeClasses();
  std::cout << "GetNthNode";
  sc.GetNthNode (0)
  vn  = sc.GetNthNode (0);
  std::cout << "Print volume node";
  vn.Print();
*/
  return 0;
}

