#ifndef __vtkMRMLTractographyVisualizationNode_h
#define __vtkMRMLTractographyVisualizationNode_h

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkTractographyVisualization.h"


class VTK_TRACTOGRAPHYVISUALIZATION_EXPORT vtkMRMLTractographyVisualizationNode
  : public vtkMRMLNode
{
public:
  static vtkMRMLTractographyVisualizationNode *New();
  vtkTypeMacro(vtkMRMLTractographyVisualizationNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Create instance of a node.
  virtual vtkMRMLNode* CreateNodeInstance();

  // Set node attributes from name/value pairs
  virtual void ReadXMLAttributes( const char** atts);

  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Copy the node's attributes to this object.
  virtual void Copy(vtkMRMLNode *node);

  // Get unique node XML tag name (like Volume, Model).
  virtual const char* GetNodeTagName()
  {
    return "VisualizationParameters";
  }

  // Update the stored reference to another node in the scene.
  virtual void UpdateReferenceID(const char *oldID, const char *newID);

  vtkGetMacro(ValueType, int);
  vtkSetMacro(ValueType, int);

  // Get/Set Stopping Mode (module parameter)
  // 0 - Segments only
  // 1 - Complete fibers
  vtkGetMacro(RemovalMode, int);
  vtkSetMacro(RemovalMode, int);

  // Get/Set Display Mode (module parameter)
  // 0 - Lines
  // 1 - Tubes
  vtkGetMacro(DisplayMode, int);
  vtkSetMacro(DisplayMode, int);

  vtkGetMacro(ShowTubes, bool);
  vtkSetMacro(ShowTubes, bool);

  // Get/Set time step (module parameter).
  vtkGetMacro(Threshold, double);
  vtkSetMacro(Threshold, double);
  vtkGetMacro(ThresholdMin, double);
  vtkSetMacro(ThresholdMin, double);
  vtkGetMacro(ThresholdMax, double);
  vtkSetMacro(ThresholdMax, double);

  // Get/Set input model.
  vtkGetStringMacro(InputModelRef);
  vtkSetStringMacro(InputModelRef);

  // Get/Set output model.
  vtkGetStringMacro(OutputModelRef);
  vtkSetStringMacro(OutputModelRef);

protected:
  vtkMRMLTractographyVisualizationNode();
  ~vtkMRMLTractographyVisualizationNode();
  vtkMRMLTractographyVisualizationNode(
      const vtkMRMLTractographyVisualizationNode&);
  void operator=(const vtkMRMLTractographyVisualizationNode&);

  double Threshold;
  double ThresholdMin;
  double ThresholdMax;
  
  char *InputModelRef;
  char *OutputModelRef;

  int ValueType;
  int RemovalMode;
  int DisplayMode;

  bool ShowTubes;
};

#endif
