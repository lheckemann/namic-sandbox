
// .NAME vtkMRMLDiffusionRSHDisplayPropertiesNode - MRML node for display of a diffusion tensor.
// .SECTION Description
// This node describes display properties at the (conceptual) single-tensor level.
// A tensor can be displayed using various scalar invariants and glyphs.
// This class is used by classes (vtkMRMLFiberBundleDisplayNode, 
// vtkMRMLDiffusionRSHVolumeDisplayNode) that handle higher-level display
// concepts for many diffusion tensors, such as choosing between scalars/glyphs/etc. 
// for specific display needs.
// This class inherits from the vtkMRMLColorNode->vtkMRMLColorTableNode superclasses,
// used for vtkMRMLModelNodes and vtkMRMLVolumeNodes, in order to
// provide specific lookup tables for the scalar invariant display.
//

#ifndef __vtkMRMLDiffusionRSHDisplayPropertiesNode_h
#define __vtkMRMLDiffusionRSHDisplayPropertiesNode_h

#include "vtkMRML.h"
#include "vtkMRMLColorTableNode.h"
#include "vtkPolyData.h"


class VTK_MRML_EXPORT vtkMRMLDiffusionRSHDisplayPropertiesNode : public vtkMRMLColorTableNode
{
 public:
  static vtkMRMLDiffusionRSHDisplayPropertiesNode *New();
  vtkTypeMacro(vtkMRMLDiffusionRSHDisplayPropertiesNode,vtkMRMLColorTableNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  //--------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Read node attributes from a MRML file in XML format.
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object.
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "DiffusionRSHDisplayProperties";};

  //--------------------------------------------------------------------------
  // Display Information: Types of scalars that may be generated from tensors.
  //--------------------------------------------------------------------------

  //BTX
  enum
  {
    RSH_Power_0,
    RSH_Power_2,
    RSH_Power_4,
    RSH_Power_6,
    RSH_Power_8,
    RSH_Power_10
  };
  //ETX

  //--------------------------------------------------------------------------
  // Display Information: Functions to choose scalar invariant
  //--------------------------------------------------------------------------

  // Description:
  // Get type of scalar invariant (tensor-derived scalar, invariant to tensor 
  // rotation) selected for display.
  vtkGetMacro(ScalarInvariant, int);

  // Description:
  // Get type of scalar invariant (tensor-derived scalar, invariant to tensor 
  // rotation) selected for display.
  vtkSetMacro(ScalarInvariant, int);
 
  // Description:


  // Description:
  // Return the lowest and highest integers, for use in looping
  int GetFirstScalarInvariant() {return this->RSH_Power_0;};
  int GetLastScalarInvariant() {return this->RSH_Power_10;};

  // Description:
  // Return a text string describing the ScalarInvariant variable
  virtual const char * GetScalarInvariantAsString();


  //--------------------------------------------------------------------------
  // Display Information: Types of glyph geometry that can be displayed
  //--------------------------------------------------------------------------

  //BTX
  enum
  {
    Surface = 0,
  };
  //ETX

  //--------------------------------------------------------------------------
  // Display Information: Functions to choose the type of glyph geometry
  //--------------------------------------------------------------------------

  // Description:
  // Get the type of glyph geometry (SURFACE etc.) 
  vtkGetMacro(GlyphGeometry, int);

  // Description:
  // Set the type of glyph geometry (SURFACE etc.) 
  //vtkSetMacro(GlyphGeometry, int);
  // Also update the glyph polydata source
  void SetGlyphGeometry( int geometry ) {

    if ( this->GlyphGeometry != geometry ) 
      {
      this->GlyphGeometry = geometry;
      this->UpdateGlyphSource();
      this->Modified();
      }
  }

  void SetGlyphGeometryToSurface() {
    this->SetGlyphGeometry(this->Surface);
  };

  // Description:
  // Return the lowest and highest integers, for use in looping
  int GetFirstGlyphGeometry() {return this->Surface;};
  int GetLastGlyphGeometry() {return this->Surface;};

  // Description:
  // Return a text string describing the GlyphGeometry variable
  virtual const char * GetGlyphGeometryAsString();

  //--------------------------------------------------------------------------
  // Display Information: Parameters of glyph geometry
  //--------------------------------------------------------------------------

  // Description:
  // Get the scale factor applied to the glyphs.
  vtkGetMacro(GlyphScaleFactor, double);

  // Description:
  // Set the scale factor applied to the glyphs.
  vtkSetMacro(GlyphScaleFactor, double);

  //--------------------------------------------------------------------------
  // Display Information: Parameters of ODFss glyph geometry
  //--------------------------------------------------------------------------

  // Description:
  // Number of polygons used in longitude direction for sphere that will
  // be scaled by tensor to form ellipsoid.
  vtkGetMacro(GlyphThetaResolution, int);
  vtkSetMacro(GlyphThetaResolution, int);

