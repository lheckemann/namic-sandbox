/*=========================================================================

  Module:    $RCSfile: vtkKWWindowLevelThresholdEditor.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkKWWindowLevelThresholdEditor.h"

#include "vtkObjectFactory.h"

#include "vtkKWMenuButton.h"
#include "vtkKWFrame.h"
#include "vtkKWMenu.h"
#include "vtkKWCheckButton.h"
#include "vtkKWEntry.h"

#define MIN_RESOLUTION 0.00001

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWWindowLevelThresholdEditor );
vtkCxxRevisionMacro(vtkKWWindowLevelThresholdEditor, "$Revision: 1.49 $");

//----------------------------------------------------------------------------
vtkKWWindowLevelThresholdEditor::vtkKWWindowLevelThresholdEditor()
{
  this->Command = NULL;
  //this->StartCommand = NULL;
  //this->EndCommand   = NULL;

  this->ImageData = NULL;

  this->WindowLevelAutoManual = vtkKWMenuButtonWithLabel::New() ;
  this->ThresholdAutoManual = vtkKWMenuButtonWithLabel::New();
   
  this->Accumulate = vtkImageAccumulateDiscrete::New();
  this->Bimodal = vtkImageBimodalAnalysis::New();
  this->ExtractComponents = vtkImageExtractComponents::New();

  this->WindowLevelRange = vtkKWRange::New();
  this->LevelEntry = vtkKWEntry::New();
  this->WindowEntry = vtkKWEntry::New();
 
  this->ThresholdRange = vtkKWRange::New();
  this->ColorTransferFunctionEditor = vtkKWColorTransferFunctionEditor::New();   
  this->Histogram = vtkKWHistogram::New();
  this->TransferFunction = vtkColorTransferFunction::New();
  this->ColorTransferFunctionEditor->SetColorTransferFunction(this->TransferFunction);
  this->TransferFunction->Delete();
}

//----------------------------------------------------------------------------
vtkKWWindowLevelThresholdEditor::~vtkKWWindowLevelThresholdEditor()
{
  if (this->Command)
    {
    delete [] this->Command;
    }
  if (this->ImageData)
    {
    this->ImageData->Delete();
    this->ImageData = NULL;
    }

  if ( this->LevelEntry ) 
    {
    this->LevelEntry->SetParent(NULL);
    this->LevelEntry->Delete();
    this->LevelEntry = NULL;
    }
  if ( this->WindowEntry ) 
    {
    this->WindowEntry->SetParent(NULL);
    this->WindowEntry->Delete();
    this->WindowEntry = NULL;
    }
  if ( this->WindowLevelAutoManual ) 
    {
    this->WindowLevelAutoManual->SetParent(NULL);
    this->WindowLevelAutoManual->Delete();
    this->WindowLevelAutoManual = NULL ;
    }
  if ( this->ThresholdAutoManual ) 
    {
    this->ThresholdAutoManual->SetParent(NULL);
    this->ThresholdAutoManual->Delete();
    this->ThresholdAutoManual = NULL;
    }
  if ( this->WindowLevelRange ) 
    {
    this->WindowLevelRange->SetParent(NULL);
    this->WindowLevelRange->Delete();
    this->WindowLevelRange = NULL;
    }
  if ( this->ThresholdRange ) 
    {
    this->ThresholdRange->SetParent(NULL);
    this->ThresholdRange->Delete();
    this->ThresholdRange = NULL;
    }
  if ( this->Histogram ) 
    {
    this->Histogram->Delete();
    this->Histogram = NULL;
    }
  if ( this->ColorTransferFunctionEditor) 
    {
    this->ColorTransferFunctionEditor->SetParent(NULL);
    this->ColorTransferFunctionEditor->Delete();
    this->ColorTransferFunctionEditor = NULL;
    }
  if (this->Accumulate)
    {
    this->Accumulate->Delete();
    }
  if (this->Bimodal)
    {
    this->Bimodal->Delete();
    }
  this->ExtractComponents->Delete();
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::SetImageData(vtkImageData* imageData)
{
  if (this->ImageData != imageData || (imageData != NULL && imageData->GetMTime() > this->ImageData->GetMTime()) ) 
    {
    vtkImageData* tempImageData = this->ImageData;
    if (this->ImageData == NULL)
      {
      this->SetWindowLevel(0,0);
      this->SetThreshold(0,0);
      }
    this->ImageData = imageData;
    if (this->ImageData != NULL)
      {
      this->ImageData->Register(this);
      }
    if (tempImageData != NULL)
      {
      tempImageData->UnRegister(this);
      }
      
    if (this->ImageData)
      {
      this->UpdateFromImage();

      this->UpdateTransferFunction();

      this->UpdateAutoLevels();

      this->Modified();
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::UpdateAutoLevels()
{
  vtkDebugMacro("UpdateAutoLevels: returning, call should be made on the volume/display node.");
  return;
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::SetWindowLevel(double window, double level)
{

  double range[2];
  range[0] = level - 0.5*window;
  range[1] = level + 0.5*window;
  double *oldRange = this->WindowLevelRange->GetRange();

  bool changed = false;
  if (oldRange == NULL ||
      range[0] != oldRange[0] ||
      range[1] != oldRange[1])
    {
    this->WindowLevelRange->SetRange(range[0], range[1]);
    changed = true;
    }
  if (window != this->WindowEntry->GetValueAsDouble())
    {
    this->WindowEntry->SetValueAsDouble(window);
    changed = true;
    }
  if (level != this->LevelEntry->GetValueAsDouble())
    {
    this->LevelEntry->SetValueAsDouble(level);
    changed = true;
    }
  if (changed)
    {
    this->UpdateTransferFunction();
    }
}

//----------------------------------------------------------------------------
double vtkKWWindowLevelThresholdEditor::GetWindow()
{
  //double *range = this->WindowLevelRange->GetRange();
  //return range[1] -  range[0];
  return this->WindowEntry->GetValueAsDouble();
}

//----------------------------------------------------------------------------
double vtkKWWindowLevelThresholdEditor::GetLevel()
{
  //double *range = this->WindowLevelRange->GetRange();
  //return 0.5 * (range[1] +  range[0]);
  return this->LevelEntry->GetValueAsDouble();
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::SetThreshold(double lower, double upper)
{
  double *range = this->ThresholdRange->GetRange();
  if (range == NULL ||
      range[0] != lower ||
      range[1] != upper)
    {
    this->ThresholdRange->SetRange(lower, upper);
    this->UpdateTransferFunction();
    }
}

//----------------------------------------------------------------------------
double vtkKWWindowLevelThresholdEditor::GetLowerThreshold()
{
  double *range = this->ThresholdRange->GetRange();
  return range[0];
}

//----------------------------------------------------------------------------
double vtkKWWindowLevelThresholdEditor::GetUpperThreshold()
{
  double *range = this->ThresholdRange->GetRange();
  return range[1];
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::CreateWidget()
{
  // Check if already created
  
  if (this->IsCreated())
    {
    vtkErrorMacro(<< this->GetClassName() << " already created");
    return;
    }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();
  
  this->UpdateTransferFunction();  

  vtkKWFrame *winLevelFrame = vtkKWFrame::New ( );
  winLevelFrame->SetParent (this);
  winLevelFrame->Create();
  this->Script(
    "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", 
    winLevelFrame->GetWidgetName());

  this->WindowLevelAutoManual->SetParent(winLevelFrame);
  this->WindowLevelAutoManual->Create();

  this->WindowLevelAutoManual->SetLabelWidth(12);
  this->WindowLevelAutoManual->SetLabelText("Window/Level:");
  this->WindowLevelAutoManual->GetWidget()->GetMenu()->AddRadioButton ( "Manual");
  this->WindowLevelAutoManual->GetWidget()->GetMenu()->AddRadioButton ( "Auto");
  this->WindowLevelAutoManual->GetWidget()->SetValue ( "Auto" );
  this->WindowLevelAutoManual->GetWidget()->GetMenu()->SetItemCommand(0, this, "ProcessButtonsCommand");
  this->WindowLevelAutoManual->GetWidget()->GetMenu()->SetItemCommand(1, this, "ProcessButtonsCommand");
  this->WindowLevelAutoManual->GetWidget()->SetWidth ( 7 );
  this->Script(
    "pack %s -side left -anchor nw -expand n -fill x -padx 2 -pady 2", 
    this->WindowLevelAutoManual->GetWidgetName());

  this->WindowLevelRange->SetParent(winLevelFrame);
  this->WindowLevelRange->Create();
  this->WindowLevelRange->SymmetricalInteractionOn();
  this->WindowLevelRange->EntriesVisibilityOff ();
  this->WindowLevelRange->SetCommand(this, "ProcessWindowLevelCommand");
  this->WindowLevelRange->SetStartCommand(this, "ProcessWindowLevelStartCommand");
  this->Script(
    "pack %s -side left -anchor nw -expand yes -fill x -padx 2 -pady 2", 
    this->WindowLevelRange->GetWidgetName());

  // Window/Level entries
  this->WindowEntry->SetParent(this->WindowLevelRange);
  this->WindowEntry->Create();
  this->WindowEntry->SetWidth(10);
  this->WindowEntry->SetCommand(this, "ProcessWindowEntryCommand");
  this->Script("grid %s -row 0 -column 3 -sticky w",
               this->WindowEntry->GetWidgetName() );

  this->LevelEntry->SetParent(this->WindowLevelRange);
  this->LevelEntry->Create();
  this->LevelEntry->SetWidth(10);
  this->LevelEntry->SetCommand(this, "ProcessLevelEntryCommand");
  this->Script("grid %s -row 0 -column 5 -sticky e",
               this->LevelEntry->GetWidgetName() );

  vtkKWFrame *threshFrame = vtkKWFrame::New ( );
  threshFrame->SetParent (this);
  threshFrame->Create();
  this->Script(
    "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", 
    threshFrame->GetWidgetName());

  this->ThresholdAutoManual->SetParent(threshFrame);
  this->ThresholdAutoManual->Create();
  this->ThresholdAutoManual->SetLabelWidth(12);
  this->ThresholdAutoManual->GetLabel()->SetJustificationToRight();
  this->ThresholdAutoManual->GetWidget()->SetWidth ( 7 );
  this->ThresholdAutoManual->SetLabelText("Threshold:");
  this->ThresholdAutoManual->GetWidget()->GetMenu()->AddRadioButton ( "Manual");
  this->ThresholdAutoManual->GetWidget()->GetMenu()->AddRadioButton ( "Auto"); 
  this->ThresholdAutoManual->GetWidget()->GetMenu()->AddRadioButton ( "Off");
  this->ThresholdAutoManual->GetWidget()->SetValue ( "Off" );
  this->ThresholdAutoManual->GetWidget()->GetMenu()->SetItemCommand(0, this, "ProcessButtonsCommand");
  this->ThresholdAutoManual->GetWidget()->GetMenu()->SetItemCommand(1, this, "ProcessButtonsCommand");
  this->ThresholdAutoManual->GetWidget()->GetMenu()->SetItemCommand(2, this, "ProcessButtonsCommand");
  this->Script(
    "pack %s -side left -anchor nw -expand n -padx 2 -pady 2", 
    this->ThresholdAutoManual->GetWidgetName());

  this->ThresholdRange->SetParent(threshFrame);
  this->ThresholdRange->Create();
  this->ThresholdRange->SymmetricalInteractionOff();
  this->ThresholdRange->SetCommand(this, "ProcessThresholdCommand");
  this->ThresholdRange->SetStartCommand(this, "ProcessThresholdStartCommand");
  this->Script(
    "pack %s -side left -anchor w -expand y -fill x -padx 2 -pady 2", 
    this->ThresholdRange->GetWidgetName());

    vtkKWFrame *applyFrame = vtkKWFrame::New ( );
  applyFrame->SetParent(this);
    applyFrame->Create();
    this->Script (
                  "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                  applyFrame->GetWidgetName());

  this->ColorTransferFunctionEditor->SetParent(this);
  this->ColorTransferFunctionEditor->Create();
  this->ColorTransferFunctionEditor->ExpandCanvasWidthOn();
  this->ColorTransferFunctionEditor->SetCanvasWidth(300);
  this->ColorTransferFunctionEditor->SetCanvasHeight(126);
  this->ColorTransferFunctionEditor->LabelVisibilityOff ();
  this->ColorTransferFunctionEditor->SetBalloonHelpString(
    "The range and histogram of the currently selected volume.");
  this->ColorTransferFunctionEditor->SetWholeParameterRangeToFunctionRange();
  this->ColorTransferFunctionEditor->SetVisibleParameterRangeToWholeParameterRange();
  
  this->ColorTransferFunctionEditor->SetPointPositionInValueRangeToTop();
  this->ColorTransferFunctionEditor->SetPointStyleToCursorDown();
  this->ColorTransferFunctionEditor->FunctionLineVisibilityOff();
  this->ColorTransferFunctionEditor->PointGuidelineVisibilityOff();
  // This will disable mobing points on the editor
  this->ColorTransferFunctionEditor->PointVisibilityOff(); 
  this->ColorTransferFunctionEditor->PointIndexVisibilityOff();
  this->ColorTransferFunctionEditor->SelectedPointIndexVisibilityOff();
  this->ColorTransferFunctionEditor->MidPointEntryVisibilityOff();
  this->ColorTransferFunctionEditor->SharpnessEntryVisibilityOff();
  
  this->ColorTransferFunctionEditor->ParameterTicksVisibilityOff();
  this->ColorTransferFunctionEditor->ComputeValueTicksFromHistogramOff();
  this->ColorTransferFunctionEditor->SetDisableAddAndRemove(1);
  this->ColorTransferFunctionEditor->SetColorSpaceOptionMenuVisibility(0);

  this->Script(
    "pack %s -side bottom -anchor nw -expand y -fill x -padx 2 -pady 2", 
    this->ColorTransferFunctionEditor->GetWidgetName());
  
  
  this->ColorTransferFunctionEditor->SetHistogramStyleToPolyLine();
  this->ColorTransferFunctionEditor->SetHistogramColor(1.0, 0., 0.);
  this->ColorTransferFunctionEditor->SetHistogramPolyLineWidth (2);

  this->ColorTransferFunctionEditor->SetColorRampPositionToCanvas();
  
  this->ColorTransferFunctionEditor->SetColorRampOutlineStyleToNone();
  
  this->ColorTransferFunctionEditor->SetColorRampHeight(
    this->ColorTransferFunctionEditor->GetCanvasHeight() - 2);
  this->ColorTransferFunctionEditor->SetPointMarginToCanvasToNone();
  
  this->ThresholdRange->SetWholeRange(0, 255);
  this->WindowLevelRange->SetWholeRange(0, 255);
  this->SetWindowLevel(100, 100);
  this->SetThreshold(0, 255);

  this->UpdateTransferFunction();
  this->UpdateAutoLevels();


  // Override the column sorting behavior by always updating

   // clean up...
  winLevelFrame->Delete ( );
  threshFrame->Delete ( );
  applyFrame->Delete();

}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::UpdateFromImage()
{
  if (this->ImageData != NULL)
  {   
    if ( this->ImageData->GetPointData()->GetScalars() != NULL )
      {
      this->Histogram->BuildHistogram( this->ImageData->GetPointData()->GetScalars(), 0);
      double *range = this->Histogram->GetRange();
      double w = this->GetWindow();
      double l = this->GetLevel();
      double r0 = l - 0.5*w;
      double r1 = l + 0.5*w;
      double tl= this->GetLowerThreshold();
      double tu= this->GetUpperThreshold();
      
      this->ThresholdRange->SetWholeRange(range[0], range[1]);
      this->WindowLevelRange->SetWholeRange(range[0] - 0.5 * (range[0] + range[1]), range[1] + 0.5 * (range[0] + range[1]));
/***
      if (this->GetAutoWindowLevel() == 1 || range[0] > r0 || range[1] < r1) 
        {
        this->SetWindowLevel(range[1] - range[0], 0.5 * (range[0] + range[1]) );
        }
      if (this->GetThresholdType() != vtkKWWindowLevelThresholdEditor::ThresholdManual || range[0] > tl || range[1] < tu)
        {
        this->SetThreshold(range[0], range[1]);
        }
***/
      // avoid crash when Image not set for histogram
      this->ColorTransferFunctionEditor->SetHistogram(NULL);
      this->ColorTransferFunctionEditor->SetHistogram(this->Histogram);
    }
  }
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::UpdateTransferFunction()
{
  double range[2] = {0,255};
  if ( this->ImageData)
  {
    this->ImageData->GetScalarRange(range);
    // AdjustRange call will take out points that are outside of the new
    // range, but it needs the points to be there in order to work, so call
    // RemoveAllPoints after it's done
    this->TransferFunction->AdjustRange(range);
  }
  this->TransferFunction->RemoveAllPoints();
  
  double low = this->GetLowerThreshold();
  double upper = this->GetUpperThreshold();
  double min = this->GetLevel() - 0.5 * this->GetWindow();
  double max = this->GetLevel() + 0.5 * this->GetWindow();
  double minVal = 0;
  double maxVal = 1;

  this->TransferFunction->SetColorSpaceToRGB();

  if (low >= max || upper <= min)
    {
    this->TransferFunction->AddRGBPoint(range[0], 0, 0, 0);
    this->TransferFunction->AddRGBPoint(range[1], 0, 0, 0); 
    }
  else
    {
    if (max <= min)
      {
      max = min +0.001;
      }

    if (low <= range[0])
      {
      low = range[0]+0.001;
      }

    if (min <= range[0])
      {
      min = range[0]+0.001;
      }

    if (upper > range[1])
      {
      upper = range[1] - 0.001;
      }

    if (min <= low)
      {
      minVal = (low - min)/(max - min);
      min = low + 0.001;
      }
    
    if (max >= upper)
      {
      maxVal = (upper - min)/(max-min);
      max = upper - 0.001;
      }
    this->TransferFunction->AddRGBPoint(range[0], 0, 0, 0);
    this->TransferFunction->AddRGBPoint(low, 0, 0, 0);
    this->TransferFunction->AddRGBPoint(min, minVal, minVal, minVal);
    this->TransferFunction->AddRGBPoint(max, maxVal, maxVal, maxVal);
    this->TransferFunction->AddRGBPoint(upper, maxVal, maxVal, maxVal);
    if (upper+0.001 < range[1])
      {
      this->TransferFunction->AddRGBPoint(upper+0.001, 0, 0, 0);
      this->TransferFunction->AddRGBPoint(range[1], 0, 0, 0);
      }
    }

  this->TransferFunction->SetAlpha(1.0);
  this->TransferFunction->Build();
  this->ColorTransferFunctionEditor->SetWholeParameterRangeToFunctionRange();
  this->ColorTransferFunctionEditor->Update();
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::SetCommand(vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(&this->Command, object, method);
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "ColorTransferFunctionEditor: " << this->ColorTransferFunctionEditor << endl;
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::ProcessWindowLevelCommand(double min, double max)
{
  double range[2];
  range[0] = min;
  range[1] = max;
  double window = max-min;
  double level = 0.5*(min+max);
  if (fabs(window - this->GetWindow() ) < MIN_RESOLUTION &&
      fabs(level - this->GetLevel() ) < MIN_RESOLUTION)
    {
    return;
    }
  bool changed = false;
  if (this->WindowEntry->GetValueAsDouble() != window)
    {
    this->WindowEntry->SetValueAsDouble(window);
    changed = true;
    }
  if (this->LevelEntry->GetValueAsDouble() != level)
    {
    this->LevelEntry->SetValueAsDouble(level);
    changed = true;
    }
  if (changed)
    {
    this->UpdateTransferFunction();
    this->InvokeEvent(vtkKWWindowLevelThresholdEditor::ValueChangedEvent, range);
    }
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::ProcessWindowLevelStartCommand(double min, double max)
{
  double range[2];
  range[0] = min;
  range[1] = max;
  if (this->WindowEntry->GetValueAsDouble() != max-min)
    {
    this->WindowEntry->SetValueAsDouble(max-min);
    }
  if ( this->LevelEntry->GetValueAsDouble() != 0.5*(min+max))
    {
    this->LevelEntry->SetValueAsDouble(0.5*(min+max));
    }
  this->UpdateTransferFunction();
  this->SetAutoWindowLevel(0);
  this->InvokeEvent(vtkKWWindowLevelThresholdEditor::ValueStartChangingEvent, range);
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::ProcessThresholdCommand(double min, double max)
{
  double *range = this->ThresholdRange->GetRange();
  if (range[0] != min || range[1] != max)
    {
    range[0] = min;
    range[1] = max;
    this->UpdateTransferFunction();
    this->InvokeEvent(vtkKWWindowLevelThresholdEditor::ValueChangedEvent, range);
    }
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::ProcessThresholdStartCommand(double min, double max)
{
  double range[2];
  range[0] = min;
  range[1] = max;
  this->UpdateTransferFunction();
  if (this->GetThresholdType() == vtkKWWindowLevelThresholdEditor::ThresholdAuto)
    {
    this->SetThresholdType(vtkKWWindowLevelThresholdEditor::ThresholdManual);
    }
  this->InvokeEvent(vtkKWWindowLevelThresholdEditor::ValueStartChangingEvent, range);
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::ProcessWindowEntryCommand(double window)
{
  vtkDebugMacro("ProcessWindowEntryCommand: win = " << window);
  
  double *wrange = this->WindowLevelRange->GetRange();

  if (fabs(window - (wrange[1]-wrange[0]) ) < MIN_RESOLUTION)
    {
    return;
    }
  double range[2];
  double level = this->GetLevel();
  range[0] = window - 0.5*level;
  range[1] = window + 0.5*level;

  if (wrange[0] != level - 0.5*window||
      wrange[1] != level + 0.5*window)
    {
    this->WindowLevelRange->SetRange(level - 0.5*window, level + 0.5*window);   
    this->UpdateTransferFunction();
    this->InvokeEvent(vtkKWWindowLevelThresholdEditor::ValueChangedEvent, range);
    }
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::ProcessLevelEntryCommand(double level)
{
  vtkDebugMacro("ProcessLevelEntryCommand: level = " << level);
  
  double *wrange = this->WindowLevelRange->GetRange();
  if (fabs(level - 0.5*(wrange[1]+wrange[0]) ) < MIN_RESOLUTION)
    {
    return;
    }
  double range[2];
  double window = this->GetWindow();
  range[0] = window - 0.5*level;
  range[1] = window + 0.5*level;

  if (wrange[0] != level - 0.5*window ||
      wrange[1] != level + 0.5*window)
    {
    this->WindowLevelRange->SetRange(level - 0.5*window, level + 0.5*window);
    this->UpdateTransferFunction();
    this->InvokeEvent(vtkKWWindowLevelThresholdEditor::ValueChangedEvent, range);
    }
}

//--------------------------------------------------
int vtkKWWindowLevelThresholdEditor::GetAutoWindowLevel()
{
  if (!strcmp(this->WindowLevelAutoManual->GetWidget()->GetValue(), "Auto"))
    {
    return 1;
    }
  else
    {
    return 0;
    }
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::SetAutoWindowLevel(int value)
{
  if (value == 1 && strcmp(this->WindowLevelAutoManual->GetWidget()->GetValue(), "Auto") != 0)
    {
    this->WindowLevelAutoManual->GetWidget()->SetValue("Auto");
    this->UpdateAutoLevels();
    }
  else if (value == 0 && strcmp(this->WindowLevelAutoManual->GetWidget()->GetValue(), "Manual") != 0)
    {
    this->WindowLevelAutoManual->GetWidget()->SetValue("Manual");
    }
}

//----------------------------------------------------------------------------
int vtkKWWindowLevelThresholdEditor::GetThresholdType()
{
  if (!strcmp(this->ThresholdAutoManual->GetWidget()->GetValue(), "Off"))
    {
    return this->ThresholdOff;
    }
  else if (!strcmp(this->ThresholdAutoManual->GetWidget()->GetValue(), "Auto"))
    {
    return this->ThresholdAuto;
    }
  else if (!strcmp(this->ThresholdAutoManual->GetWidget()->GetValue(), "Manual"))
    {
    return this->ThresholdManual;
    }
  else 
    {
    return -1;
    }
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::SetThresholdType(int value)
{
  if (value == ThresholdAuto && strcmp(this->ThresholdAutoManual->GetWidget()->GetValue(), "Auto") != 0)
    {
    this->ThresholdAutoManual->GetWidget()->SetValue("Auto");
    this->UpdateAutoLevels();
    }
  if (value == ThresholdOff && strcmp(this->ThresholdAutoManual->GetWidget()->GetValue(), "Off") != 0) 
    {
    this->ThresholdAutoManual->GetWidget()->SetValue("Off");
    }
  if (value == ThresholdManual && strcmp(this->ThresholdAutoManual->GetWidget()->GetValue(), "Manual") != 0)
    {
    this->ThresholdAutoManual->GetWidget()->SetValue("Manual");
    }

}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::ProcessButtonsCommand()
{
  vtkDebugMacro("ProcessButtonsCommand: calling update auto levels");
  this->UpdateAutoLevels();
  this->InvokeEvent(vtkKWWindowLevelThresholdEditor::ValueChangedEvent, NULL);
}

//----------------------------------------------------------------------------
void vtkKWWindowLevelThresholdEditor::ProcessCheckButtonCommand(int state)
{
  vtkDebugMacro("ProcessCheckButtonCommand: invoking value changed event");
  this->InvokeEvent(vtkKWWindowLevelThresholdEditor::ValueChangedEvent, NULL);
}
