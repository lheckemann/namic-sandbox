#include "mrmlScene.h"
#include "mrmlNode.h"

// This file tests creation of a MRML tree containing all node types

int TestAllMrml(int argc, char *argv[])
{
  // Please add all nodes to the list (not vtkMrmlNode)
  const char * const nodeTypeList[] =
    {
    "ModelNode",
    "ModelStorageNode",
    //  "Node",
    "StorageNode",
    "VolumeArchetypeStorageNode",
    "VolumeNode",
    NULL 
    };

  // Create a Scene with all nodes
  mrml::Scene::Pointer scene = mrml::Scene::New();

  const char *nodetype;
  for(nodetype = *nodeTypeList; nodetype != NULL; ++nodetype)
    {
    // unique name
    // Create the node
    mrml::LightObject::Pointer name = mrml::ObjectFactoryBase::CreateInstance( nodetype );
    mrml::Node::Pointer node = dynamic_cast<mrml::Node*>(name.GetPointer());
    node->Print();

    // Note the tree may not have proper syntax, this is just a test
    // Put item on the tree
    scene->AddNode( node );
    }

  // Now we can test individual things about the nodes
  //  examplevtkMrmlVolumeNode SetDescription "hello!!!!!!!"


  // Now write the file
  const char filename[] = "test.xml";
  scene->Commit( filename );
  std::cout << "MRML file saved as "  << filename  << std::endl;

  return 0;
}