  // Description:
  // Number of polygons used in latitude direction for sphere that will
  // be scaled by tensor to form ellipsoid.
  vtkGetMacro(GlyphPhiResolution, int);
  vtkSetMacro(GlyphPhiResolution, int);

  //--------------------------------------------------------------------------
  // Display Information: Functions to choose the type of glyph coloring
  //--------------------------------------------------------------------------

  //BTX
  enum
  {
    Probability = 0,
    ColorOrientation = 1,
  };
  //ETX

  // Description:
  // Get type of scalar invariant (tensor-derived scalar, invariant to tensor 
  // rotation) selected for display.
  vtkGetMacro(ColorGlyphBy, int);

  // Description:
  // Get type of scalar invariant (tensor-derived scalar, invariant to tensor 
  // rotation) selected for display.
  vtkSetMacro(ColorGlyphBy, int);

  // Description:
  // Resolution of lines displayed as tensor glyphs
  vtkGetMacro(SpatialGlyphResolution, int);
  //vtkSetMacro(SpatialGlyphResolution, int);
  void SetSpatialGlyphResolution( int resolution ) {

    if ( this->SpatialGlyphResolution != resolution ) 
      {
      this->SpatialGlyphResolution = resolution;
//      if ( this->GlyphGeometry == this->Lines || this->GlyphGeometry == this->Tubes)
//        {
//        // Update the source if the resolution has changed
//        this->UpdateGlyphSource();
//        }

      this->Modified();
      }
  }

  vtkGetMacro(GlyphMinMaxNormalization, int);
  void SetGlyphMinMaxNormalization( int _arg )
  {
    if ( this->GlyphMinMaxNormalization != _arg ) 
      {
      this->GlyphMinMaxNormalization = _arg;
      this->Modified();
      }
  }

  void SetAngularGlyphResolution( int resolution ) {

    if ( this->GlyphThetaResolution != resolution ) 
      {
      this->GlyphThetaResolution = resolution;
      this->GlyphPhiResolution = resolution;
      
      this->UpdateGlyphSource();
      this->Modified();
      }
  }


  // Description:
  // Return the lowest and highest integers, for use in looping
  int GetFirstColorGlyphBy() {return this->Probability;};
  int GetLastColorGlyphBy() {return this->ColorOrientation;};
  
  // Description:
  // Return a text string describing the ColorGlyphBy
  virtual const char * GetColorGlyphByAsString();
 
  // Description:
  // Set scalar invariant to trace (sum of eigenvalues).
  void ColorGlyphByProbability() {
    this->SetColorGlyphBy(this->Probability);
  };

  // Description:
  // Set scalar invariant to FA (normalized variance of eigenvalues)
  void ColorGlyphByColorOrientation() {
    this->SetColorGlyphBy(this->ColorOrientation);
  };  


  //--------------------------------------------------------------------------
  // Convenience functions to get an appropriate glyph source
  //--------------------------------------------------------------------------

  // Description:
  // Get a polydata object according to current glyph display settings
  // (so a line, sphere, or tube) to use as a source for a glyphing filter.
  vtkGetObjectMacro( GlyphSource, vtkPolyData );

 //Helper function to get the string of Scalar enums
  static const char *GetScalarEnumAsString(int val);

 protected:
  vtkMRMLDiffusionRSHDisplayPropertiesNode();
  ~vtkMRMLDiffusionRSHDisplayPropertiesNode();
  vtkMRMLDiffusionRSHDisplayPropertiesNode(const vtkMRMLDiffusionRSHDisplayPropertiesNode&);
  void operator=(const vtkMRMLDiffusionRSHDisplayPropertiesNode&);

  void UpdateGlyphSource ( );

  // ---- Parameters that should be written to MRML --- //

  // Scalar display parameters
  int ScalarInvariant;

  // Glyph general parameters
  int GlyphGeometry;
  int ColorGlyphBy;
  double GlyphScaleFactor;

  // Glyph parameters
  int GlyphThetaResolution;
  int GlyphPhiResolution;

  // Spatial Glyph Resolution
  int SpatialGlyphResolution;
  
  //Flag for min max normalization
  int GlyphMinMaxNormalization;
  

  // ---- End of parameters that should be written to MRML --- //
 
  // ---- VTK objects for display --- //
  vtkPolyData * GlyphSource;

  // This is used internally to set a pointer to this polydata
  // and reference count it.  
  // TO DO: is this causing an extra modified event?
  vtkSetObjectMacro( GlyphSource, vtkPolyData );


  // TO DO: add specific lookup tables ranging from 0..1 for or -1 1
  // for scalar invariants with those ranges

  // TO DO: read/write MRML for all parameters

};

#endif

