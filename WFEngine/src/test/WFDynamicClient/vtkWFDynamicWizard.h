#ifndef VTKWFDYNAMICWIZARD_H_
#define VTKWFDYNAMICWIZARD_H_

#include <vtkKWDialog.h>

#include "WFDirectInterface.h"

#include <string>
#include <map>

class vtkKWMultiColumnList;
class vtkKWPushButtonSet;

class vtkWFDynamicWizard : public vtkKWDialog
{
public:
    static vtkWFDynamicWizard *New();
    vtkTypeRevisionMacro(vtkWFDynamicWizard,vtkKWDialog);
    
    virtual int ConnectToWFEngine();
    
    virtual void SaveState();
    virtual void mclDWSelectionChangedCallback();
protected:
    vtkWFDynamicWizard();
    virtual ~vtkWFDynamicWizard();
    
    // Description:
    // Create the widget.
    virtual void CreateWidget();
    
    virtual void addWorkflowToList(const char* workflowName, const char* fileName, int date);
    
    //Callback Functions
    static void loadBtnPushCmdCallback(vtkObject* obj, unsigned long,void*, void*);
    static void closeBtnPushCmdCallback(vtkObject* obj, unsigned long,void*, void*);
    
    virtual void closeWorkflow();
private:
    vtkKWMultiColumnList *m_mclDW;
    WFDirectInterface *m_wfDI;
    vtkKWPushButtonSet *m_pbtnSet;
    
//    std::map<int id, std::string fileName> listToWFMap;
};

#endif /*VTKWFDYNAMICWIZARD_H_*/
