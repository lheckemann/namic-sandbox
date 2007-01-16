#ifndef VTKWFDYNAMICWIZARD_H_
#define VTKWFDYNAMICWIZARD_H_

#include <vtkKWDialog.h>

#include "WFDirectInterface.h"

#include <string>
#include <map>

class vtkKWMultiColumnList;
class vtkKWPushButtonSet;
class vtkKWNotebook;
class vtkKWWizardWidget;

class vtkWFDynamicWizard : public vtkKWDialog
{
public:
    static vtkWFDynamicWizard *New();
    vtkTypeRevisionMacro(vtkWFDynamicWizard,vtkKWDialog);
    
    virtual int ConnectToWFEngine();
    
    virtual void SaveState();
        
    virtual void workStepCallBack();
protected:
    vtkWFDynamicWizard();
    virtual ~vtkWFDynamicWizard();
    
    // Description:
    // Create the widget.
    virtual void CreateWidget();
    
    virtual void addWorkflowToList(const char* workflowName, const char* fileName, int date);
    
    //Callback Functions
    static void mclDWSelectionChangedCallback(vtkObject* obj, unsigned long,void*, void*);
    static void loadBtnPushCmdCallback(vtkObject* obj, unsigned long,void*, void*);
    static void nextTransitionCallback(vtkObject* obj, unsigned long,void*, void*);
    static void backTransitionCallback(vtkObject* obj, unsigned long,void*, void*);
    static void closeBtnPushCmdCallback(vtkObject* obj, unsigned long,void*, void*);
    
    virtual void closeWorkflow();
    
    virtual void createWizard();
    
    static void navigationStackedChangedCallback(vtkObject* obj, unsigned long, void*, void*); 
private:
    vtkKWMultiColumnList *m_mclDW;
    WFDirectInterface *m_wfDI;
    vtkKWPushButtonSet *m_pbtnSet;
    vtkKWNotebook *m_nbDW;
    vtkKWWizardWidget *m_curWizWidg;
    
    char* m_loadedWF;
    int m_wfSel;
    int m_selWF;
    int m_curStepID;
    //BTX
    std::map<int, std::string> m_kwwID2StepIDMap;
    //ETX
//    std::map<int id, std::string fileName> listToWFMap;
};

#endif /*VTKWFDYNAMICWIZARD_H_*/
