#include "vtkKWMeshVisu.h"

#include "vtkKWApplication.h"
#include "vtkKWFrame.h"
#include "vtkKWColorTransferFunctionEditor.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithSpinButtons.h"
#include "vtkKWMenuButtonWithSpinButtonsWithLabel.h"
#include "vtkKWNotebook.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWWindow.h"
#include "vtkObjectFactory.h"
#include "vtkKWMenu.h"
#include "vtkToolkits.h"
#include "vtkColorTransferFunction.h"
#include "vtkKWPushButtonSet.h"
#include "vtkKWPushButton.h"
#include "vtkKWLoadSaveDialog.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>

// ITK-includes
#include <itkDefaultDynamicMeshTraits.h>
#include <itkSceneSpatialObject.h>
#include <itkMetaMeshConverter.h>
#include <itkMeshTovtkPolyData.h>

// VTK-includes
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkPolyDataNormals.h"
#include "vtkGlyph3D.h"
#include "vtkArrowSource.h"
#include "vtkAssignAttribute.h"
#include <assert.h>
//#include "vtkSphereSource.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMeshVisu );
vtkCxxRevisionMacro(vtkKWMeshVisu, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkKWMeshVisu::vtkKWMeshVisu ()
{
  this->vtkMesh = 0 ;
  this->meshMapper = vtkPolyDataMapper::New();
  this->meshActor = 0 ;
  this->normalFilter = vtkPolyDataNormals::New ();

  this->glyphMapper = vtkPolyDataMapper::New();
  this->glyphActor = 0 ;
  this->glyphFilter = vtkGlyph3D::New();

  this->arrowSource = vtkArrowSource::New();

}

vtkKWMeshVisu::~vtkKWMeshVisu()
{
  if ( this->vtkMesh )
    this->vtkMesh->Delete () ;
  
  if ( this->meshActor )
    this->meshActor->Delete() ;
  
  if ( this->glyphActor )
    this->glyphActor->Delete ();

  this->meshMapper->Delete ();
  this->normalFilter->Delete();
  this->glyphMapper->Delete();
  this->glyphFilter->Delete();
  this->arrowSource->Delete() ;
}

int vtkKWMeshVisu::Run(int argc, char *argv[])
{
  // Process some command-line arguments
  // The --test option here is used to run this example as a non-interactive 
  // test for software quality purposes. You can ignore it.

  int option_test = 0;
  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  args.AddArgument(
    "--test", vtksys::CommandLineArguments::NO_ARGUMENT, &option_test, "");
  args.Parse();
  
  // Create the application
  // If --test was provided, ignore all registry settings, and exit silently
  // Restore the settings that have been saved to the registry, like
  // the geometry of the user interface so far.

  vtkKWApplication *app = this->GetApplication();
  app->SetName("KWMeshVisu");
  if (option_test)
    {
    app->SetRegistryLevel(0);
    app->PromptBeforeExitOff();
    }
  app->RestoreApplicationSettingsFromRegistry();

  // Set a help link. Can be a remote link (URL), or a local file

  app->SetHelpDialogStartingPage("http://www.kwwidgets.org");

  // Add a window
  // Set 'SupportHelp' to automatically add a menu entry for the help link

  vtkKWWindow *win = vtkKWWindow::New();
  win->SupportHelpOn();
  app->AddWindow(win);
  win->Create();
  win->SecondaryPanelVisibilityOff();

  // Populate file menu

  vtkKWMenu *file_menu = win->GetFileMenu() ;
  file_menu->InsertCommand(0, "Load Mesh", this, "LoadMeshCallback");
  file_menu->InsertCommand(1, "Load 1D Attribute", this, "Load1DAttributeCallback");
  file_menu->InsertCommand(2, "Load 3D Attribute", this, "Load3DAttributeCallback");
  file_menu->InsertCommand(3, "Load Attribute Mesh", this, "LoadAttributeMeshCallback");
  file_menu->InsertCommand(4, "Save Attribute Mesh", this, "SaveAttributeMeshCallback");
  file_menu->InsertSeparator (5) ;

  // Also add push butttons for file menu options
  vtkKWPushButtonSet *filebuttons = vtkKWPushButtonSet::New();
  filebuttons->SetParent(win->GetMainPanelFrame());
  filebuttons->Create();
  filebuttons->SetPadX(2);
  filebuttons->SetPadY(2);
  filebuttons->SetBorderWidth(2);
  filebuttons->SetReliefToGroove();
  filebuttons->ExpandWidgetsOn();
  filebuttons->SetWidgetsPadY(1);

  app->Script("pack %s -expand n -fill x -padx 2 -pady 2", 
              filebuttons->GetWidgetName());

  vtkKWPushButton *pb = filebuttons->AddWidget(0);
  pb->SetText("Load Mesh");
  pb->SetCommand(this, "LoadMeshCallback");

  pb = filebuttons->AddWidget(1);
  pb->SetText("Load 1D Attribute");
  pb->SetCommand(this, "Load1DAttributeCallback");

  pb = filebuttons->AddWidget(2);
  pb->SetText("Load 3D Attribute");
  pb->SetCommand(this, "Load3DAttributeCallback");

  pb = filebuttons->AddWidget(3);
  pb->SetText("Load Attribute Mesh");
  pb->SetCommand(this, "LoadAttributeMeshCallback");

  pb = filebuttons->AddWidget(4);
  pb->SetText("Save Attribute Mesh");
  pb->SetCommand(this, "SaveAttributeMeshCallback");

  // Add a render widget, attach it to the view frame, and pack
  
  this->RenderWidget = vtkKWRenderWidget::New();
  this->RenderWidget->SetParent(win->GetViewFrame());
  this->RenderWidget->Create();

  app->Script("pack %s -expand y -fill both -anchor c -expand y", 
              this->RenderWidget->GetWidgetName());

  // Create a menu button to control the orientation

  vtkKWMenuButtonWithSpinButtonsWithLabel *colormode_menubutton = 
    vtkKWMenuButtonWithSpinButtonsWithLabel::New();

  colormode_menubutton->SetParent(win->GetMainPanelFrame());
  colormode_menubutton->Create();
  colormode_menubutton->SetLabelText("Color Mode:");
  colormode_menubutton->SetPadX(2);
  colormode_menubutton->SetPadY(2);
  colormode_menubutton->SetBorderWidth(2);
  colormode_menubutton->SetReliefToGroove();

  app->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
              colormode_menubutton->GetWidgetName());

  vtkKWMenuButton *mb = colormode_menubutton->GetWidget()->GetWidget();
  mb->AddRadioButton("P Value", this, "PValueCallback");
  mb->AddRadioButton("Distance Map", this, "DistanceMapCallback");

  // Create a transfer function editor

  this->TFuncEditor = vtkKWColorTransferFunctionEditor::New();
  this->TFuncEditor->SetParent(win->GetMainPanelFrame());
  this->TFuncEditor->Create();
  this->TFuncEditor->SetPadX(2);
  this->TFuncEditor->SetPadY(2);
  this->TFuncEditor->SetBorderWidth(2);
  this->TFuncEditor->SetReliefToGroove();

  this->TFuncEditor->DisableAddAndRemoveOn();
  this->TFuncEditor->MidPointVisibilityOff();
  this->TFuncEditor->ParameterTicksVisibilityOn();
  this->TFuncEditor->SetRangeLabelPositionToTop();
  this->TFuncEditor->SetLabelPositionToTop();

  this->TFuncEditor->SetFunctionChangingCommand (this->RenderWidget, "Render" );
  this->TFuncEditor->SetFunctionChangedCommand (this->RenderWidget, "Render" );

  app->Script("pack %s -side top -anchor nw -expand n -fill x  -padx 2 -pady 2",
              this->TFuncEditor->GetWidgetName());

  // Create default color transfer functions for pvalue and distance map

  this->PValueTFunc = vtkColorTransferFunction::New();
  this->PValueTFunc->SetColorSpaceToHSV();
  this->PValueTFunc->AddHSVPoint(0, 0, 1, 1);
  this->PValueTFunc->AddHSVPoint(0.05, 0.27, 0.6, 0.97, 0, 1);
  this->PValueTFunc->AddHSVPoint(1, 0.6, 1, 1);

  this->DistanceMapTFunc = vtkColorTransferFunction::New();
  this->DistanceMapTFunc->SetColorSpaceToHSV();
  this->UpdateDistanceMapTFunc();

  // Set default color mode

  mb->SetValue("P Value");
  this->PValueCallback ();

  // Start the application
  // If --test was provided, do not enter the event loop and run this example
  // as a non-interactive test for software quality purposes.

  int ret = 0;
  win->Display();
  if (!option_test)
    {
    app->Start(argc, argv);
    ret = app->GetExitStatus();
    }
  win->Close();

  // Deallocate and exit

  colormode_menubutton->Delete();
  this->RenderWidget->Delete();
  this->TFuncEditor->Delete();
  this->DistanceMapTFunc->Delete();
  this->PValueTFunc->Delete();
  filebuttons->Delete();
  win->Delete();

  return ret;
}

