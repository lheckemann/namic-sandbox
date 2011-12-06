/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkDistractorModelingLogic.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicerModelsGUI.h"

#include <math.h>
#include <fstream>
#include <iostream>
#include <iomanip>

vtkCxxRevisionMacro(vtkDistractorModelingLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkDistractorModelingLogic);

//---------------------------------------------------------------------------
vtkDistractorModelingLogic::vtkDistractorModelingLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkDistractorModelingLogic::DataCallback);

  // Initialize Distractor values here

  this->Distractor1.RailAnchor[0] = 0;
  this->Distractor1.RailAnchor[1] = 0;
  this->Distractor1.RailAnchor[2] = 0;

  this->Distractor1.SliderAnchor[0] = 0;
  this->Distractor1.SliderAnchor[1] = 0;
  this->Distractor1.SliderAnchor[2] = 0;

  this->Distractor1.PistonAnchor[0] = 0;
  this->Distractor1.PistonAnchor[1] = 0;
  this->Distractor1.PistonAnchor[2] = 0;

  this->Distractor1.CylinderAnchor[0] = 0;
  this->Distractor1.CylinderAnchor[1] = 0;
  this->Distractor1.CylinderAnchor[2] = 0;

  this->Distractor1.newSliderAnchorX = 0.0;
  this->Distractor1.newSliderAnchorZ = 0.0;

  this->Distractor1.PistonRotationAngle_deg = 0.0;

  // Set High and random number to check if they have been initialize
  this->Distractor1.Range[0] = -1102.0;
  this->Distractor1.Range[1] = -1102.0;

  this->RailModelPath = "";
  this->SliderModelPath = "";
  this->PistonModelPath = "";
  this->CylinderModelPath = "";

  this->xmlPath = "";
}


