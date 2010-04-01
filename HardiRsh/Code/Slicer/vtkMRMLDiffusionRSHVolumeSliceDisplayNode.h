// .NAME vtkMRMLDiffusionRSHVolumeSliceDisplayNode - MRML node to represent display properties for tractography.
// .SECTION Description
// vtkMRMLDiffusionRSHVolumeSliceDisplayNode nodes store display properties of trajectories 
// from tractography in diffusion MRI data, including color type (by bundle, by fiber, 
// or by scalar invariants), display on/off for tensor glyphs and display of 
// trajectory as a line or tube.
//

#ifndef __vtkMRMLDiffusionRSHVolumeSliceDisplayNode_h
#define __vtkMRMLDiffusionRSHVolumeSliceDisplayNode_h

#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"

#include "vtkMRML.h"
#include "vtkMRMLGlyphableVolumeSliceDisplayNode.h"
//#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLDiffusionRSHDisplayPropertiesNode.h"

#include "vtkDiffusionRSHGlyph.h"

class vtkDiffusionRSHGlyph;
class vtkTransform;
class vtkTransformPolyDataFilter;

class VTK_MRML_EXPORT vtkMRMLDiffusionRSHVolumeSliceDisplayNode : public vtkMRMLGlyphableVolumeSliceDisplayNode
{
 public:
  static vtkMRMLDiffusionRSHVolumeSliceDisplayNode *New (  );
  vtkTypeMacro ( vtkMRMLDiffusionRSHVolumeSliceDisplayNode,vtkMRMLGlyphableVolumeSliceDisplayNode );
  void PrintSelf ( ostream& os, vtkIndent indent );
  
  //--------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------

  virtual vtkMRMLNode* CreateNodeInstance (  );

  // Description:
  // Read node attributes from XML (MRML) file
  virtual void ReadXMLAttributes ( const char** atts );

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML ( ostream& of, int indent );


  // Description:
  // Copy the node's attributes to this object
  virtual void Copy ( vtkMRMLNode *node );
  
  // Description:
  // Get node XML tag name (like Volume, UnstructuredGrid)
  virtual const char* GetNodeTagName ( ) {return "DiffusionRSHVolumeSliceDisplayNode";};

  // Description:
  // Updates this node if it depends on other nodes 
  // when the node is deleted in the scene
  virtual void UpdateReferences();

  // Description:
  // Finds the storage node and read the data
  virtual void UpdateScene(vtkMRMLScene *scene);

  // Description:
  // Update the stored reference to another node in the scene
  virtual void UpdateReferenceID(const char *oldID, const char *newID);

  // Description:
  // alternative method to propagate events generated in Display nodes
  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/, 
                                   unsigned long /*event*/, 
                                   void * /*callData*/ );


  // Description:
  // Sets polydata for glyph input (usually stored in FiberBundle node)
  void SetPolyData(vtkPolyData *glyphPolyData);

  // Description:
  // Gets resulting glyph PolyData 
  virtual vtkPolyData* GetPolyData();
  
  // Description:
  // Gets resulting glyph PolyData transfomed to slice XY
  virtual vtkPolyData* GetPolyDataTransformedToSlice();
   
  // Description:
  // Update the pipeline based on this node attributes
  virtual void UpdatePolyDataPipeline();

  // Description:
  // Set ImageData for a volume slice
  virtual void SetSliceImage(vtkImageData *image);
 
  // Description:
  // Set slice to RAS transformation
  virtual void SetSlicePositionMatrix(vtkMatrix4x4 *matrix);

  // Description:
  // Set slice to IJK transformation
  virtual void SetSliceGlyphRotationMatrix(vtkMatrix4x4 *matrix);

  //--------------------------------------------------------------------------
  // Display Information: Geometry to display (not mutually exclusive)
  //--------------------------------------------------------------------------


  //--------------------------------------------------------------------------
  // Display Information: Color Mode
  // 0) solid color by group 1) color by scalar invariant 
  // 2) color by avg scalar invariant 3) color by other
  //--------------------------------------------------------------------------

  //BTX
  enum
  {
    colorModeSolid = 0,
    colorModeScalar = 1,
    colorModeFunctionOfScalar = 2,
    colorModeUseCellScalars = 3
  };
  //ETX

  //--------------------------------------------------------------------------
  // Display Information: ColorMode for ALL nodes
  //--------------------------------------------------------------------------

 // Description:
  // Color mode for glyphs. The color modes are mutually exclusive.
  vtkGetMacro ( ColorMode, int );
  vtkSetMacro ( ColorMode, int );
 
  // Description:
  // Color by solid color (for example the whole fiber bundle red. blue, etc.)
  void SetColorModeToSolid ( ) {
    this->SetColorMode ( this->colorModeSolid );
  };

  // Description:
  // Color according to the tensors using various scalar invariants.
  void SetColorModeToScalar ( ) {
    this->SetColorMode ( this->colorModeScalar );
  };

  // Description:
  // Color according to the tensors using a function of scalar invariants along the tract.
  // This enables coloring by average FA, for example.
  void SetColorModeToFunctionOfScalar ( ) {
    this->SetColorMode ( this->colorModeFunctionOfScalar );
  };

  // Description:
  // Use to color by the active cell scalars.  This is intended to support
  // external processing of fibers, for example to label each with the distance
  // of that fiber from an fMRI activation.  Then by making that information
  // the active cell scalar field, this will allow coloring by that information.
  // TO DO: make sure this information can be saved with the tract, save name of
  // active scalar field if needed.
  void SetColorModeToUseCellScalars ( ) {
    this->SetColorMode ( this->colorModeUseCellScalars );
  };



  //--------------------------------------------------------------------------
  // Display Information: ColorMode for glyphs
  //--------------------------------------------------------------------------
  

  //--------------------------------------------------------------------------
  // MRML nodes that are observed
  //--------------------------------------------------------------------------
  
 
  // Node reference to ALL DT nodes

  // Description:
  // Get diffusion tensor display MRML object for fiber glyph.
  vtkMRMLDiffusionRSHDisplayPropertiesNode* GetDiffusionRSHDisplayPropertiesNode ( );

  // Description:
  // Set diffusion tensor display MRML object for fiber glyph.
  void SetAndObserveDiffusionRSHDisplayPropertiesNodeID ( const char *ID );

  // Description:
  // Get ID of diffusion tensor display MRML object for fiber glyph.
  vtkGetStringMacro(DiffusionRSHDisplayPropertiesNodeID);

 protected:
  vtkMRMLDiffusionRSHVolumeSliceDisplayNode ( );
  ~vtkMRMLDiffusionRSHVolumeSliceDisplayNode ( );
  vtkMRMLDiffusionRSHVolumeSliceDisplayNode ( const vtkMRMLDiffusionRSHVolumeSliceDisplayNode& );
  void operator= ( const vtkMRMLDiffusionRSHVolumeSliceDisplayNode& );

    vtkDiffusionRSHGlyph  *DiffusionRSHGlyphFilter;

    // ALL MRML nodes
    vtkMRMLDiffusionRSHDisplayPropertiesNode *DiffusionRSHDisplayPropertiesNode;
    char *DiffusionRSHDisplayPropertiesNodeID;

    vtkSetReferenceStringMacro(DiffusionRSHDisplayPropertiesNodeID);

    // Enumerated
    int ColorMode;


};

#endif