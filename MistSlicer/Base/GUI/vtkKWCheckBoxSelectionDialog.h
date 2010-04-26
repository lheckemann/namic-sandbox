/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerNodeSelectorWidget.h,v $
  Date:      $Date: 2006/01/08 04:48:05 $
  Version:   $Revision: 1.45 $

=========================================================================auto=*/

// .NAME vtkSlicerNodeSelectorWidget - menu to select volumes from current mrml scene
// .SECTION Description
// Inherits most behavior from kw widget, but is specialized to observe
// the current mrml scene and update the entries of the pop up menu to correspond
// to the currently available volumes.  This widget also has a notion of the current selection
// that can be observed or set externally
//


#ifndef __vtkKWCheckBoxSelectionDialog_h
#define __vtkKWCheckBoxSelectionDialog_h

#include "vtkStringArray.h"

#include "vtkSlicerWidget.h"

class vtkKWDialog;
class vtkKWEntryWithLabel;
class vtkKWCheckButton;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWPushButton;

class VTK_SLICER_BASE_GUI_EXPORT vtkKWCheckBoxSelectionDialog : public vtkSlicerWidget
{
  
public:
  static vtkKWCheckBoxSelectionDialog* New();
  vtkTypeRevisionMacro(vtkKWCheckBoxSelectionDialog,vtkSlicerWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  // Description:
  // removes observers on widgets in the class
  virtual void RemoveWidgetObservers ( );

  // Description:
  // Add entry name
  void AddEntry(char *Label, int selected);

  // Description:
  // Clear all entries
  void RemoveAllEntries();

  // Description:
  // Returns selected entries
  vtkStringArray* GetSelectedEntries();
 
  // Description:
  // Returns unselected entries
  vtkStringArray* GetUnselectedEntries();

  // Description:
  // invoke the widget
  void Invoke();

  // Description:
  // Title
  vtkGetStringMacro(Title);
  vtkSetStringMacro(Title);

  // Description:
  // First colomn name
  vtkGetStringMacro(EntryColomnName);
  vtkSetStringMacro(EntryColomnName);

  // Description:
  // First colomn name
  vtkGetStringMacro(BoxColomnName);
  vtkSetStringMacro(BoxColomnName);

  // Description:
  // Cancel action
  vtkSetMacro(Cancel, int);
  vtkGetMacro(Cancel, int);


  // Update widget
  //void UpdateWidget();
  
protected:
  vtkKWCheckBoxSelectionDialog();
  virtual ~vtkKWCheckBoxSelectionDialog();

  // Description:
  // Create the widget.
  virtual void CreateWidget();


private:
    
  //BTX
  //std::vector<std::string> Labels;
  //std::vector<std::string> Descriptions;
  //std::vector<std::int> Selected;
  //ETX
  
  vtkStringArray *SelectedLabels;

  char *Title;
  char *EntryColomnName;
  char *BoxColomnName;
  int Cancel;

  vtkKWDialog *Dialog;

  vtkKWPushButton *OkButton;

  vtkKWPushButton *CancelButton;
  
  vtkKWMultiColumnListWithScrollbars *MultiColumnList;

  vtkKWCheckBoxSelectionDialog(const vtkKWCheckBoxSelectionDialog&); // Not implemented
  void operator=(const vtkKWCheckBoxSelectionDialog&); // Not Implemented
};

#endif
