/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLLinearTransformNode.cxx,v $
Date:      $Date: 2006/03/17 17:01:53 $
Version:   $Revision: 1.14 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkLinearTransform.h"

#include "vtkMRML4DBundleNode.h"
#include "vtkMRMLScene.h"

//------------------------------------------------------------------------------
vtkMRML4DBundleNode* vtkMRML4DBundleNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRML4DBundleNode");
  if(ret)
    {
    return (vtkMRML4DBundleNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRML4DBundleNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRML4DBundleNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRML4DBundleNode");
  if(ret)
    {
    return (vtkMRML4DBundleNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRML4DBundleNode;
}

//----------------------------------------------------------------------------
vtkMRML4DBundleNode::vtkMRML4DBundleNode()
{
  this->MatrixTransformToParent = NULL;

  vtkMatrix4x4 *matrix  = vtkMatrix4x4::New();
  matrix->Identity();
  this->SetAndObserveMatrixTransformToParent(matrix);
  matrix->Delete();

  this->FrameNodeIDList.clear();
  this->TransformNodeIDList.clear();
  this->CurrentFrameNodeIndex = -1;
}

//----------------------------------------------------------------------------
vtkMRML4DBundleNode::~vtkMRML4DBundleNode()
{
  if (this->MatrixTransformToParent) 
    {
    this->SetAndObserveMatrixTransformToParent(NULL);
    }

  this->FrameNodeIDList.clear();
  this->TransformNodeIDList.clear();
}

//----------------------------------------------------------------------------
void vtkMRML4DBundleNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  if (this->MatrixTransformToParent != NULL) 
    {
    std::stringstream ss;
    for (int row=0; row<4; row++) 
      {
      for (int col=0; col<4; col++) 
        {
        ss << this->MatrixTransformToParent->GetElement(row, col);
        if (!(row==3 && col==3)) 
          {
          ss << " ";
          }
        }
      if ( row != 3 )
        {
        ss << " ";
        }
      }
    of << indent << " matrixTransformToParent=\"" << ss.str() << "\"";
    }

}

//----------------------------------------------------------------------------
void vtkMRML4DBundleNode::ReadXMLAttributes(const char** atts)
{

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "matrixTransformToParent")) 
      {
      vtkMatrix4x4 *matrix  = vtkMatrix4x4::New();
      matrix->Identity();
      if (this->MatrixTransformToParent != NULL) 
        {
        this->SetAndObserveMatrixTransformToParent(NULL);
        }
      std::stringstream ss;
      double val;
      ss << attValue;
      for (int row=0; row<4; row++) 
        {
        for (int col=0; col<4; col++) 
          {
          ss >> val;
          matrix->SetElement(row, col, val);
          }
        }
      this->SetAndObserveMatrixTransformToParent(matrix);
      matrix->Delete();
      }
    }  
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRML4DBundleNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRML4DBundleNode *node = (vtkMRML4DBundleNode *) anode;
  for (int i=0; i<4; i++) 
    {
    for (int j=0; j<4; j++)
      {
      this->GetMatrixTransformToParent()->SetElement(i,j,(node->MatrixTransformToParent->GetElement(i,j)));
      }
    }
}

//----------------------------------------------------------------------------
void vtkMRML4DBundleNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  if (this->MatrixTransformToParent != NULL) 
    {
    os << indent << "MatrixTransformToParent: " << "\n";
    for (int row=0; row<4; row++) 
      {
      for (int col=0; col<4; col++) 
        {
        os << this->MatrixTransformToParent->GetElement(row, col);
        if (!(row==3 && col==3)) 
          {
          os << " ";
          }
        else 
          {
          os << "\n";
          }
        } // for (int col
      } // for (int row
    }
}

//----------------------------------------------------------------------------
vtkGeneralTransform* vtkMRML4DBundleNode::GetTransformToParent()
{
  this->TransformToParent->Identity();
  this->TransformToParent->Concatenate(this->MatrixTransformToParent);
  return this->TransformToParent;
}

