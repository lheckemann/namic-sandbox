#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkDistractorDefinition.h"

#include <math.h>
#include <fstream>
#include <iostream>
#include <iomanip>

vtkCxxRevisionMacro(vtkDistractorDefinition, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkDistractorDefinition);

vtkDistractorDefinition::vtkDistractorDefinition()
{
  this->SetDistractorName("");
  double initialize[3] = {0,0,0};

  this->SetRailAnchor(initialize);
  this->SetSliderAnchor(initialize);
  this->SetPistonAnchor(initialize);
  this->SetCylinderAnchor(initialize);
  this->SetNewSliderAnchor(initialize);
  this->SetPistonRotationAngle_deg(0.0);
  this->SetRange(0.0, 0.0);

  this->SetDistractorRail(NULL);
  this->SetDistractorSlider(NULL);
  this->SetDistractorPiston(NULL);
  this->SetDistractorCylinder(NULL);
}

vtkDistractorDefinition::~vtkDistractorDefinition()
{
}

void vtkDistractorDefinition::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkDistractorDefinition:             " << this->GetClassName() << "\n";

}

const char* vtkDistractorDefinition::GetDistractorName()
{
  return this->DistractorName.c_str();
}

void vtkDistractorDefinition::SetDistractorName(const char* dName)
{
  this->DistractorName = dName;
}

double* vtkDistractorDefinition::GetRailAnchor()
{
  return this->RailAnchor;
}

void vtkDistractorDefinition::SetRailAnchor(double rAnchor[3])
{
  this->RailAnchor[0] = rAnchor[0];
  this->RailAnchor[1] = rAnchor[1];
  this->RailAnchor[2] = rAnchor[2];
}

double* vtkDistractorDefinition::GetSliderAnchor()
{
  return this->SliderAnchor;
}

void vtkDistractorDefinition::SetSliderAnchor(double sAnchor[3])
{
  this->SliderAnchor[0] = sAnchor[0];
  this->SliderAnchor[1] = sAnchor[1];
  this->SliderAnchor[2] = sAnchor[2];
}

double* vtkDistractorDefinition::GetPistonAnchor()
{
  return this->PistonAnchor;
}

void vtkDistractorDefinition::SetPistonAnchor(double pAnchor[3])
{
  this->PistonAnchor[0] = pAnchor[0];
  this->PistonAnchor[1] = pAnchor[1];
  this->PistonAnchor[2] = pAnchor[2];
}

double* vtkDistractorDefinition::GetCylinderAnchor()
{
  return this->CylinderAnchor;
}

void vtkDistractorDefinition::SetCylinderAnchor(double cAnchor[3])
{
  this->CylinderAnchor[0] = cAnchor[0];
  this->CylinderAnchor[1] = cAnchor[1];
  this->CylinderAnchor[2] = cAnchor[2];
}

double* vtkDistractorDefinition::GetNewSliderAnchor()
{
  return this->NewSliderAnchor;
}

void vtkDistractorDefinition::SetNewSliderAnchor(double nsAnchor[3])
{
  this->NewSliderAnchor[0] = nsAnchor[0];
  this->NewSliderAnchor[1] = nsAnchor[1];
  this->NewSliderAnchor[2] = nsAnchor[2];
}

double vtkDistractorDefinition::GetPistonRotationAngle_deg()
{
  return this->PistonRotationAngle_deg;
}

void vtkDistractorDefinition::SetPistonRotationAngle_deg(double pRA_deg)
{
  if(((pRA_deg >=0) && (pRA_deg <=360)) ||
     ((pRA_deg >= -180) && (pRA_deg <= 180)))
    {
    this->PistonRotationAngle_deg = pRA_deg;
    }
}

double* vtkDistractorDefinition::GetRange()
{
  return this->Range;
}

void vtkDistractorDefinition::SetRange(double min, double max)
{
  if(min <= max)
    {
    this->Range[0] = min;
    this->Range[1] = max;
    }
  else
    {
    this->Range[0] = max;
    this->Range[1] = min;
    }
}

void vtkDistractorDefinition::SetRangeMin(double min)
{
  this->Range[0] = min;
}

void vtkDistractorDefinition::SetRangeMax(double max)
{
  this->Range[1] = max;
}

vtkMRMLModelNode* vtkDistractorDefinition::GetDistractorRail()
{
  return this->DistractorRail;
}

void vtkDistractorDefinition::SetDistractorRail(vtkMRMLModelNode* RailModel)
{
  this->DistractorRail = RailModel;
}

vtkMRMLModelNode* vtkDistractorDefinition::GetDistractorSlider()
{
  return this->DistractorSlider;
}

void vtkDistractorDefinition::SetDistractorSlider(vtkMRMLModelNode* SliderModel)
{
  this->DistractorSlider = SliderModel;
}

vtkMRMLModelNode* vtkDistractorDefinition::GetDistractorPiston()
{
  return this->DistractorPiston;
}

void vtkDistractorDefinition::SetDistractorPiston(vtkMRMLModelNode* PistonModel)
{
  this->DistractorPiston = PistonModel;
}

vtkMRMLModelNode* vtkDistractorDefinition::GetDistractorCylinder()
{
  return this->DistractorCylinder;
}

