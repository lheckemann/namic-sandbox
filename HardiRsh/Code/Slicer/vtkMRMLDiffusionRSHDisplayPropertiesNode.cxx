/*=auto=========================================================================

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLDiffusionRSHDisplayPropertiesNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/
#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLDiffusionRSHDisplayPropertiesNode.h"
#include "vtkMRMLScene.h"

#include "vtkLookupTable.h"
#include "vtkLineSource.h"
#include "vtkTubeFilter.h"
//#include "vtkSphereSource.h"
#include "vtkSbiaSphereSource.h"

//#include "vtkSphericalHarmonicSource.h"

//------------------------------------------------------------------------------
vtkMRMLDiffusionRSHDisplayPropertiesNode* vtkMRMLDiffusionRSHDisplayPropertiesNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDiffusionRSHDisplayPropertiesNode");
  if(ret)
    {
    return (vtkMRMLDiffusionRSHDisplayPropertiesNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDiffusionRSHDisplayPropertiesNode;
}

//-----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLDiffusionRSHDisplayPropertiesNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDiffusionRSHDisplayPropertiesNode");
  if(ret)
    {
    return (vtkMRMLDiffusionRSHDisplayPropertiesNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDiffusionRSHDisplayPropertiesNode;
}


//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHDisplayPropertiesNode::vtkMRMLDiffusionRSHDisplayPropertiesNode()
{
  // Default display is FA (often used) and line glyphs (quickest to render)
  this->ScalarInvariant = this->RSH_Power_0,
  this->GlyphGeometry = this->Surface;
  this->ColorGlyphBy = this->Probability;

  // Glyph general parameters
  this->GlyphScaleFactor = 50;
  
  // Glyph parameters
  this->GlyphThetaResolution = 15;
  this->GlyphPhiResolution = 15;
  
  // Glyph Spatial Resolutions
  this->SpatialGlyphResolution = 20;  // was 10

  this->GlyphMinMaxNormalization = 0; //No Min Max Normalization

  // VTK Objects
  this->GlyphSource = NULL;
  this->UpdateGlyphSource();

  // set the type to user
  this->SetTypeToUser();

}

//----------------------------------------------------------------------------
vtkMRMLDiffusionRSHDisplayPropertiesNode::~vtkMRMLDiffusionRSHDisplayPropertiesNode()
{
  if ( this->GlyphSource != NULL )
    {
    this->GlyphSource->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHDisplayPropertiesNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults

  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " glyphGeometry=\"" << this->GlyphGeometry << "\"";
  of << indent << " colorGlyphBy=\"" << this->ColorGlyphBy << "\"";
  of << indent << " glyphScaleFactor=\"" << this->GlyphScaleFactor << "\"";
  of << indent << " glyphThetaResolution=\"" << this->GlyphThetaResolution << "\"";
  of << indent << " glyphPhiResolution=\"" << this->GlyphPhiResolution << "\"";
  of << indent << " spatialGlyphResolution=\"" << this->SpatialGlyphResolution << "\"";
  of << indent << " GlyphMinMaxNormalization=\"" << this->GlyphMinMaxNormalization << "\"";

}

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHDisplayPropertiesNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
  {
      attName = *(atts++);
      attValue = *(atts++);
      if (!strcmp(attName, "glyphGeometry")) 
      {
      int glyphGeometry;
      std::stringstream ss;
      ss << attValue;
      ss >> glyphGeometry;
      this->SetGlyphGeometry(glyphGeometry);
      }
      else if (!strcmp(attName, "colorGlyphBy")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> ColorGlyphBy;
      }
      else if (!strcmp(attName, "glyphScaleFactor")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> GlyphScaleFactor;
      }
      else if (!strcmp(attName, "glyphThetaResolution")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> GlyphThetaResolution;
      }      
      else if (!strcmp(attName, "glyphPhiResolution")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> GlyphPhiResolution;
      }
      else if (!strcmp(attName, "spatialGlyphResolution")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> SpatialGlyphResolution;
      }      
      else if (!strcmp(attName, "GlyphMinMaxNormalization")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> GlyphMinMaxNormalization;
      }      
  }
  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, ID
void vtkMRMLDiffusionRSHDisplayPropertiesNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLDiffusionRSHDisplayPropertiesNode *node = (vtkMRMLDiffusionRSHDisplayPropertiesNode *) anode;

  this->SetScalarInvariant(node->ScalarInvariant);
  this->SetGlyphGeometry(node->GlyphGeometry);
  this->SetColorGlyphBy(node->ColorGlyphBy);
  this->SetGlyphScaleFactor(node->GlyphScaleFactor);
  this->SetGlyphThetaResolution(node->GlyphThetaResolution);
  this->SetGlyphPhiResolution(node->GlyphPhiResolution);
  this->SetSpatialGlyphResolution(node->SpatialGlyphResolution);
  this->SetGlyphMinMaxNormalization(node->GlyphMinMaxNormalization);
  
  this->EndModify(disabledModify);
 }

//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHDisplayPropertiesNode::PrintSelf(ostream& os, vtkIndent indent)
{

  Superclass::PrintSelf(os,indent);
  os << indent << "ScalarInvariant          :        " << this->ScalarInvariant << "\n";
  os << indent << "glyphGeometry            :        " << this->GlyphGeometry << "\n";
  os << indent << "colorGlyphBy             :        " << this->ColorGlyphBy << "\n";
  os << indent << "glyphScaleFactor         :        " << this->GlyphScaleFactor << "\n";
  os << indent << "glyphThetaResolution     :        " << this->GlyphThetaResolution << "\n";
  os << indent << "glyphPhiResolution       :        " << this->GlyphPhiResolution << "\n";
  os << indent << "spatialGlyphResolution   :        " << this->SpatialGlyphResolution << "\n";
  os << indent << "GlyphMinMaxNormalization :        " << this->GlyphMinMaxNormalization << "\n";
  
}


//----------------------------------------------------------------------------
void vtkMRMLDiffusionRSHDisplayPropertiesNode::UpdateGlyphSource ( )
{
  vtkDebugMacro("Get Glyph Source");

  // Get rid of any old glyph source
  if ( this->GlyphSource != NULL )
    {
    this->GlyphSource->Delete();
    this->GlyphSource = NULL;
    }

   // Create a new glyph source according to current settings

   switch ( this->GlyphGeometry )
   {
     case Surface:
      // vtkSphereSource *sphere = vtkSphereSource::New();
      vtkSbiaSphereSource *sphere = vtkSbiaSphereSource::New();
      sphere->SetThetaResolution( this->GlyphThetaResolution );
      sphere->SetPhiResolution( this->GlyphPhiResolution );

      sphere->Update( );
      this->SetGlyphSource( sphere->GetOutput( ) );
      sphere->Delete( );

      vtkDebugMacro("Get Glyph Source: Ellipsoids");
      break;

   }
}

const char* vtkMRMLDiffusionRSHDisplayPropertiesNode::GetScalarEnumAsString(int var)
{
  if (var ==  vtkMRMLDiffusionRSHDisplayPropertiesNode::RSH_Power_0)
    {
    return "RSH Power 0";
    }
  else if (var == vtkMRMLDiffusionRSHDisplayPropertiesNode::RSH_Power_2)
    {
    return "RSH Power 2";
    }
  else if (var == vtkMRMLDiffusionRSHDisplayPropertiesNode::RSH_Power_4)
    {
    return "RSH Power 4";
    }
  else if (var == vtkMRMLDiffusionRSHDisplayPropertiesNode::RSH_Power_6)
    {
    return "RSH Power 6";
    }
  else if (var == vtkMRMLDiffusionRSHDisplayPropertiesNode::RSH_Power_8)
    {
    return "RSH Power 8";
    }
  else if (var == vtkMRMLDiffusionRSHDisplayPropertiesNode::RSH_Power_10)
    {
    return "RSH Power 10";
    }
  else
    return "(unknown)";
}

const char* vtkMRMLDiffusionRSHDisplayPropertiesNode::GetScalarInvariantAsString()
{
  return this->GetScalarEnumAsString(this->ScalarInvariant);
}

const char* vtkMRMLDiffusionRSHDisplayPropertiesNode::GetGlyphGeometryAsString()
{
  if (this->GlyphGeometry == this->Surface)
    {
    return "Surface";
    }
  return "(unknown)";
}

const char *
vtkMRMLDiffusionRSHDisplayPropertiesNode::GetColorGlyphByAsString()
{

  if (this->ColorGlyphBy ==  this->Probability)
    {
    return "Probability";
    }
  else if (this->ColorGlyphBy == this->ColorOrientation)
    {
    return "Orientation";
    }
  else
    return "(unknown)";

}