//----------------------------------------------------------------------------
int  vtkMRML4DBundleNode::GetMatrixTransformToWorld(vtkMatrix4x4* transformToWorld)
{
  if (this->IsTransformToWorldLinear() != 1) 
    {
    transformToWorld->Identity();
    return 0;
    }

  vtkMatrix4x4 *xform = vtkMatrix4x4::New();
  xform->DeepCopy(transformToWorld);
  vtkMatrix4x4::Multiply4x4(xform, this->MatrixTransformToParent, transformToWorld);
  xform->Delete();

  vtkMRMLTransformNode *parent = this->GetParentTransformNode();
  if (parent != NULL) 
    {
    vtkMRML4DBundleNode *lparent = dynamic_cast < vtkMRML4DBundleNode* > (parent);
    if (lparent) 
      {
      return (lparent->GetMatrixTransformToWorld(transformToWorld));
      }
    }
  // TODO: what does this return code mean?
  return 1;
}

//----------------------------------------------------------------------------
int  vtkMRML4DBundleNode::GetMatrixTransformToNode(vtkMRMLTransformNode* node,
                                                          vtkMatrix4x4* transformToNode)
{
  if (node == NULL) 
    {
    this->GetMatrixTransformToWorld(transformToNode);
    return 1;
    }
  if (this->IsTransformToNodeLinear(node) != 1) 
    {
    transformToNode->Identity();
    return 0;
    }
  
  
  if (this->IsTransformNodeMyParent(node)) 
    {
    vtkMRMLTransformNode *parent = this->GetParentTransformNode();
    if (parent != NULL) 
      {

      vtkMatrix4x4 *xform = vtkMatrix4x4::New();
      xform->DeepCopy(transformToNode);
      vtkMatrix4x4::Multiply4x4(xform, this->MatrixTransformToParent, transformToNode);
      xform->Delete();

      if (strcmp(parent->GetID(), node->GetID()) ) 
        {
        this->GetMatrixTransformToNode(node, transformToNode);
        }
      }
    else if (this->MatrixTransformToParent) 
      {
      vtkMatrix4x4 *xform = vtkMatrix4x4::New();
      xform->DeepCopy(transformToNode);
      vtkMatrix4x4::Multiply4x4(xform, this->MatrixTransformToParent, transformToNode);
      xform->Delete();
      }
    }
  else if (this->IsTransformNodeMyChild(node)) 
    {
    vtkMRML4DBundleNode *lnode = dynamic_cast <vtkMRML4DBundleNode *> (node);
    vtkMRML4DBundleNode *parent = dynamic_cast <vtkMRML4DBundleNode *> (node->GetParentTransformNode());
    if (parent != NULL) 
      {

      vtkMatrix4x4 *xform = vtkMatrix4x4::New();
      xform->DeepCopy(transformToNode);
      vtkMatrix4x4::Multiply4x4(xform, lnode->MatrixTransformToParent, transformToNode);
      xform->Delete();

      if (strcmp(parent->GetID(), this->GetID()) ) 
        {
        this->GetMatrixTransformToNode(this, transformToNode);
        }
      }
    else if (lnode->MatrixTransformToParent) 
      {
      vtkMatrix4x4 *xform = vtkMatrix4x4::New();
      xform->DeepCopy(transformToNode);
      vtkMatrix4x4::Multiply4x4(xform, lnode->MatrixTransformToParent, transformToNode);
      xform->Delete();
      }
    }
  else 
    {
    this->GetMatrixTransformToWorld(transformToNode);
    vtkMatrix4x4* transformToWorld2 = vtkMatrix4x4::New();
    transformToWorld2->Identity();
    
    node->GetMatrixTransformToWorld(transformToWorld2);
    transformToWorld2->Invert();
    
    vtkMatrix4x4 *xform = vtkMatrix4x4::New();
    xform->DeepCopy(transformToNode);
    vtkMatrix4x4::Multiply4x4(xform, transformToWorld2, transformToNode);
    xform->Delete();
    transformToWorld2->Delete();
    }
  // TODO: what does this return code mean?
  return 1;
}