void vtkDistractorDefinition::SetDistractorCylinder(vtkMRMLModelNode* CylinderModel)
{
  this->DistractorCylinder = CylinderModel;
}

void vtkDistractorDefinition::Hide()
{
  if(this->GetDistractorRail() &&
     this->GetDistractorSlider() &&
     this->GetDistractorPiston() &&
     this->GetDistractorCylinder())
    {
    this->GetDistractorRail()->GetModelDisplayNode()->SetVisibility(false);
    this->GetDistractorSlider()->GetModelDisplayNode()->SetVisibility(false);
    this->GetDistractorPiston()->GetModelDisplayNode()->SetVisibility(false);
    this->GetDistractorCylinder()->GetModelDisplayNode()->SetVisibility(false);
    }
}

void vtkDistractorDefinition::Show()
{
  if(this->GetDistractorRail() &&
     this->GetDistractorSlider() &&
     this->GetDistractorPiston() &&
     this->GetDistractorCylinder())
    {
    this->GetDistractorRail()->GetModelDisplayNode()->SetVisibility(true);
    this->GetDistractorSlider()->GetModelDisplayNode()->SetVisibility(true);
    this->GetDistractorPiston()->GetModelDisplayNode()->SetVisibility(true);
    this->GetDistractorCylinder()->GetModelDisplayNode()->SetVisibility(true);
    }
}

void vtkDistractorDefinition::UpdateAnchors(vtkMatrix4x4* mat)
{
// Rail
  double rAnch[3];

rAnch[0] = mat->GetElement(0,0)*this->GetRailAnchor()[0] +
    mat->GetElement(0,1)*this->GetRailAnchor()[1] +
    mat->GetElement(0,2)*this->GetRailAnchor()[2] +
    mat->GetElement(0,3)*1;

rAnch[1] = mat->GetElement(1,0)*this->GetRailAnchor()[0] +
    mat->GetElement(1,1)*this->GetRailAnchor()[1] +
    mat->GetElement(1,2)*this->GetRailAnchor()[2] +
    mat->GetElement(1,3)*1;

rAnch[2] = mat->GetElement(2,0)*this->GetRailAnchor()[0] +
    mat->GetElement(2,1)*this->GetRailAnchor()[1] +
    mat->GetElement(2,2)*this->GetRailAnchor()[2] +
    mat->GetElement(2,3)*1;

 this->SetRailAnchor(rAnch);

// Slider
 double sAnch[3];

sAnch[0] = mat->GetElement(0,0)*this->GetSliderAnchor()[0] +
    mat->GetElement(0,1)*this->GetSliderAnchor()[1] +
    mat->GetElement(0,2)*this->GetSliderAnchor()[2] +
    mat->GetElement(0,3)*1;

sAnch[1] = mat->GetElement(1,0)*this->GetSliderAnchor()[0] +
    mat->GetElement(1,1)*this->GetSliderAnchor()[1] +
    mat->GetElement(1,2)*this->GetSliderAnchor()[2] +
    mat->GetElement(1,3)*1;

sAnch[2] = mat->GetElement(2,0)*this->GetSliderAnchor()[0] +
    mat->GetElement(2,1)*this->GetSliderAnchor()[1] +
    mat->GetElement(2,2)*this->GetSliderAnchor()[2] +
    mat->GetElement(2,3)*1;

 this->SetSliderAnchor(sAnch);

// Piston
 double pAnch[3];

pAnch[0] = mat->GetElement(0,0)*this->GetPistonAnchor()[0] +
    mat->GetElement(0,1)*this->GetPistonAnchor()[1] +
    mat->GetElement(0,2)*this->GetPistonAnchor()[2] +
    mat->GetElement(0,3)*1;

pAnch[1] = mat->GetElement(1,0)*this->GetPistonAnchor()[0] +
    mat->GetElement(1,1)*this->GetPistonAnchor()[1] +
    mat->GetElement(1,2)*this->GetPistonAnchor()[2] +
    mat->GetElement(1,3)*1;

pAnch[2] = mat->GetElement(2,0)*this->GetPistonAnchor()[0] +
    mat->GetElement(2,1)*this->GetPistonAnchor()[1] +
    mat->GetElement(2,2)*this->GetPistonAnchor()[2] +
    mat->GetElement(2,3)*1;

 this->SetPistonAnchor(pAnch);

// Cylinder
 double cAnch[3];

cAnch[0] = mat->GetElement(0,0)*this->GetCylinderAnchor()[0] +
    mat->GetElement(0,1)*this->GetCylinderAnchor()[1] +
    mat->GetElement(0,2)*this->GetCylinderAnchor()[2] +
    mat->GetElement(0,3)*1;

cAnch[1] = mat->GetElement(1,0)*this->GetCylinderAnchor()[0] +
    mat->GetElement(1,1)*this->GetCylinderAnchor()[1] +
    mat->GetElement(1,2)*this->GetCylinderAnchor()[2] +
    mat->GetElement(1,3)*1;

cAnch[2] = mat->GetElement(2,0)*this->GetCylinderAnchor()[0] +
    mat->GetElement(2,1)*this->GetCylinderAnchor()[1] +
    mat->GetElement(2,2)*this->GetCylinderAnchor()[2] +
    mat->GetElement(2,3)*1;

 this->SetCylinderAnchor(cAnch);
}
