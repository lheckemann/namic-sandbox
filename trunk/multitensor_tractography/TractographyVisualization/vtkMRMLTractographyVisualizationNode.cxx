#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkMRMLTractographyVisualizationNode.h"
#include "vtkMRMLScene.h"


//------------------------------------------------------------------------------
vtkMRMLTractographyVisualizationNode*
    vtkMRMLTractographyVisualizationNode::New()
{
  // First try to create the object from the vtkObjectFactory.
  vtkObject* ret =
      vtkObjectFactory::CreateInstance("vtkMRMLTractographyVisualizationNode");
  if(ret)
  {
    return (vtkMRMLTractographyVisualizationNode*)ret;
  }

  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTractographyVisualizationNode;
}


//------------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLTractographyVisualizationNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory.
  vtkObject* ret =
      vtkObjectFactory::CreateInstance("vtkMRMLTractographyVisualizationNode");
  if(ret)
  {
    return (vtkMRMLTractographyVisualizationNode*)ret;
  }

  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTractographyVisualizationNode;
}


//------------------------------------------------------------------------------
vtkMRMLTractographyVisualizationNode::vtkMRMLTractographyVisualizationNode()
{
  this->Threshold = 0.0;
  this->ThresholdMin = 0.0;
  this->ThresholdMax = 100.0;

  this->InputModelRef = NULL;
  this->OutputModelRef = NULL;

  this->ValueType = 0;
  this->RemovalMode = 0;
  this->ShowTubes = false;
}


//------------------------------------------------------------------------------
vtkMRMLTractographyVisualizationNode::~vtkMRMLTractographyVisualizationNode()
{
  this->SetInputModelRef(NULL);
  this->SetOutputModelRef(NULL);
}


//------------------------------------------------------------------------------
void vtkMRMLTractographyVisualizationNode::WriteXML(ostream& of, int nIndent)
{
//  Superclass::WriteXML(of, nIndent);
//
//  // Write all MRML node attributes into output stream
//  vtkIndent indent(nIndent);
//  {
//    std::stringstream ss;
//    ss << this->Threshold;
//    of << indent << " Threshold=\"" << ss.str() << "\"";
//  }
//  {
//    std::stringstream ss;
//    if (this->InputModelRef)
//    {
//      ss << this->InputModelRef;
//      of << indent << " InputModelRef=\"" << ss.str() << "\"";
//    }
//  }
//  {
//    std::stringstream ss;
//    if (this->OutputModelRef)
//    {
//      ss << this->OutputModelRef;
//      of << indent << " OutputModelRef=\"" << ss.str() << "\"";
//    }
//  }
//  {
//    std::stringstream ss;
//    ss << this->ThresholdMin;
//    of << indent << " ThresholdMin=\"" << ss.str() << "\"";
//  }
//  {
//    std::stringstream ss;
//    ss << this->ThresholdMax;
//    of << indent << " ThresholdMax=\"" << ss.str() << "\"";
//  }
//  {
//    std::stringstream ss;
//    ss << this->ValueType;
//    of << indent << " ValueType=\"" << ss.str() << "\"";
//  }
//  {
//    std::stringstream ss;
//    ss << this->RemovalMode;
//    of << indent << " RemovalMode=\"" << ss.str() << "\"";
//  }
//  {
//    std::stringstream ss;
//    ss << this->ShowTubes;
//    of << indent << " ShowTubes=\"" << ss.str() << "\"";
//  }
}


//------------------------------------------------------------------------------
void vtkMRMLTractographyVisualizationNode::ReadXMLAttributes(
    const char** atts)
{
//  vtkMRMLNode::ReadXMLAttributes(atts);
//
//  // Read all MRML node attributes from two arrays of names and values
//  const char* attName;
//  const char* attValue;
//  while (*atts != NULL) 
//  {
//    attName = *(atts++);
//    attValue = *(atts++);
//    if (!strcmp(attName, "Threshold")) 
//    {
//      std::stringstream ss;
//      ss << attValue;
//      ss >> this->Threshold;
//    }
//    if (!strcmp(attName, "ThresholdMin")) 
//    {
//      std::stringstream ss;
//      ss << attValue;
//      ss >> this->ThresholdMin;
//    }
//    if (!strcmp(attName, "ThresholdMax")) 
//    {
//      std::stringstream ss;
//      ss << attValue;
//      ss >> this->ThresholdMax;
//    }
//    else if (!strcmp(attName, "ValueType")) 
//    {
//      std::stringstream ss;
//      ss << attValue;
//      ss >> this->ValueType;
//    }
//    else if (!strcmp(attName, "RemovalMode")) 
//    {
//      std::stringstream ss;
//      ss << attValue;
//      ss >> this->RemovalMode;
//    }
//    else if (!strcmp(attName, "ShowTubes")) 
//    {
//      std::stringstream ss;
//      ss << attValue;
//      ss >> this->ShowTubes;
//    }
//    else if (!strcmp(attName, "InputModelRef"))
//    {
//      this->SetInputModelRef(attValue);
//      this->Scene->AddReferencedNodeID(this->InputModelRef, this);
//    }
//    else if (!strcmp(attName, "OutputModelRef"))
//    {
//      this->SetOutputModelRef(attValue);
//      this->Scene->AddReferencedNodeID(this->OutputModelRef, this);
//    }
//  }
}


//------------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID.
void vtkMRMLTractographyVisualizationNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLTractographyVisualizationNode *node =
      (vtkMRMLTractographyVisualizationNode *)anode;

  this->SetInputModelRef(node->InputModelRef);
  this->SetOutputModelRef(node->OutputModelRef);

  this->SetThreshold(node->Threshold);
  this->SetThresholdMin(node->ThresholdMin);
  this->SetThresholdMax(node->ThresholdMax);

  this->SetValueType(node->ValueType);
  this->SetRemovalMode(node->RemovalMode);
  this->SetShowTubes(node->ShowTubes);
}


//------------------------------------------------------------------------------
void vtkMRMLTractographyVisualizationNode::PrintSelf(ostream& os,
                                                     vtkIndent indent)
{
  vtkstd::cout << "print self not implemented yet" << vtkstd::endl;
}


//------------------------------------------------------------------------------
void vtkMRMLTractographyVisualizationNode::UpdateReferenceID(const char *oldID,
                                                             const char *newID)
{
  if (!strcmp(oldID, this->InputModelRef))
  {
    this->SetInputModelRef(newID);
  }
  if (!strcmp(oldID, this->OutputModelRef))
  {
    this->SetOutputModelRef(newID);
  }
}
