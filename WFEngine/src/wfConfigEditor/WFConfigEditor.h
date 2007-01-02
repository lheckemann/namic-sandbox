#ifndef WFCONFIGEDITOR_H_
#define WFCONFIGEDITOR_H_

#include <vtkKWDialog.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkKWPushButton.h>
#include <vtkKWPushButtonSet.h>

using namespace std;

class WFConfigEditor : public vtkKWDialog
{
public:
 static WFConfigEditor *New();
 WFConfigEditor();
 vtkTypeRevisionMacro(WFConfigEditor, vtkKWDialog);
 virtual ~WFConfigEditor();
protected:
 
 
 void InitializeGUI();
 
 struct wfEntry
   {
     const char *Name;
     const char *Version;
     const char *Maintainer;
     int TeamSize;
     int Lock;
     const char *Color;
     double Completion;
   };
   
//   typedef vtkKWDialog super;
   
private:
 //GUI-Elements
 vtkKWMultiColumnListWithScrollbars *wfCfgMCL;
 vtkKWPushButtonSet *wfCfgBtnSetBottom;
 vtkKWPushButton *wfCfgBtnLoad;
 vtkKWPushButton *wfCfgBtnClose;
 vtkKWPushButton *wfCfgBtnSave;
 //vtkKWButton wfCfgBtnLoad;
 //vtkKWButton wfCfgBtnLoad;
 typedef vtkKWDialog super;
 
// static WFConfigEditor *New();
 
 
};

#endif /*WFCONFIGEDITOR_H_*/
