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

  // Initialize Distractor paths here
  this->RailModelPath = "";
  this->SliderModelPath = "";
  this->PistonModelPath = "";
  this->CylinderModelPath = "";

  this->xmlPath = "";

  this->DistractorList = vtkCollection::New();

}


//---------------------------------------------------------------------------
vtkDistractorModelingLogic::~vtkDistractorModelingLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  if(this->DistractorList)
    {
    this->DistractorList->RemoveAllItems();
    this->DistractorList->Delete();
    }

  if(this->DistractorSelected)
    {
    this->SetDistractorSelected(NULL);
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
  SliderTranslation->Translate(SliderCenter[0] + this->GetDistractorSelected()->GetRailAnchor()[0],
                               SliderCenter[1] + this->GetDistractorSelected()->GetRailAnchor()[1],
                               SliderCenter[2] + this->GetDistractorSelected()->GetRailAnchor()[2]);


  vtkTransform* SliderRotation = vtkTransform::New();
  SliderRotation->RotateY(value);


  vtkTransform* SliderInvertTranslation = vtkTransform::New();
  SliderInvertTranslation->Translate(-SliderCenter[0] - this->GetDistractorSelected()->GetRailAnchor()[0],
                                     -SliderCenter[1] - this->GetDistractorSelected()->GetRailAnchor()[1],
                                     -SliderCenter[2] - this->GetDistractorSelected()->GetRailAnchor()[2]);

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


  double newSliderAnchorX =
    SliderTransformationMatrix->GetElement(0,0)*this->GetDistractorSelected()->GetSliderAnchor()[0]+
    SliderTransformationMatrix->GetElement(0,1)*this->GetDistractorSelected()->GetSliderAnchor()[1]+
    SliderTransformationMatrix->GetElement(0,2)*this->GetDistractorSelected()->GetSliderAnchor()[2]+
    SliderTransformationMatrix->GetElement(0,3)*1;

  double newSliderAnchorZ =
    SliderTransformationMatrix->GetElement(2,0)*this->GetDistractorSelected()->GetSliderAnchor()[0]+
    SliderTransformationMatrix->GetElement(2,1)*this->GetDistractorSelected()->GetSliderAnchor()[1]+
    SliderTransformationMatrix->GetElement(2,2)*this->GetDistractorSelected()->GetSliderAnchor()[2]+
    SliderTransformationMatrix->GetElement(2,3)*1;

  double newSliderAnchor[3] = {newSliderAnchorX, 0.0, newSliderAnchorZ};

  this->GetDistractorSelected()->SetNewSliderAnchor(newSliderAnchor);

  // Delete
  SliderTranslation->Delete();
  SliderRotation->Delete();
  SliderInvertTranslation->Delete();
  SliderTransformationMatrix->Delete();
}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::MovePiston(double value, vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode)
{
  double PistonAnchor[3] = {this->GetDistractorSelected()->GetPistonAnchor()[0],
                            this->GetDistractorSelected()->GetPistonAnchor()[1],
                            this->GetDistractorSelected()->GetPistonAnchor()[2]};

  vtkTransform* PistonTranslation = vtkTransform::New();
  PistonTranslation->Translate(PistonAnchor[0],PistonAnchor[1],PistonAnchor[2]);

  double* nSliderAnchor = this->GetDistractorSelected()->GetNewSliderAnchor();

  double gamma = atan2((nSliderAnchor[2]-PistonAnchor[2]),
                       (nSliderAnchor[0]-PistonAnchor[0]));

  double beta = atan2((this->GetDistractorSelected()->GetSliderAnchor()[2]-PistonAnchor[2]),
                      (this->GetDistractorSelected()->GetSliderAnchor()[0]-PistonAnchor[0]));

  this->GetDistractorSelected()->SetPistonRotationAngle_deg((beta-gamma)*180/M_PI);

  vtkTransform* PistonRotation = vtkTransform::New();
  PistonRotation->RotateY(this->GetDistractorSelected()->GetPistonRotationAngle_deg());

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
  double tx = this->GetDistractorSelected()->GetNewSliderAnchor()[0] - this->GetDistractorSelected()->GetCylinderAnchor()[0];
  double ty = 0.0;
  double tz = this->GetDistractorSelected()->GetNewSliderAnchor()[2] - this->GetDistractorSelected()->GetCylinderAnchor()[2];

  vtkTransform* CylinderInvertTranslation = vtkTransform::New();
  CylinderInvertTranslation->Translate(this->GetDistractorSelected()->GetNewSliderAnchor()[0],
                                       this->GetDistractorSelected()->GetCylinderAnchor()[1],
                                       this->GetDistractorSelected()->GetNewSliderAnchor()[2]);

  vtkTransform* CylinderTranslation = vtkTransform::New();
  CylinderTranslation->Translate(-this->GetDistractorSelected()->GetNewSliderAnchor()[0],
                                 -this->GetDistractorSelected()->GetCylinderAnchor()[1],
                                 -this->GetDistractorSelected()->GetNewSliderAnchor()[2]);

  vtkTransform* CylinderRotation = vtkTransform::New();
  CylinderRotation->RotateY(this->GetDistractorSelected()->GetPistonRotationAngle_deg());

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


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::OpenDistractorFile(const char* xmlFile)
{
  this->DistractorObject = vtkDistractorDefinition::New();

  // Test Read XML
  std::filebuf *fbuf;
  long size;
  char *buffer;

  std::stringstream DistractorPath;
  DistractorPath << xmlFile;
  this->xmlPath = DistractorPath.str();
  DistractorPath << "/Distractor.xml";

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


  this->GetDistractorList()->AddItem(this->DistractorObject);

  if(this->GetDistractorSelected())
    {
    this->GetDistractorSelected()->Hide();
    }

  this->SetDistractorSelected(this->DistractorObject);
  this->GetDistractorSelected()->Show();
  this->DistractorObject->Delete();


}

//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::startElement(void *userData, const XML_Char *name, const XML_Char **atts) {
  int i;
  vtkDistractorModelingLogic* LoadClass = (vtkDistractorModelingLogic*)userData;

  for (i=0; atts[i] ; i+=2)
    {

    // Distractor Name
    if(!strcmp(name,"Distractor"))
      {
      if(!strcmp(atts[i],"name"))
        {
        std::stringstream DName;
        DName << atts[i+1];

        LoadClass->DistractorObject->SetDistractorName(DName.str().c_str());
        LoadClass->DistName = DName.str();
        }
      }

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
        double rAnchor[3] = {atof(tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str())};
        LoadClass->DistractorObject->SetRailAnchor(rAnchor);
        }
      else if(!strcmp(name,"Slider"))
        {
        double sAnchor[3] = {atof(tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str())};
        LoadClass->DistractorObject->SetSliderAnchor(sAnchor);
        }
      else if(!strcmp(name,"Piston"))
        {
        double pAnchor[3] = {atof(tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str())};
        LoadClass->DistractorObject->SetPistonAnchor(pAnchor);
        }
      else if(!strcmp(name,"Cylinder"))
        {
        double cAnchor[3] = {atof(tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str())};
        LoadClass->DistractorObject->SetCylinderAnchor(cAnchor);
        }
      }

    // Range of motion
    if(!strcmp(name,"Range"))
      {
      if(!strcmp(atts[i],"min"))
        {
        LoadClass->DistractorObject->SetRangeMin(atof(atts[i+1]));
        }
      else if(!strcmp(atts[i],"max"))
        {
        LoadClass->DistractorObject->SetRangeMax(atof(atts[i+1]));
        }
      }
    }

}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::endElement(void *userData, const XML_Char *name) {
}


