/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLDiffusionRSHVolumeDisplayNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLDiffusionRSHVolumeDisplayNode.h"
#include "vtkMRMLScene.h"

#include "vtkMRMLDiffusionRSHVolumeSliceDisplayNode.h"
#include "vtkMRMLVolumeNode.h"
#include "vtkDiffusionRSHGlyph.h"

//#include "vtkSphereSource.h"
#include "vtkSbiaSphereSource.h"

//------------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeDisplayNode* vtkMRMLDiffusionRSHVolumeDisplayNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDiffusionRSHVolumeDisplayNode");
  if(ret)
    {
    return (vtkMRMLDiffusionRSHVolumeDisplayNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDiffusionRSHVolumeDisplayNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLDiffusionRSHVolumeDisplayNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDiffusionRSHVolumeDisplayNode");
  if(ret)
    {
    return (vtkMRMLDiffusionRSHVolumeDisplayNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDiffusionRSHVolumeDisplayNode;
}

//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeDisplayNode::vtkMRMLDiffusionRSHVolumeDisplayNode()
{
 this->ScalarInvariant = vtkMRMLDiffusionRSHDisplayPropertiesNode::RSH_Power_2;
 this->RSHMathematics = vtkDiffusionRSHMathematicsSimple::New();
 this->RSHMathematicsAlpha = vtkDiffusionRSHMathematicsSimple::New();
 this->Threshold->SetInput( this->RSHMathematics->GetOutput());
 this->MapToWindowLevelColors->SetInput( this->RSHMathematics->GetOutput());

 this->DiffusionRSHGlyphFilter = vtkDiffusionRSHGlyph::New();
// vtkSphereSource *sphere = vtkSphereSource::New();
 vtkSbiaSphereSource *sphere = vtkSbiaSphereSource::New();
 sphere->Update();
 this->DiffusionRSHGlyphFilter->SetSource( sphere->GetOutput() );
 sphere->Delete();

 this->VisualizationMode = 0;
}

//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeDisplayNode::~vtkMRMLDiffusionRSHVolumeDisplayNode()
{
  this->RSHMathematics->Delete();
  this->RSHMathematicsAlpha->Delete();

  this->DiffusionRSHGlyphFilter->Delete();
}


//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " scalarInvariant=\"" << this->ScalarInvariant << "\"";

}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "scalarInvariant")) 
      {
      int scalarInvariant;
      std::stringstream ss;
      ss << attValue;
      ss >> scalarInvariant;
      this->SetScalarInvariant(scalarInvariant);
      }

    }
  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLDiffusionRSHVolumeDisplayNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLDiffusionRSHVolumeDisplayNode *node = (vtkMRMLDiffusionRSHVolumeDisplayNode *) anode;
  this->SetScalarInvariant(node->ScalarInvariant);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::PrintSelf(ostream& os, vtkIndent indent)
{

  Superclass::PrintSelf(os,indent);

  os << indent << "ScalarInvariant:             " << this->ScalarInvariant << "\n";

}

//---------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event,
                                           void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  return;
}

//-----------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::UpdateScene(vtkMRMLScene *scene)
{
   Superclass::UpdateScene(scene);

}

//-----------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::UpdateReferences()
{
  Superclass::UpdateReferences();
}


//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);
}



//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::SetImageData(vtkImageData *imageData)
{
  this->RSHMathematics->SetInput(0, imageData);
  this->RSHMathematicsAlpha->SetInput(0, imageData);
}
//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::UpdateImageDataPipeline()
{

  int operation = this->GetScalarInvariant();
  this->RSHMathematics->SetOperation(operation);
  switch (operation)
    {

  default:
      this->Threshold->SetInput( this->RSHMathematics->GetOutput());
      this->MapToWindowLevelColors->SetInput( this->RSHMathematics->GetOutput());
      this->AppendComponents->RemoveAllInputs();
      this->AppendComponents->SetInputConnection(0, this->MapToColors->GetOutput()->GetProducerPort() );
      this->AppendComponents->AddInputConnection(0, this->AlphaLogic->GetOutput()->GetProducerPort() );
      break;
    }

  Superclass::UpdateImageDataPipeline();
}

//----------------------------------------------------------------------------
std::vector< vtkMRMLGlyphableVolumeSliceDisplayNode*>  vtkMRMLDiffusionRSHVolumeDisplayNode::GetSliceGlyphDisplayNodes( vtkMRMLVolumeNode* volumeNode )
{
  std::vector< vtkMRMLGlyphableVolumeSliceDisplayNode*> nodes;
  int nnodes = volumeNode->GetNumberOfDisplayNodes();
  vtkMRMLDiffusionRSHVolumeSliceDisplayNode *node = NULL;
  for (int n=0; n<nnodes; n++)
    {
    node = vtkMRMLDiffusionRSHVolumeSliceDisplayNode::SafeDownCast(volumeNode->GetNthDisplayNode(n));
    if (node)
      {
      nodes.push_back( node );
      }
    }
  return nodes;
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeDisplayNode::AddSliceGlyphDisplayNodes( vtkMRMLVolumeNode* volumeNode )
{
  std::cout << "vtkMRMLDiffusionRSHVolumeDisplayNode " << (void *) this << std::endl;
  std::cout << "vtkMRMLDiffusionRSHVolumeDisplayNode " << (void *) volumeNode << std::endl;

  std::vector< vtkMRMLGlyphableVolumeSliceDisplayNode*> nodes = this->GetSliceGlyphDisplayNodes( volumeNode );
  if (nodes.size() == 0)
    {
    vtkMRMLDiffusionRSHDisplayPropertiesNode *glyphDTDPN = vtkMRMLDiffusionRSHDisplayPropertiesNode::New();
    this->GetScene()->AddNode(glyphDTDPN);
    int modifyState = glyphDTDPN->StartModify();
    glyphDTDPN->EndModify(modifyState);
    glyphDTDPN->Delete();
    
    for (int i=0; i<3; i++)
      {
      if (this->GetScene())
        {
        vtkMRMLDiffusionRSHVolumeSliceDisplayNode *node = vtkMRMLDiffusionRSHVolumeSliceDisplayNode::New();
        if (i == 0) 
          {
          node->SetName("Red");
          }
        else if (i == 1) 
          {
          node->SetName("Yellow");
          }
        else if (i == 2) 
          {
          node->SetName("Green");
          }

        this->GetScene()->AddNode(node);
        node->Delete();

        int modifyState = node->StartModify();     
        node->SetVisibility(0);

        node->SetAndObserveDiffusionRSHDisplayPropertiesNodeID(glyphDTDPN->GetID());

        node->SetAndObserveColorNodeID("vtkMRMLColorTableNodeRainbow");
        
        node->EndModify(modifyState);

        volumeNode->AddAndObserveDisplayNodeID(node->GetID());
        
        }
      }
   }
}

//----------------------------------------------------------------------------
vtkImageData* vtkMRMLDiffusionRSHVolumeDisplayNode::GetImageData()
{
  this->UpdateImageDataPipeline();
  if (this->RSHMathematics->GetInput() == NULL)
    {
    return NULL;
    }
  this->AppendComponents->Update();
  return this->AppendComponents->GetOutput();
};
