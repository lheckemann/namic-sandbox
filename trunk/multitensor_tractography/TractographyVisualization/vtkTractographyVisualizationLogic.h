#ifndef __vtkTractographyVisualizationLogic_h
#define __vtkTractographyVisualizationLogic_h

#include "vtkSlicerModuleLogic.h"

#include "vtkTractographyVisualization.h"
#include "vtkMRMLTractographyVisualizationNode.h"


class vtkFloatArray;
class vtkTubeFilter;

class VTK_TRACTOGRAPHYVISUALIZATION_EXPORT vtkTractographyVisualizationLogic
  : public vtkSlicerModuleLogic
{
public:
  static vtkTractographyVisualizationLogic *New();
  vtkTypeMacro(vtkTractographyVisualizationLogic,vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ProcessMrmlEvents(vtkObject *caller, unsigned long event,
                                 void *callData ) { }

  // Description: Get/Set MRML node storing parameter values.
  vtkGetObjectMacro(TractographyVisualizationNode, 
                    vtkMRMLTractographyVisualizationNode);

  void SetAndObserveTractographyVisualizationNode(
      vtkMRMLTractographyVisualizationNode *n) 
  {
    vtkSetAndObserveMRMLNodeMacro(this->TractographyVisualizationNode, n);
  }

  void Apply();
  void Reduce(bool rec_fa, bool rec_ra, bool rec_trac, bool rec_avg_fa);

  bool CreateTempNode();
  void CleanUp();
  
  // This is not needed since it can be done in the models module as well.
  //void SetColorMap();

  void ToggleTubeDisplay();
  const vtkMRMLModelNode *GetTempNode()
  {
    return this->TmpNode;
  }

  bool GetFaAvailable() {
    return this->FaAvailable;
  }

  void SetValueType();

protected:
  vtkTractographyVisualizationLogic();
  virtual ~vtkTractographyVisualizationLogic();
  vtkTractographyVisualizationLogic(const vtkTractographyVisualizationLogic&);
  void operator=(const vtkTractographyVisualizationLogic&);

  // Calculates the mean and covariance of the currently thresholded value. Also
  // it sets the current threashold do mean + 2 * standard deviation.
  float CalculateStatistics(const char *name, vtkPolyData *data);

  void CalculateMinMax(const char *name, vtkFloatArray *min_values,
                       vtkFloatArray *max_values, double& min, double& max);

  vtkMRMLTractographyVisualizationNode* TractographyVisualizationNode;

  vtkPolyData *TmpData;
  vtkMRMLModelNode *TmpNode;
  vtkMRMLModelDisplayNode *TmpDisplayNode;
  vtkMRMLModelDisplayNode *TmpTubeDisplayNode;

  vtkTubeFilter *TubeFilter;

  vtkPolyData *PolyData;
  bool DownSampled;

  vtkFloatArray *MinValues;
  vtkFloatArray *MaxValues;
  vtkFloatArray *MinFaValues;
  vtkFloatArray *MaxFaValues;
  double NormMin;
  double NormMax;
  double NormDefaultThres;
  double FaMin;
  double FaMax;
  double FaDefaultThres;

  bool FaAvailable;

  double Fa(double d1, double d2, double d3);
  double Ra(double d1, double d2, double d3);
};

#endif