//---------------------------------------------------------------------------
void vtkDistractorModelingLogic::MoveBones(vtkMRMLModelNode* BonePlateModel, vtkMRMLLinearTransformNode* BonePlateTransform,
                                           double value)
{
  // Create BoneAnchor and newBoneAnchor on Distractor1 structure
  // Calculate newBoneAnchor = R*BoneAnchor
  // Translate Rotation Center of the bone (BoneAnchor)
  // Rotate Bone as Slider
  // Translate Rotation Center of the bone back to new position (newBoneAnchor)
  // Update BoneAnchor

  double BoneCenter[3];
  BonePlateModel->GetPolyData()->GetCenter(BoneCenter);

  vtkMatrix4x4* BoneTransformationMatrix = BonePlateTransform->GetMatrixTransformToParent();

  double BoneAnchor[3] = {BoneCenter[1],//+BoneTransformationMatrix->GetElement(0,3),
                          BoneCenter[2],//+BoneTransformationMatrix->GetElement(1,3),
                          BoneCenter[3]};//+BoneTransformationMatrix->GetElement(2,3)};

  vtkTransform* BoneTranslation = vtkTransform::New();
  BoneTranslation->Translate(BoneAnchor[0] + this->GetDistractorSelected()->GetRailAnchor()[0],
                             BoneAnchor[1] + this->GetDistractorSelected()->GetRailAnchor()[1],
                             BoneAnchor[2] + this->GetDistractorSelected()->GetRailAnchor()[2]);

  vtkTransform* BoneRotation = vtkTransform::New();
  BoneRotation->RotateY(value);


  double newBoneAnchorX =
    BoneRotation->GetMatrix()->GetElement(0,0)*BoneAnchor[0]+
    BoneRotation->GetMatrix()->GetElement(0,1)*BoneAnchor[1]+
    BoneRotation->GetMatrix()->GetElement(0,2)*BoneAnchor[2]+
    BoneRotation->GetMatrix()->GetElement(0,3)*1;

  double newBoneAnchorY =
    BoneRotation->GetMatrix()->GetElement(1,0)*BoneAnchor[0]+
    BoneRotation->GetMatrix()->GetElement(1,1)*BoneAnchor[1]+
    BoneRotation->GetMatrix()->GetElement(1,2)*BoneAnchor[2]+
    BoneRotation->GetMatrix()->GetElement(1,3)*1;

  double newBoneAnchorZ =
    BoneRotation->GetMatrix()->GetElement(2,0)*BoneAnchor[0]+
    BoneRotation->GetMatrix()->GetElement(2,1)*BoneAnchor[1]+
    BoneRotation->GetMatrix()->GetElement(2,2)*BoneAnchor[2]+
    BoneRotation->GetMatrix()->GetElement(2,3)*1;


  vtkTransform* BoneInvertTranslation = vtkTransform::New();
  BoneInvertTranslation->Translate(-newBoneAnchorX - this->GetDistractorSelected()->GetRailAnchor()[0],
                                   -newBoneAnchorY - this->GetDistractorSelected()->GetRailAnchor()[1],
                                   -newBoneAnchorZ - this->GetDistractorSelected()->GetRailAnchor()[2]);

  BoneInvertTranslation->PostMultiply();
  BoneInvertTranslation->Concatenate(BoneRotation);
  BoneInvertTranslation->PostMultiply();
  BoneInvertTranslation->Concatenate(BoneTranslation);
  BoneInvertTranslation->GetMatrix(BoneTransformationMatrix);

  BoneTranslation->Delete();
  BoneRotation->Delete();
  BoneInvertTranslation->Delete();

}

