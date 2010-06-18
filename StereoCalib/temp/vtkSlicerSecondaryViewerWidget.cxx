/*==========================================================================
 
 Portions (c) Copyright 2010 Atsushi Yamada (Fujimoto Lab, Nagoya Institute of Technology (NIT)) 
                 and M. Komura (NIT) All Rights Reserved.
 
 Acknowledgement: K. Chinzei (AIST), Y. Hayashi (Nagoya Univ.), T. Takeuchi (SFC Corp.), H. Liu (BWH), J. Tokuda (BWH), N. Hata (BWH), and H. Fujimoto (NIT). 
 CMakeLists.txt, FindOpenCV.cmake, and FindOpenIGTLink.cmake are contributions of K. Chinzei(AIST) and T. Takeuchi (SFC Corp.).

 !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!
 This module is based on the "Secondary Window" module by J. Tokuda (BWH).
 !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!
 
 See README.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 
 Program:   SecondaryWindowWithOpenCV
 Module:    $HeadURL: $
 Date:      $Date:01/25/2010 $
 Version:   $Revision: $
 
 ==========================================================================*/

#include <string>
#include <sstream>

#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkSlicerSecondaryViewerWidget.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerGUILayout.h"
#include "vtkSlicerViewerInteractorStyle.h"

#include "vtkProp3D.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkFollower.h"
#include "vtkProperty.h"
#include "vtkTexture.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkPolyDataMapper.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkOutlineSource.h"
#include "vtkVectorText.h"
#include "vtkRenderWindow.h"
#include "vtkImplicitBoolean.h"
#include "vtkPlane.h"
#include "vtkClipPolyData.h"

#include "vtkMRMLDisplayableNode.h"
#include "vtkMRMLDisplayNode.h"
#include "vtkMRMLTransformNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLClipModelsNode.h"
#include "vtkMRMLModelHierarchyNode.h"
#include "vtkMRMLColorNode.h"
#include "vtkMRMLProceduralColorNode.h"

#include "vtkSlicerModelHierarchyLogic.h"

#include "vtkKWWidget.h"

// for picking
#include "vtkWorldPointPicker.h"
#include "vtkPropPicker.h"
#include "vtkCellPicker.h"
#include "vtkPointPicker.h"


#include "vtkSlicerBoxWidget2.h"
#include "vtkSlicerBoxRepresentation.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSlicerSecondaryViewerWidget );
vtkCxxRevisionMacro ( vtkSlicerSecondaryViewerWidget, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
vtkSlicerSecondaryViewerWidget::vtkSlicerSecondaryViewerWidget ( )
{

}




//---------------------------------------------------------------------------
vtkSlicerSecondaryViewerWidget::~vtkSlicerSecondaryViewerWidget ( )
{
}

//---------------------------------------------------------------------------
void vtkSlicerSecondaryViewerWidget::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf ( os, indent );

    os << indent << "vtkSlicerSecondaryViewerWidget: " << this->GetClassName ( ) << "\n";

}

//---------------------------------------------------------------------------
void vtkSlicerSecondaryViewerWidget::ProcessMRMLEvents ( vtkObject *caller,
                                                unsigned long event, 
                                                void *callData )
{ 
  if (this->ProcessingMRMLEvent != 0 )
    {
    return;
    }
}
//---------------------------------------------------------------------------
void vtkSlicerSecondaryViewerWidget::CreateWidget ( )
{
  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }
  
  // Call the superclass to create the whole widget
  
  this->Superclass::CreateWidget();

  this->ViewerFrame = vtkKWFrame::New ( );
  this->ViewerFrame->SetParent ( this->GetParent ( ) );
  this->ViewerFrame->Create ( );
  this->MainViewer = vtkKWRenderWidget::New ( );
  
  this->MainViewer->SetParent (this->ViewerFrame );
  this->MainViewer->Create ( );
  // vtkRenderWindowInteractor *rwi = this->MainViewer->GetRenderWindowInteractor();
  // if (rwi)
  //   {
  //   vtkSlicerViewerInteractorStyle *iStyle = vtkSlicerViewerInteractorStyle::New();
  //   iStyle->SetViewerWidget( this );
  //   if (this->ApplicationLogic != NULL)
  //     {
  //     iStyle->SetApplicationLogic ( this->ApplicationLogic );
  //     }
  //   else
  //     {
  //     vtkDebugMacro("Not setting interactor style's application logic to null.");
  //     }

  //   rwi->SetInteractorStyle (iStyle);
  //   iStyle->Delete();
  //   }
}

int vtkSlicerSecondaryViewerWidget::GetDisplayedModelsVisibility(vtkMRMLDisplayNode *model)
{
  int visibility = 1;
  
  std::map<std::string, int>::iterator iter;
  iter = this->DisplayedVisibility.find(model->GetID());
  if (iter != this->DisplayedVisibility.end())
    {
    visibility = iter->second;
    }

  return visibility;
}


//---------------------------------------------------------------------------
void vtkSlicerSecondaryViewerWidget::Render()
{
  // *** added code to check the RenderState and restore to whatever it
  // was before the specific request to render, instead of just setting
  // renderState to OFF.
  int currentRenderState = this->MainViewer->GetRenderState();
  this->MainViewer->RenderStateOn();
  this->MainViewer->Render();
  vtkDebugMacro("vtkSlicerViewerWidget::Render called render" << endl);
  //this->MainViewer->RenderStateOff();
  this->MainViewer->SetRenderState(currentRenderState);
  this->SetRenderPending(0);
}
