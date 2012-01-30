#ifndef __vtkDistractorDefinition_h
#define __vtkDistractorDefinition_h

#include "vtkOsteoPlanWin32Header.h"
#include "vtkObject.h"
#include "vtkMRMLModelNode.h"
#include "vtkMatrix4x4.h"

class VTK_OsteoPlan_EXPORT vtkDistractorDefinition : public vtkObject
{

 public:

  static vtkDistractorDefinition *New();
  vtkTypeRevisionMacro(vtkDistractorDefinition,vtkObject);
  void  PrintSelf(ostream&, vtkIndent);

  // Name
  const char* GetDistractorName();
  void SetDistractorName(const char* dName);

  // Anchors
  double* GetRailAnchor();
  void SetRailAnchor(double rAnchor[3]);

  double* GetSliderAnchor();
  void SetSliderAnchor(double sAnchor[3]);

  double* GetPistonAnchor();
  void SetPistonAnchor(double pAnchor[3]);

  double* GetCylinderAnchor();
  void SetCylinderAnchor(double cAnchor[3]);

  double* GetNewSliderAnchor();
  void SetNewSliderAnchor(double nsAnchor[3]);

  double GetPistonRotationAngle_deg();
  void SetPistonRotationAngle_deg(double pRA_deg);

  // Range
  double* GetRange();
  void SetRange(double min, double max);
  void SetRangeMin(double min);
  void SetRangeMax(double max);

  // Models
  vtkMRMLModelNode* GetDistractorRail();
  void SetDistractorRail(vtkMRMLModelNode* RailModel);

  vtkMRMLModelNode* GetDistractorSlider();
  void SetDistractorSlider(vtkMRMLModelNode* SliderModel);

  vtkMRMLModelNode* GetDistractorPiston();
  void SetDistractorPiston(vtkMRMLModelNode* PistonModel);

  vtkMRMLModelNode* GetDistractorCylinder();
  void SetDistractorCylinder(vtkMRMLModelNode* CylinderModel);

  void Hide();
  void Show();


  // Update Anchors
  void UpdateAnchors(vtkMatrix4x4* mat);

 protected:

  vtkDistractorDefinition();
  ~vtkDistractorDefinition();

  vtkDistractorDefinition(const vtkDistractorDefinition&);

 private:

  // Members
  //BTX
  std::string DistractorName;
  //ETX

  double RailAnchor[3];
  double SliderAnchor[3];
  double PistonAnchor[3];
  double CylinderAnchor[3];

  double NewSliderAnchor[3];

  double PistonRotationAngle_deg;

  double Range[2];

  vtkMRMLModelNode* DistractorRail;
  vtkMRMLModelNode* DistractorSlider;
  vtkMRMLModelNode* DistractorPiston;
  vtkMRMLModelNode* DistractorCylinder;
};

#endif
