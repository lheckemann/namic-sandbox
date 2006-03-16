#include "mrmlObject.h"
#include "mrmlCommand.h"
#include "mrmlCreateObjectFunction.h"
#include "mrmlEventObject.h"
#include "mrmlFastMutexLock.h"
#include "mrmlVolume.h"
#include "mrmlIndent.h"
#include "mrmlLightObject.h"
#include "mrmlMacro.h"
#include "mrmlModel.h"
#include "mrmlModelNode.h"
#include "mrmlModelStorageNode.h"
#include "mrmlNode.h"
#include "mrmlObjectFactoryBase.h"
#include "mrmlObjectFactory.h"
#include "mrmlObject.h"
#include "mrmlParser.h"
#include "mrmlScene.h"
#include "mrmlSimpleFastMutexLock.h"
#include "mrmlStorageNode.h"
#include "mrmlTimeStamp.h"
#include "mrmlTransform.h"
#include "mrmlVersion.h"
#include "mrmlVolumeArchetypeStorageNode.h"
#include "mrmlVolumeNode.h"

int testInstance(int , char *[])
{
  // Declare a pointer
  mrml::Object::Pointer o;
  // code should seg fault:
  //o->Print();

  mrml::Object::Pointer oo = mrml::Object::New();
  oo->Print();

  mrml::FastMutexLock::Pointer p2 = mrml::FastMutexLock::New();
  mrml::Volume::Pointer p3 = mrml::Volume::New();
  mrml::LightObject::Pointer p5 = mrml::LightObject::New();
  mrml::Model::Pointer p6 = mrml::Model::New();
  mrml::ModelNode::Pointer p7 = mrml::ModelNode::New();
  mrml::ModelStorageNode::Pointer p8 = mrml::ModelStorageNode::New();
  mrml::Object::Pointer               p10 =      mrml::Object::New();
  mrml::Parser::Pointer                   p11 =  mrml::Parser::New();
  mrml::Scene::Pointer                     p12 = mrml::Scene::New();
  mrml::Transform::Pointer                 p15 = mrml::Transform::New();
  mrml::VolumeArchetypeStorageNode::Pointer p16 = mrml::VolumeArchetypeStorageNode::New();
  mrml::VolumeNode::Pointer                p17 = mrml::VolumeNode::New();

  return 0;
}


