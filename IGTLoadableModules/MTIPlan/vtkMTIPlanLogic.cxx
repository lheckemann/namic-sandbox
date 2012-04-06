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

#include "vtkMTIPlanLogic.h"

vtkCxxRevisionMacro(vtkMTIPlanLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkMTIPlanLogic);

//---------------------------------------------------------------------------
vtkMTIPlanLogic::vtkMTIPlanLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkMTIPlanLogic::DataCallback);

  this->ReadyToDraw = false;
}


//---------------------------------------------------------------------------
vtkMTIPlanLogic::~vtkMTIPlanLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkMTIPlanLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkMTIPlanLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkMTIPlanLogic::DataCallback(vtkObject *caller,
                                   unsigned long eid, void *clientData, void *callData)
{
  vtkMTIPlanLogic *self = reinterpret_cast<vtkMTIPlanLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkMTIPlanLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkMTIPlanLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
void vtkMTIPlanLogic::ParseMTIPlanFile(const char* filename)
{
  std::filebuf *fbuf;
  long size;
  char* buffer;

  // Avoid compiler warning about last argument of XML_Parse is NULL
  int dummy = 0;

  std::ifstream file_in(filename, ios::in);

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

  this->SetReadyToDraw(true);
}

//---------------------------------------------------------------------------
void vtkMTIPlanLogic::startElement(void *userData, const XML_Char *name, const XML_Char **atts) {
  vtkMTIPlanLogic* LoadClass = (vtkMTIPlanLogic*)userData;

  if(!strcmp(name,"target"))
    {
    if(!strcmp(atts[0],"coordinates"))
      {
      LoadClass->vTargets.push_back(atts[1]);
      }
    }

  if(!strcmp(name,"entry_point"))
    {
    if(!strcmp(atts[0],"coordinates"))
      {
      LoadClass->vEntryPoint.push_back(atts[1]);
      }
    }

  if(!strcmp(name,"interventional_plan"))
    {
    if(!strcmp(atts[0],"patient_name"))
      {
      LoadClass->CaseName = std::string(atts[1]);
      }
    }
}


//---------------------------------------------------------------------------
void vtkMTIPlanLogic::endElement(void *userData, const XML_Char *name)
{
  vtkMTIPlanLogic* LoadClass = (vtkMTIPlanLogic*)userData;

  if(!strcmp(name,"trajectory"))
    {
    if(LoadClass->vEntryPoint.size()==1)
      {
      std::stringstream inputEP;
      inputEP << LoadClass->vEntryPoint[0];
      std::string bufEP;
      std::vector<std::string> tokensEP;
      while(inputEP >> bufEP)
        {
        tokensEP.push_back(bufEP);
        }

      for(unsigned int i=0; i < LoadClass->vTargets.size(); i++)
        {
        std::stringstream inputT;
        inputT << LoadClass->vTargets[i];
        std::string bufT;
        std::vector<std::string> tokensT;
        while(inputT >> bufT)
          {
          tokensT.push_back(bufT);
          }

        Trajectory traj;
        traj.EntryPoint[0] = atof(tokensEP[0].c_str());
        traj.EntryPoint[1] = atof(tokensEP[1].c_str());
        traj.EntryPoint[2] = atof(tokensEP[2].c_str());
        traj.Target[0] = atof(tokensT[0].c_str());
        traj.Target[1] = atof(tokensT[1].c_str());
        traj.Target[2] = atof(tokensT[2].c_str());

        LoadClass->vTrajectories.push_back(traj);
        }
      }
    LoadClass->vTargets.clear();
    LoadClass->vEntryPoint.clear();
    }
}






