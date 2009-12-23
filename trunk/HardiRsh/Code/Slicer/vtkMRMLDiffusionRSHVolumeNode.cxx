/*=auto=========================================================================

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLVolumeNode.cxx,v $
Date:      $Date: 2006/03/17 17:01:53 $
Version:   $Revision: 1.14 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLDiffusionRSHVolumeNode.h"
#include "vtkMRMLDiffusionRSHVolumeDisplayNode.h"
#include "vtkMRMLScene.h"

//------------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeNode* vtkMRMLDiffusionRSHVolumeNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDiffusionRSHVolumeNode");
  if(ret)
    {
    return (vtkMRMLDiffusionRSHVolumeNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDiffusionRSHVolumeNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLDiffusionRSHVolumeNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDiffusionRSHVolumeNode");
  if(ret)
    {
    return (vtkMRMLDiffusionRSHVolumeNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDiffusionRSHVolumeNode;
}

//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeNode::vtkMRMLDiffusionRSHVolumeNode()
{
  this->Order = 2; //Second order Tensor
}
//----------------------------------------------------------------------------

void vtkMRMLDiffusionRSHVolumeNode::SetAndObserveDisplayNodeID(const char *displayNodeID)
{
  if (!this->GetScene())
    {
    vtkWarningMacro("SetAndObserveDisplayNodeID: no scene set, cannot observe a display node");
    return;
    }
  if (displayNodeID == NULL)
    {
    vtkDebugMacro("SetAndObserveDisplayNodeID: null display node id");
    return;
    }
  if (vtkMRMLDiffusionRSHVolumeDisplayNode::SafeDownCast(this->GetScene()->GetNodeByID(displayNodeID))!=NULL)
    {
    Superclass::SetAndObserveDisplayNodeID(displayNodeID);
    }
  else
    {
    vtkErrorMacro("SetAndObserveDisplayNodeID: The node to display " << displayNodeID << " can not display RSHs");
    }
}

//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeNode::~vtkMRMLDiffusionRSHVolumeNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeNode::ReadXMLAttributes(const char** atts)
{

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);
  }

}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLDiffusionRSHVolumeNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  //vtkMRMLDiffusionRSHVolumeNode *node = (vtkMRMLDiffusionRSHVolumeNode *) anode;
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID,newID);
}

//-----------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeNode::UpdateReferences()
{
  Superclass::UpdateReferences();
}

//---------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeNode::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event,
                                           void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


 
