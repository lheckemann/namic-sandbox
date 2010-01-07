/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLDiffusionRSHVolumeSliceDisplayNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.3 $

=========================================================================auto=*/
#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkDiffusionRSHGlyph.h"

#include "vtkTransformPolyDataFilter.h"

#include "vtkMRMLDiffusionRSHVolumeSliceDisplayNode.h"
#include "vtkMRMLScene.h"

//------------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeSliceDisplayNode* vtkMRMLDiffusionRSHVolumeSliceDisplayNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDiffusionRSHVolumeSliceDisplayNode");
  if(ret)
    {
    return (vtkMRMLDiffusionRSHVolumeSliceDisplayNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDiffusionRSHVolumeSliceDisplayNode;
}

//-----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLDiffusionRSHVolumeSliceDisplayNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDiffusionRSHVolumeSliceDisplayNode");
  if(ret)
    {
    return (vtkMRMLDiffusionRSHVolumeSliceDisplayNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDiffusionRSHVolumeSliceDisplayNode;
}


//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeSliceDisplayNode::vtkMRMLDiffusionRSHVolumeSliceDisplayNode()
  :vtkMRMLGlyphableVolumeSliceDisplayNode()
{

  // Enumerated
  this->DiffusionRSHDisplayPropertiesNode = NULL;
  this->DiffusionRSHDisplayPropertiesNodeID = NULL;


  this->DiffusionRSHGlyphFilter = vtkDiffusionRSHGlyph::New();
  this->DiffusionRSHGlyphFilter->SetResolution (1);

  this->ColorMode = this->colorModeScalar;
/*  
  this->SliceToXYTransformer = vtkTransformPolyDataFilter::New();

  this->SliceToXYTransform = vtkTransform::New();

  this->SliceToXYMatrix = vtkMatrix4x4::New();
  this->SliceToXYMatrix->Identity();
  this->SliceToXYTransform->PreMultiply();
  this->SliceToXYTransform->SetMatrix(this->SliceToXYMatrix);

  this->SliceToXYTransformer->SetTransform(this->SliceToXYTransform);
*/
  this->SliceToXYTransformer->SetInput(this->DiffusionRSHGlyphFilter->GetOutput());
}


//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHVolumeSliceDisplayNode::~vtkMRMLDiffusionRSHVolumeSliceDisplayNode()
{
  this->RemoveObservers ( vtkCommand::ModifiedEvent, this->MRMLCallbackCommand );
  this->SetAndObserveDiffusionRSHDisplayPropertiesNodeID(NULL);
  this->DiffusionRSHGlyphFilter->Delete();
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::WriteXML(ostream& of, int nIndent)
{

  // Write all attributes not equal to their defaults

  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

//  of << indent << " colorMode =\"" << this->ColorMode << "\"";

  if (this->DiffusionRSHDisplayPropertiesNodeID != NULL) 
    {
    of << indent << " DiffusionRSHDisplayPropertiesNodeRef=\"" << this->DiffusionRSHDisplayPropertiesNodeID << "\"";
    }


}


//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
/*
    if (!strcmp(attName, "colorMode")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> ColorMode;
      }
    else 
*/
    if (!strcmp(attName, "DiffusionRSHDisplayPropertiesNodeRef")) 
      {
      this->SetDiffusionRSHDisplayPropertiesNodeID(attValue);
      //this->Scene->AddReferencedNodeID(this->FiberLineDiffusionRSHDisplayPropertiesNodeID, this);
      }
    }  

  this->EndModify(disabledModify);
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, ID
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);
  vtkMRMLDiffusionRSHVolumeSliceDisplayNode *node = (vtkMRMLDiffusionRSHVolumeSliceDisplayNode *) anode;

  this->SetDiffusionRSHDisplayPropertiesNodeID(node->DiffusionRSHDisplayPropertiesNodeID);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::PrintSelf(ostream& os, vtkIndent indent)
{
 //int idx;
  
  Superclass::PrintSelf(os,indent);
//  os << indent << "ColorMode:             " << this->ColorMode << "\n";
}
//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::SetSliceGlyphRotationMatrix(vtkMatrix4x4 *matrix)
{
  if (this->DiffusionRSHGlyphFilter)
    {
    this->DiffusionRSHGlyphFilter->SetGlyphRotationMatrix(matrix);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::SetSlicePositionMatrix(vtkMatrix4x4 *matrix)
{
  Superclass::SetSlicePositionMatrix(matrix);
  if (this->DiffusionRSHGlyphFilter)
    {
    this->DiffusionRSHGlyphFilter->SetVolumePositionMatrix(matrix);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::SetSliceImage(vtkImageData *image)
{
  if (this->DiffusionRSHGlyphFilter)
    {
    this->DiffusionRSHGlyphFilter->SetInput(image);
    this->DiffusionRSHGlyphFilter->SetDimensions(image->GetDimensions());
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::SetPolyData(vtkPolyData *glyphPolyData)
{
}

//----------------------------------------------------------------------------
vtkPolyData* vtkMRMLDiffusionRSHVolumeSliceDisplayNode::GetPolyData()
{
 // if (this->DiffusionRSHGlyphFilter && this->Visibility != 0)
  if ( this->DiffusionRSHGlyphFilter 
        && this->DiffusionRSHGlyphFilter->GetInput() != NULL 
        && this->GetDiffusionRSHDisplayPropertiesNode() != NULL )
    {
    this->UpdatePolyDataPipeline();
    this->DiffusionRSHGlyphFilter->Update();
    return this->DiffusionRSHGlyphFilter->GetOutput();
    }
  else
    {
    return NULL;
    }
}

//----------------------------------------------------------------------------
vtkPolyData* vtkMRMLDiffusionRSHVolumeSliceDisplayNode::GetPolyDataTransformedToSlice()
{

  if ( this->DiffusionRSHGlyphFilter && this->Visibility != 0 && this->GetDiffusionRSHDisplayPropertiesNode() )
    {
    this->UpdatePolyDataPipeline();
    this->DiffusionRSHGlyphFilter->Update();
    this->SliceToXYTransformer->Update();
    return this->SliceToXYTransformer->GetOutput();
    }
  else
    {
    return NULL;
    }
}
//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::UpdatePolyDataPipeline()
{
  // set display properties according to the tensor-specific display properties node for glyphs
  vtkMRMLDiffusionRSHDisplayPropertiesNode * DiffusionRSHDisplayNode = this->GetDiffusionRSHDisplayPropertiesNode( );

  if (DiffusionRSHDisplayNode != NULL)
  {
  // TO DO: implement max # ellipsoids, random sampling features
    this->DiffusionRSHGlyphFilter->SetResolution(1);
    this->DiffusionRSHGlyphFilter->SetScaleFactor( DiffusionRSHDisplayNode->GetGlyphScaleFactor( ) );
    this->DiffusionRSHGlyphFilter->SetDimensionResolution(
                DiffusionRSHDisplayNode->GetSpatialGlyphResolution( ) ,
                DiffusionRSHDisplayNode->GetSpatialGlyphResolution( ) );
    this->DiffusionRSHGlyphFilter->SetMinMaxNormalization( DiffusionRSHDisplayNode->GetGlyphMinMaxNormalization() );      
                                                        
    this->DiffusionRSHGlyphFilter->SetSource( DiffusionRSHDisplayNode->GetGlyphSource( ) );
    
    vtkDebugMacro("setting glyph geometry" << DiffusionRSHDisplayNode->GetGlyphGeometry( ) );
    
    // set glyph coloring
    if (this->GetColorMode ( ) == colorModeSolid)
    {
      this->ScalarVisibilityOff( );
    }
    else
    {
      if (this->GetColorMode ( ) == colorModeScalar)
      {
        this->ScalarVisibilityOn( );

        switch ( DiffusionRSHDisplayNode->GetColorGlyphBy( ))
        {
          case vtkMRMLDiffusionRSHDisplayPropertiesNode::ColorOrientation:
            vtkDebugMacro("coloring with Orientation==============================");
            this->DiffusionRSHGlyphFilter->ColorGlyphsByOrientation( );
            break;
          case vtkMRMLDiffusionRSHDisplayPropertiesNode::Probability:
            vtkDebugMacro("coloring with Probability==============================");
            this->DiffusionRSHGlyphFilter->ColorGlyphsByValue( );
            break;
          default:
            vtkErrorMacro("Glyph coloring not correct");
            break;
        }
      }
    }
  }
  else
  {
    this->ScalarVisibilityOff( );
  }
   
 if (this->GetScalarVisibility() && this->GetAutoScalarRange())
  {
  //this->DiffusionRSHGlyphFilter->Update();
  double *range = this->DiffusionRSHGlyphFilter->GetOutput()->GetScalarRange();
  this->ScalarRange[0] = range[0];
  this->ScalarRange[1] = range[1];
  // avoid Set not to cause event loops
  //this->SetScalarRange( this->DiffusionRSHGlyphFilter->GetOutput()->GetScalarRange() );
  }

}

//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHDisplayPropertiesNode* vtkMRMLDiffusionRSHVolumeSliceDisplayNode::GetDiffusionRSHDisplayPropertiesNode ( )
{
  vtkMRMLDiffusionRSHDisplayPropertiesNode* node = NULL;

  // Find the node corresponding to the ID we have saved.
  if  ( this->GetScene ( ) && this->GetDiffusionRSHDisplayPropertiesNodeID ( ) )
    {
    vtkMRMLNode* cnode = this->GetScene ( ) -> GetNodeByID ( this->DiffusionRSHDisplayPropertiesNodeID );
    node = vtkMRMLDiffusionRSHDisplayPropertiesNode::SafeDownCast ( cnode );
    }

  return node;
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::SetAndObserveDiffusionRSHDisplayPropertiesNodeID ( const char *id )
{
  // Stop observing any old node
  std::cout << "vtkMRMLDiffusionRSHVolumeSliceDisplayNode " << (void *) this << std::endl;
  std::cout << "vtkMRMLDiffusionRSHVolumeSliceDisplayNode " << (void *) this->DiffusionRSHDisplayPropertiesNode << std::endl;
  vtkSetAndObserveMRMLObjectMacro ( this->DiffusionRSHDisplayPropertiesNode, NULL );

  // Set the ID. This is the "ground truth" reference to the node.
  this->SetDiffusionRSHDisplayPropertiesNodeID ( id );

  // Get the node corresponding to the ID. This pointer is only to observe the object.
  vtkMRMLNode *cnode = this->GetDiffusionRSHDisplayPropertiesNode ( );

  // Observe the node using the pointer.
  vtkSetAndObserveMRMLObjectMacro ( this->DiffusionRSHDisplayPropertiesNode , cnode );

}
//---------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event, 
                                           void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);
  return;
}

//-----------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::UpdateScene(vtkMRMLScene *scene)
{
   Superclass::UpdateScene(scene);

   this->SetAndObserveDiffusionRSHDisplayPropertiesNodeID(this->GetDiffusionRSHDisplayPropertiesNodeID());
}

//-----------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::UpdateReferences()
{
  Superclass::UpdateReferences();

  if (this->DiffusionRSHDisplayPropertiesNodeID != NULL && this->Scene->GetNodeByID(this->DiffusionRSHDisplayPropertiesNodeID) == NULL)
    {
    this->SetAndObserveDiffusionRSHDisplayPropertiesNodeID(NULL);
    }
}


//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHVolumeSliceDisplayNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);
  if (this->DiffusionRSHDisplayPropertiesNodeID && !strcmp(oldID, this->DiffusionRSHDisplayPropertiesNodeID))
    {
    this->SetDiffusionRSHDisplayPropertiesNodeID(newID);
    }
}




