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

#include "vtkLineSource.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkActorCollection.h"
#include "vtkProperty.h"
#include "vtkColor.h"

#include "vtkBiopsyModuleLogic.h"

vtkCxxRevisionMacro(vtkBiopsyModuleLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkBiopsyModuleLogic);

//---------------------------------------------------------------------------
vtkBiopsyModuleLogic::vtkBiopsyModuleLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBiopsyModuleLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkBiopsyModuleLogic::~vtkBiopsyModuleLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkBiopsyModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkBiopsyModuleLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkBiopsyModuleLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkBiopsyModuleLogic *self = reinterpret_cast<vtkBiopsyModuleLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBiopsyModuleLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleLogic::UpdateAll()
{

}

void vtkBiopsyModuleLogic::RefreshLines(vtkMRMLFiducialListNode* fiducialListNode, vtkActorCollection* actorCollection, vtkCollection* color, vtkSlicerApplicationGUI* slicerGUI)
{
  if(fiducialListNode && slicerGUI && actorCollection && color)
    {
      if(fiducialListNode->GetNumberOfFiducials() == 2)
        {
          vtkLineSource* lineFiducials = vtkLineSource::New();
          float* p1 = fiducialListNode->GetNthFiducialXYZ(0);
          float* p2 = fiducialListNode->GetNthFiducialXYZ(1);
      
          lineFiducials->SetPoint1(p1[0],p1[1],p1[2]);
          lineFiducials->SetPoint2(p2[0],p2[1],p2[2]);
          lineFiducials->Update();
      
          vtkPolyDataMapper* lineMapper = vtkPolyDataMapper::New();
          lineMapper->SetInputConnection(lineFiducials->GetOutputPort());
      
          vtkActor* lineActor = vtkActor::New();
          lineActor->SetMapper(lineMapper);
          lineActor->GetProperty()->SetColor((double)rand()/RAND_MAX,(double)rand()/RAND_MAX,(double)rand()/RAND_MAX);         
 

          // FIXME: Be sure color is not already existing         
       
           // // Generate random color and make sure not already generated
       //   bool color_existing = false;  // To avoid compilation warning, shouldn't be defined on loop 
          //   double col[3] = {0,0,0};    // Idem
 
          //   do
       //     {
       //       col[0] = (double)(rand()/RAND_MAX);
          //       col[1] = (double)(rand()/RAND_MAX);
          //       col[2] = (double)(rand()/RAND_MAX);
 
          //       bool color_existing = false;
          //       for(int i=0; i<color->GetNumberOfItems();i++)
       //         {
       //          double* assigned_color = reinterpret_cast<double*>(color->GetItemAsObject(i));
          //           if(assigned_color[0] == col[0] && assigned_color[1] == col[1] && assigned_color[2] == col[2])
       //            {
       //              color_existing = true;
       //            }
       //         }        

       //     }
       //   while(color_existing);

        
          // lineActor->GetProperty()->SetColor(col[0],col[1],col[2]);

          // color->AddItem((vtkObject*)(col));
          actorCollection->AddItem(lineActor);
      
          slicerGUI->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(lineActor);
      
          lineActor->Delete();
          lineMapper->Delete();
          lineFiducials->Delete();
        }
    }
}