//----------------------------------------------------------------------------
void vtkMRML4DBundleNode::SetAndObserveMatrixTransformToParent(vtkMatrix4x4 *matrix)
{
  if (this->MatrixTransformToParent != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations( 
      this->MatrixTransformToParent, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    this->SetMatrixTransformToParent(NULL);
    }
  this->SetMatrixTransformToParent(matrix);
  if ( this->MatrixTransformToParent )
    {
    vtkEventBroker::GetInstance()->AddObservation( 
      this->MatrixTransformToParent, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }
}


//---------------------------------------------------------------------------
void vtkMRML4DBundleNode::ProcessMRMLEvents ( vtkObject *caller,
                                                    unsigned long event, 
                                                    void *callData )
{
  Superclass::ProcessMRMLEvents ( caller, event, callData );

  if (this->MatrixTransformToParent != NULL && this->MatrixTransformToParent == vtkMatrix4x4::SafeDownCast(caller) &&
      event ==  vtkCommand::ModifiedEvent)
    {
    this->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent, NULL);
    }
}


//----------------------------------------------------------------------------
void vtkMRML4DBundleNode::ApplyTransform(vtkMatrix4x4* transformMatrix)
{
  vtkMatrix4x4* matrixToParent = this->GetMatrixTransformToParent();
  vtkMatrix4x4* newMatrixToParent = vtkMatrix4x4::New();

  vtkMatrix4x4::Multiply4x4(matrixToParent,transformMatrix,newMatrixToParent);

  this->SetAndObserveMatrixTransformToParent(newMatrixToParent);

  newMatrixToParent->Delete();
}

//----------------------------------------------------------------------------
void vtkMRML4DBundleNode::ApplyTransform(vtkAbstractTransform* transform)
{
  if (!transform->IsA("vtkLinearTransform"))
    {
    vtkErrorMacro(<<"Can't harden a general transform in a linear transform.");
    }
  this->ApplyTransform(vtkLinearTransform::SafeDownCast(transform)->GetMatrix());
}


//----------------------------------------------------------------------------
int vtkMRML4DBundleNode::SwitchCurrentFrame(int i)
{
  
  if (i < 0 || i > this->FrameNodeIDList.size())
    {
    return -1;
    }
  
  this->CurrentFrameNodeIndex = i;
  return i;
}


//----------------------------------------------------------------------------
int vtkMRML4DBundleNode::GetNumberOfFrames()
{

  return this->FrameNodeIDList.size();

}


//----------------------------------------------------------------------------
int vtkMRML4DBundleNode::InsertFrame(int i, const char* nodeID)
{
  int index;

  if (i < 0)
    {
    index = 0;
    }
  else if (i > this->FrameNodeIDList.size())
    {
    index = this->FrameNodeIDList.size();
    }
  else
    {
    index = i;
    }

  NodeIDListType::iterator iter;
  iter = this->FrameNodeIDList.begin();
  iter += index;

  this->FrameNodeIDList.insert(iter, std::string(nodeID));

  return index;
  
}


//----------------------------------------------------------------------------
int vtkMRML4DBundleNode::AddFrame(const char* nodeID)
{
  this->FrameNodeIDList.push_back(std::string(nodeID));
}


//----------------------------------------------------------------------------
int vtkMRML4DBundleNode::RemoveFrame(int i)
{

  int index;

  if (i < 0)
    {
    index = 0;
    }
  else if (i > this->FrameNodeIDList.size())
    {
    index = this->FrameNodeIDList.size();
    }
  else
    {
    index = i;
    }

  NodeIDListType::iterator iter;
  iter = this->FrameNodeIDList.begin();
  iter += index;
  this->FrameNodeIDList.erase(iter);

  return index;

}


//----------------------------------------------------------------------------
int vtkMRML4DBundleNode::RemoveFrame(const char* nodeID)
{
  
  int index;

  NodeIDListType::iterator iter;
  for (iter = this->FrameNodeIDList.begin(); iter != this->FrameNodeIDList.end(); iter ++)
    {
    if (*iter == nodeID)
      {
      this->FrameNodeIDList.erase(iter);
      return index;
      }
    index ++;
    }

  return -1;
}



// End