//---------------------------------------------------------------------------
vtkDistractorModelingLogic::~vtkDistractorModelingLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkDistractorModelingLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::DataCallback(vtkObject *caller,
                                              unsigned long eid, void *clientData, void *callData)
{
  vtkDistractorModelingLogic *self = reinterpret_cast<vtkDistractorModelingLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkDistractorModelingLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::MoveDistractor(double value,
                                                vtkMRMLModelNode* Slider, vtkMRMLLinearTransformNode* SliderTransformationNode,
                                                vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode,
                                                vtkMRMLModelNode* Cylinder, vtkMRMLLinearTransformNode* CylinderTransformationNode)
{
  this->MoveSlider(value,Slider,SliderTransformationNode);

  this->MovePiston(value,Piston,PistonTransformationNode);

  this->MoveCylinder(value,Cylinder,CylinderTransformationNode);
}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::MoveSlider(double value, vtkMRMLModelNode* Slider, vtkMRMLLinearTransformNode* SliderTransformationNode)
{
  double SliderCenter[3];
  Slider->GetPolyData()->GetCenter(SliderCenter);

  vtkMatrix4x4* SliderTransformationMatrix = vtkMatrix4x4::New();
  SliderTransformationMatrix->Identity();

  vtkTransform* SliderTranslation = vtkTransform::New();
  SliderTranslation->Translate(SliderCenter[0] + this->Distractor1.RailAnchor[0],
                               SliderCenter[1] + this->Distractor1.RailAnchor[1],
                               SliderCenter[2] + this->Distractor1.RailAnchor[2]);


  vtkTransform* SliderRotation = vtkTransform::New();
  SliderRotation->RotateY(value);


  vtkTransform* SliderInvertTranslation = vtkTransform::New();
  SliderInvertTranslation->Translate(-SliderCenter[0] - this->Distractor1.RailAnchor[0],
                                     -SliderCenter[1] - this->Distractor1.RailAnchor[1],
                                     -SliderCenter[2] - this->Distractor1.RailAnchor[2]);

  SliderInvertTranslation->PostMultiply();
  SliderInvertTranslation->Concatenate(SliderRotation);
  SliderInvertTranslation->PostMultiply();
  SliderInvertTranslation->Concatenate(SliderTranslation);
  SliderInvertTranslation->GetMatrix(SliderTransformationMatrix);

  SliderTransformationNode->SetAndObserveMatrixTransformToParent(SliderTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Slider->GetParentTransformNode() && Slider->GetParentTransformNode()!=SliderTransformationNode)
    {
    SliderTransformationNode->SetAndObserveTransformNodeID(Slider->GetParentTransformNode()->GetID());
    }


  this->Distractor1.newSliderAnchorX =
    SliderTransformationMatrix->GetElement(0,0)*this->Distractor1.SliderAnchor[0]+
    SliderTransformationMatrix->GetElement(0,1)*this->Distractor1.SliderAnchor[1]+
    SliderTransformationMatrix->GetElement(0,2)*this->Distractor1.SliderAnchor[2]+
    SliderTransformationMatrix->GetElement(0,3)*1;

  this->Distractor1.newSliderAnchorZ =
    SliderTransformationMatrix->GetElement(2,0)*this->Distractor1.SliderAnchor[0]+
    SliderTransformationMatrix->GetElement(2,1)*this->Distractor1.SliderAnchor[1]+
    SliderTransformationMatrix->GetElement(2,2)*this->Distractor1.SliderAnchor[2]+
    SliderTransformationMatrix->GetElement(2,3)*1;


  // Delete
  SliderTranslation->Delete();
  SliderRotation->Delete();
  SliderInvertTranslation->Delete();
  SliderTransformationMatrix->Delete();
}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::MovePiston(double value, vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode)
{
  double PistonAnchor[3] = {this->Distractor1.PistonAnchor[0],
                            this->Distractor1.PistonAnchor[1],
                            this->Distractor1.PistonAnchor[2]};

  vtkTransform* PistonTranslation = vtkTransform::New();
  PistonTranslation->Translate(PistonAnchor[0],PistonAnchor[1],PistonAnchor[2]);

  double gamma = atan2((this->Distractor1.newSliderAnchorZ-this->Distractor1.PistonAnchor[2]),
                       (this->Distractor1.newSliderAnchorX-this->Distractor1.PistonAnchor[0]));

  double beta = atan2((this->Distractor1.SliderAnchor[2]-this->Distractor1.PistonAnchor[2]),
                      (this->Distractor1.SliderAnchor[0]-this->Distractor1.PistonAnchor[0]));

  this->Distractor1.PistonRotationAngle_deg = ((beta-gamma)*180/M_PI);

  vtkTransform* PistonRotation = vtkTransform::New();
  PistonRotation->RotateY(this->Distractor1.PistonRotationAngle_deg);

  vtkMatrix4x4* PistonTransformationMatrix = vtkMatrix4x4::New();
  PistonTransformationMatrix->Identity();

  vtkTransform* PistonInvertTranslation = vtkTransform::New();
  PistonInvertTranslation->Translate(-PistonAnchor[0],-PistonAnchor[1],-PistonAnchor[2]);
  PistonInvertTranslation->PostMultiply();
  PistonInvertTranslation->Concatenate(PistonRotation);
  PistonInvertTranslation->PostMultiply();
  PistonInvertTranslation->Concatenate(PistonTranslation);
  PistonInvertTranslation->GetMatrix(PistonTransformationMatrix);

  PistonTransformationNode->SetAndObserveMatrixTransformToParent(PistonTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Piston->GetParentTransformNode() && Piston->GetParentTransformNode()!=PistonTransformationNode)
    {
    PistonTransformationNode->SetAndObserveTransformNodeID(Piston->GetParentTransformNode()->GetID());
    }


  // Delete
  PistonTranslation->Delete();
  PistonRotation->Delete();
  PistonInvertTranslation->Delete();
  PistonTransformationMatrix->Delete();
}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::MoveCylinder(double value, vtkMRMLModelNode* Cylinder, vtkMRMLLinearTransformNode* CylinderTransformationNode)
{
  double tx = this->Distractor1.newSliderAnchorX - this->Distractor1.CylinderAnchor[0];
  double ty = 0.0;
  double tz = this->Distractor1.newSliderAnchorZ - this->Distractor1.CylinderAnchor[2];

  vtkTransform* CylinderInvertTranslation = vtkTransform::New();
  CylinderInvertTranslation->Translate(this->Distractor1.newSliderAnchorX,
                                       this->Distractor1.CylinderAnchor[1],
                                       this->Distractor1.newSliderAnchorZ);

  vtkTransform* CylinderTranslation = vtkTransform::New();
  CylinderTranslation->Translate(-this->Distractor1.newSliderAnchorX,
                                 -this->Distractor1.CylinderAnchor[1],
                                 -this->Distractor1.newSliderAnchorZ);

  vtkTransform* CylinderRotation = vtkTransform::New();
  CylinderRotation->RotateY(this->Distractor1.PistonRotationAngle_deg);

  vtkMatrix4x4* CylinderTransformationMatrix = vtkMatrix4x4::New();
  CylinderTransformationMatrix->Identity();

  vtkTransform* CylinderTransformation = vtkTransform::New();
  CylinderTransformation->Translate(tx,ty,tz);
  CylinderTransformation->PreMultiply();
  CylinderTransformation->Concatenate(CylinderTranslation);
  CylinderTransformation->PostMultiply();
  CylinderTransformation->Concatenate(CylinderRotation);
  CylinderTransformation->PostMultiply();
  CylinderTransformation->Concatenate(CylinderInvertTranslation);
  CylinderTransformation->GetMatrix(CylinderTransformationMatrix);

  CylinderTransformationNode->SetAndObserveMatrixTransformToParent(CylinderTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Cylinder->GetParentTransformNode() && Cylinder->GetParentTransformNode()!=CylinderTransformationNode)
    {
    CylinderTransformationNode->SetAndObserveTransformNodeID(Cylinder->GetParentTransformNode()->GetID());
    }


  // Delete
  CylinderInvertTranslation->Delete();
  CylinderTranslation->Delete();
  CylinderRotation->Delete();
  CylinderTransformationMatrix->Delete();
  CylinderTransformation->Delete();
}

void vtkDistractorModelingLogic::OpenDistractorFile(const char* xmlFile)
{
  // Test Read XML
  std::filebuf *fbuf;
  long size;
  char *buffer;

  std::stringstream DistractorPath;
  DistractorPath << xmlFile;
  this->xmlPath = DistractorPath.str();
  DistractorPath << "/Distractor.xml";

  std::cerr << "(INSIDE) " << this->xmlPath << std::endl;

  // Avoid compiler warning about last argument of XML_Parse is NULL
  int dummy = 0;

  std::ifstream file_in(DistractorPath.str().c_str(), ios::in);

  XML_Parser parser = XML_ParserCreate(NULL);
  XML_SetUserData(parser, this);
  XML_SetElementHandler(parser, startElement, endElement);
  fbuf = file_in.rdbuf();
  size = fbuf->pubseekoff (0,ios::end,ios::in);
  fbuf->pubseekpos (0,ios::in);
  buffer = new char[size];
  fbuf->sgetn (buffer, size);
  XML_Parse(parser, buffer, size, dummy);

  file_in.close();
  XML_ParserFree(parser);
  free(buffer);

}

void vtkDistractorModelingLogic::startElement(void *userData, const XML_Char *name, const XML_Char **atts) {
  int i;
  vtkDistractorModelingLogic* LoadClass = (vtkDistractorModelingLogic*)userData;

  for (i=0; atts[i] ; i+=2)
    {

    // VTK Files
    if(!strcmp(atts[i],"VTKFile"))
      {
      std::stringstream pathFile;
      pathFile << LoadClass->xmlPath << "/" << atts[i+1];

      if(!strcmp(name,"Rail"))
        {
        LoadClass->RailModelPath = pathFile.str();
        }
      else if(!strcmp(name,"Slider"))
        {
        LoadClass->SliderModelPath = pathFile.str();
        }
      else if(!strcmp(name,"Piston"))
        {
        LoadClass->PistonModelPath = pathFile.str();
        }
      else if(!strcmp(name,"Cylinder"))
        {
        LoadClass->CylinderModelPath = pathFile.str();
        }
      }

    // Anchors
    if(!strcmp(atts[i],"Anchor"))
      {
      std::stringstream anchor;
      anchor << atts[i+1];
      std::string buf;
      std::vector<std::string> tokens;

      while(anchor >> buf)
        {
        tokens.push_back(buf);
        }

      if(!strcmp(name,"Rail"))
        {
        LoadClass->Distractor1.RailAnchor[0] = atof(tokens[0].c_str());
        LoadClass->Distractor1.RailAnchor[1] = atof(tokens[1].c_str());
        LoadClass->Distractor1.RailAnchor[2] = atof(tokens[2].c_str());
        }
      else if(!strcmp(name,"Slider"))
        {
        LoadClass->Distractor1.SliderAnchor[0] = atof(tokens[0].c_str());
        LoadClass->Distractor1.SliderAnchor[1] = atof(tokens[1].c_str());
        LoadClass->Distractor1.SliderAnchor[2] = atof(tokens[2].c_str());
        }
      else if(!strcmp(name,"Piston"))
        {
        LoadClass->Distractor1.PistonAnchor[0] = atof(tokens[0].c_str());
        LoadClass->Distractor1.PistonAnchor[1] = atof(tokens[1].c_str());
        LoadClass->Distractor1.PistonAnchor[2] = atof(tokens[2].c_str());
        }
      else if(!strcmp(name,"Cylinder"))
        {
        LoadClass->Distractor1.CylinderAnchor[0] = atof(tokens[0].c_str());
        LoadClass->Distractor1.CylinderAnchor[1] = atof(tokens[1].c_str());
        LoadClass->Distractor1.CylinderAnchor[2] = atof(tokens[2].c_str());
        }
      }

    // Range of motion
    if(!strcmp(name,"Range"))
      {
      if(!strcmp(atts[i],"min"))
        {
        LoadClass->Distractor1.Range[0] = atof(atts[i+1]);
        }
      else if(!strcmp(atts[i],"max"))
        {
        LoadClass->Distractor1.Range[1] = atof(atts[i+1]);
        }
      }


    }
}

void vtkDistractorModelingLogic::endElement(void *userData, const XML_Char *name) {
}

void vtkDistractorModelingLogic::LoadModel(const char* modelPath)
{
}
