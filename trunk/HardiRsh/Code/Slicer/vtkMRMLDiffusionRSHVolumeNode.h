/*=auto=========================================================================

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.13 $

=========================================================================auto=*/
// .NAME vtkMRMLDiffusionRSHVolumeNode - MRML node for representing diffusion weighted MRI volume 
// .SECTION Description
// Diffusion Weigthed Volume nodes describe data sets that encode diffusion weigthed
// images. These images are the basis for computing the diffusion tensor.
// The node is a container for the neccesary information to interpert DW images:
// 1. Gradient information.
// 2. B value for each gradient.
// 3. Measurement frame that relates the coordinate system where the gradients are given 
//  to RAS.

#ifndef __vtkMRMLDiffusionRSHVolumeNode_h
#define __vtkMRMLDiffusionRSHVolumeNode_h


#include "vtkMRMLDiffusionImageVolumeNode.h"
#include "vtkMRMLDiffusionRSHVolumeDisplayNode.h"
#include "vtkMRMLDiffusionRSHVolumeSliceDisplayNode.h"
#include "vtkMRMLNRRDStorageNode.h"
#include "vtkMRMLNRRDStorageNode.h"

class vtkDoubleArray;
class vtkMRMLDiffusionTensorVolumeSliceDisplayNode;
class vtkMRMLStorageNode;

class VTK_MRML_EXPORT vtkMRMLDiffusionRSHVolumeNode : public vtkMRMLDiffusionImageVolumeNode
{
  public:
  static vtkMRMLDiffusionRSHVolumeNode *New();
  vtkTypeMacro(vtkMRMLDiffusionRSHVolumeNode,vtkMRMLDiffusionImageVolumeNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "DiffusionRSHVolume";};

  // Description:
  // Associated volume display MRML node
  virtual void SetAndObserveDisplayNodeID(const char *DisplayNodeID);

  // Description:
  // Update the stored reference to another node in the scene
  virtual void UpdateReferenceID(const char *oldID, const char *newID);

   // Description:
  // Finds the storage node and read the data
  //void UpdateScene(vtkMRMLScene *scene);

  // Description:
  // Updates this node if it depends on other nodes 
  // when the node is deleted in the scene
  void UpdateReferences();

  
  // Description:
  // alternative method to propagate events generated in Display nodes
  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/, 
                                   unsigned long /*event*/, 
                                   void * /*callData*/ );
 
  // Description:
  // Associated display MRML node
  virtual vtkMRMLDiffusionRSHVolumeDisplayNode* GetDiffusionTensorVolumeDisplayNode()
  {
    return vtkMRMLDiffusionRSHVolumeDisplayNode::SafeDownCast(this->GetDisplayNode());
  }

  // Description:
  // Create default storage node or NULL if does not have one
  virtual vtkMRMLStorageNode* CreateDefaultStorageNode()
    {
    return vtkMRMLNRRDStorageNode::New();
    };


protected:
  vtkMRMLDiffusionRSHVolumeNode();
  ~vtkMRMLDiffusionRSHVolumeNode();
  vtkMRMLDiffusionRSHVolumeNode(const vtkMRMLDiffusionRSHVolumeNode&);
  void operator=(const vtkMRMLDiffusionRSHVolumeNode&);

};

#endif