//----------------------------------------------------------------------------
void vtkKWMeshVisu::LoadMeshCallback()
{
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;

  dialog->SetParent(this->RenderWidget->GetParentWindow()) ;
  dialog->Create();
  this->GetApplication()->RetrieveDialogLastPathRegistryValue(dialog, "DataPath");
  dialog->SetTitle ("Load Mesh");
  dialog->SetFileTypes ("{{ITK Mesh File} {.meta}}");
  dialog->SetDefaultExtension (".meta");
  

  if ( dialog->Invoke () == 0 )
  {
    return ;
  }

  this->GetApplication()->SaveDialogLastPathRegistryValue(dialog, "DataPath");

  typedef double real ;
  typedef itk::DefaultDynamicMeshTraits < real, 3, 3, real, real > MeshTraitsType ; 
  typedef itk::Mesh < real, 3, MeshTraitsType > itkMeshType ;
  typedef itk::MeshSpatialObject < itkMeshType > itkMeshSOType ;
  typedef itk::MetaMeshConverter < 3, real, MeshTraitsType > MeshConverterType ;

  // read the data in itk format
  MeshConverterType * itkConverter = new MeshConverterType() ;
  itkMeshSOType::Pointer meshSO = itkConverter->ReadMeta (dialog->GetFileName()) ;
  itkMeshType::Pointer mesh = meshSO->GetMesh() ;
  delete (itkConverter);
  //assert (mesh->GetNumberOfPoints() > 0 ) ;

  // convert to vtk format
  itkMeshTovtkPolyData * ITKVTKConverter = new itkMeshTovtkPolyData;
  ITKVTKConverter->SetInput ( mesh ) ;
  this->vtkMesh = ITKVTKConverter->GetOutput () ;
  delete (ITKVTKConverter);
  //assert (vtkMesh->GetNumberOfPoints() > 0 ) ;
  
  //vtkSphereSource *source = vtkSphereSource::New();

  this->normalFilter->SetInput (this->vtkMesh);

  // setup vtk rendering
  this->meshMapper->SetInput(this->normalFilter->GetOutput());
  //this->meshMapper->SetInput(source->GetOutput());
  this->meshMapper->Update() ;
  
  this->meshActor = vtkActor::New();
  this->meshActor->SetMapper(this->meshMapper);

  this->glyphFilter->SetInput(this->meshMapper->GetInput());
  this->glyphFilter->SetSource(this->arrowSource->GetOutput());
  //this->glyphFilter->SetScaleModeToScaleByVector();
  this->glyphFilter->SetScaleModeToDataScalingOff();
  //this->glyphFilter->SetScaleFactor(2);
  this->glyphFilter->SetVectorModeToUseVector();
  this->glyphFilter->SetColorModeToColorByVector();
  this->glyphFilter->OrientOn ();

  this->glyphMapper->SetInput(this->glyphFilter->GetOutput());
  
  this->glyphActor = vtkActor::New();
  this->glyphActor->SetMapper (this->glyphMapper);

  this->RenderWidget->AddViewProp (this->meshActor) ;
  
  this->RenderWidget->ResetCamera();
  this->RenderWidget->Render();  
 

  dialog->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMeshVisu::Load1DAttributeCallback()
{
  int nPts = this->vtkMesh->GetNumberOfPoints() ;
  double data ;
  vtkFloatArray *scalars = vtkFloatArray::New() ;

  // get the file name for reading the attributes 
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;

  dialog->SetParent(this->RenderWidget->GetParentWindow()) ;
  dialog->Create();
  this->GetApplication()->RetrieveDialogLastPathRegistryValue(dialog, "DataPath");
  dialog->SetTitle ("Load 1D Attributes");
  dialog->SetFileTypes ("{{Attribute File} {.txt}}");
  dialog->SetDefaultExtension (".txt");
  
  if ( dialog->Invoke () == 0 )
  {
    return ;
  }

  this->GetApplication()->SaveDialogLastPathRegistryValue(dialog, "DataPath");

  // read in the file
  std::ifstream attrFile ;
  attrFile.open ( dialog->GetFileName() ) ;

  // check the header
  int nPtsFile, nDim ;
  attrFile >> nPtsFile >> nDim ;
  assert ( nPtsFile == nPts ) ;
  assert ( nDim == 1 ) ;

  for (int i = 0 ; i < nPts ; i++ )
  {
    attrFile >> data ;
    scalars->InsertTuple1 (i, data) ;
  }

  attrFile.close () ;

  // make the final connections
  
  this->vtkMesh->GetPointData()->SetScalars ( scalars ) ;
  this->UpdateDistanceMapTFunc();  
  this->RenderWidget->RemoveViewProp(this->glyphActor);
  this->meshMapper->ScalarVisibilityOn ();

  this->RenderWidget->Render();
  dialog->Delete ();
  scalars->Delete ();
}

//----------------------------------------------------------------------------
void vtkKWMeshVisu::Load3DAttributeCallback()
{
  int nPts = this->vtkMesh->GetNumberOfPoints() ;
  float x[3] ;
  vtkFloatArray *vectors = vtkFloatArray::New() ;
  
  // get the file name for reading the attributes 
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;

  dialog->SetParent(this->RenderWidget->GetParentWindow()) ;
  dialog->Create();
  this->GetApplication()->RetrieveDialogLastPathRegistryValue(dialog, "DataPath");
  dialog->SetTitle ("Load 3D Attributes");
  dialog->SetFileTypes ("{{Attribute File} {.txt}}");
  dialog->SetDefaultExtension (".txt");
  
  if ( dialog->Invoke () == 0 )
  {
    return ;
  }

  this->GetApplication()->SaveDialogLastPathRegistryValue(dialog, "DataPath");

  // read in the file
  std::ifstream attrFile ;
  attrFile.open ( dialog->GetFileName() ) ;


  // check the header
  int nPtsFile, nDim ;
  attrFile >> nPtsFile >> nDim ;
  assert ( nPtsFile == nPts ) ;
  assert ( nDim == 3 ) ;


  vectors->SetNumberOfComponents (3);

  for (int i = 0 ; i < nPts ; i++ )
  {
      attrFile >> x[0] >> x[1] >> x[2] ;    
      //double angle = (2.5 - x[0] ) * 1.58 ;
      //vectors->InsertNextTuple3 (cos(angle), sin(angle), 0);
      vectors->InsertNextTuple3 (x[0], x[1], x[2]) ;
  }

  attrFile.close () ;
  
  this->vtkMesh->GetPointData()->SetVectors ( vectors ) ;
  
  this->UpdateDistanceMapTFunc();  
  this->RenderWidget->AddViewProp(this->glyphActor);
  this->meshMapper->ScalarVisibilityOff ();
 
  this->RenderWidget->Render();
  dialog->Delete();
  vectors->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMeshVisu::LoadAttributeMeshCallback()
{
}

//----------------------------------------------------------------------------
void vtkKWMeshVisu::SaveAttributeMeshCallback()
{
}

//----------------------------------------------------------------------------
void vtkKWMeshVisu::PValueCallback()
{
  this->TFuncEditor->SetColorTransferFunction (this->PValueTFunc) ;
  this->TFuncEditor->LockEndPointsParameterOn();
  this->TFuncEditor->SetLabelText ("P-Value Color Map");
  this->meshMapper->SetLookupTable (this->PValueTFunc) ;
  this->meshMapper->ScalarVisibilityOn ();
  this->meshMapper->SetColorModeToMapScalars ();

  this->TFuncEditor->SetWholeParameterRange(0.0, 1.0);
  this->RenderWidget->Render();
  
}

//----------------------------------------------------------------------------
void vtkKWMeshVisu::DistanceMapCallback()
{
  this->TFuncEditor->SetColorTransferFunction (this->DistanceMapTFunc) ;
  this->TFuncEditor->LockEndPointsParameterOff();
  this->TFuncEditor->SetLabelText ("Distance Color Map");
  this->meshMapper->SetLookupTable (this->DistanceMapTFunc) ;
  this->meshMapper->ScalarVisibilityOn ();
  this->meshMapper->SetColorModeToMapScalars ();

  this->TFuncEditor->SetWholeParameterRange(this->vtkMesh->GetScalarRange());
  
  this->RenderWidget->Render();
}

void vtkKWMeshVisu::UpdateDistanceMapTFunc ()
{
  double range[2];
  if (!this->vtkMesh)
  {
    return;
  }
  this->vtkMesh->GetScalarRange(range);
  this->DistanceMapTFunc->RemoveAllPoints () ;
  this->DistanceMapTFunc->AddHSVPoint(range[0], 0, 1, 1);
  this->DistanceMapTFunc->AddHSVPoint(range[1], 0.6, 1, 1);
}

