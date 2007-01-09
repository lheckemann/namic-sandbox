#include "vtkWFDynamicWizard.h"

#include <vtkObjectFactory.h>
#include <vtkKWNotebook.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWPushButtonSet.h>
#include <vtkKWPushButton.h>
#include <vtkKWApplication.h>

#include <vtkCallbackCommand.h>

#include <time.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkWFDynamicWizard );
vtkCxxRevisionMacro(vtkWFDynamicWizard, "$Revision: 1.2 $");

vtkWFDynamicWizard::vtkWFDynamicWizard()
{
}

vtkWFDynamicWizard::~vtkWFDynamicWizard()
{
}

void vtkWFDynamicWizard::CreateWidget()
{
    if (this->IsCreated())
    {
        vtkErrorMacro("class already created");
        return;
    }

    // Call the superclass to create the whole widget

    this->Superclass::CreateWidget();
    
    vtkKWApplication *app = this->GetApplication();
    
    vtkKWNotebook *nbDW = vtkKWNotebook::New();
    nbDW->SetParent(this->GetFrame());
    nbDW->SetMinimumWidth(400);
    nbDW->SetMinimumHeight(200);
    nbDW->SetBackgroundColor(255,0,0);
    nbDW->Create();
    
    app->Script("pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2", 
            nbDW->GetWidgetName());
    
    int wfSel = nbDW->AddPage("Workflow-Selection");
    int selWF = nbDW->AddPage("selected Workflow");
    
    nbDW->SetPageEnabled(selWF, 0);
    
//    vtkKWFrameWithLabel *frameDW = vtkKWFrameWithLabel::New();
//    frameDW->SetParent(nbDW->GetFrame(wfSel));
//    frameDW->Create();
//    frameDW->SetBackgroundColor(255,0,0);
//    
//    app->Script("pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2", 
//            frameDW->GetWidgetName());
    
    m_mclDW = vtkKWMultiColumnList::New();
    m_mclDW->SetParent(nbDW->GetFrame(wfSel));
    m_mclDW->Create();
    
    m_mclDW->AddColumn("Workflow-Name");
    m_mclDW->AddColumn("File-Name");
    m_mclDW->AddColumn("Created");
    
//    vtkCallbackCommand *mclDWSelectionChangedCmd = vtkCallbackCommand::New();
//    mclDWSelectionChangedCmd->SetCallback(&vtkWFDynamicWizard::mclDWSelectionChangedCallback);
//    
//    mclDW->AddObserver("SelectionChangedEvent", mclDWSelectionChangedCmd);
    m_mclDW->SetSelectionChangedCommand(this, "mclDWSelectionChangedCallback");
    
    app->Script("pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2", 
            m_mclDW->GetWidgetName());
    
    m_pbtnSet = vtkKWPushButtonSet::New();
    m_pbtnSet->SetParent(nbDW->GetFrame(wfSel));
    m_pbtnSet->PackHorizontallyOn();
    
    m_pbtnSet->Create();

    vtkKWPushButton *pbtn = m_pbtnSet->AddWidget(0);
    pbtn->SetText("Load");
    pbtn->SetEnabled(0);
    
    vtkCallbackCommand *loadBtnPushCmd = vtkCallbackCommand::New();
    loadBtnPushCmd->SetCallback(&vtkWFDynamicWizard::loadBtnPushCmdCallback);
    loadBtnPushCmd->SetClientData(this);
    
    pbtn->AddObserver(vtkKWPushButton::InvokedEvent, loadBtnPushCmd);
        
    pbtn = m_pbtnSet->AddWidget(1);
    pbtn->SetText("Close");
    pbtn->SetEnabled(1);
    
    vtkCallbackCommand *closeBtnPushCmd = vtkCallbackCommand::New();
    closeBtnPushCmd->SetCallback(&vtkWFDynamicWizard::closeBtnPushCmdCallback);
    closeBtnPushCmd->SetClientData(this);
    
    pbtn->AddObserver(vtkKWPushButton::InvokedEvent, closeBtnPushCmd);
    
    app->Script("pack %s -side top -anchor se -expand n -fill none -padx 2 -pady 2", 
            m_pbtnSet->GetWidgetName());
}

int vtkWFDynamicWizard::ConnectToWFEngine()
{
    m_wfDI = WFDirectInterface::New();
    
    if(m_wfDI->InitializeWFEngine())
    {
        std::vector<WFDirectInterface::workflowDesc*> knownWFs;
        knownWFs = m_wfDI->GetAllKnownWorkflows();
        
        std::cout<<"knownWFs.size() "<<knownWFs.size()<<std::endl;
        
        for(std::vector<WFDirectInterface::workflowDesc*>::iterator iter = knownWFs.begin(); iter != knownWFs.end(); iter++)
        {
            WFDirectInterface::workflowDesc *curWFDesc = *iter;
            this->addWorkflowToList(curWFDesc->workflowName.c_str(), curWFDesc->fileName.c_str(), curWFDesc->createTime);
        }
        
        return 1;
    }
    else return 0;
}

void vtkWFDynamicWizard::addWorkflowToList(const char* workflowName, const char* fileName, int date)
{
    int id = this->m_mclDW->GetNumberOfRows();
    this->m_mclDW->InsertCellText(id,0, workflowName);
    this->m_mclDW->InsertCellText(id,1, fileName);
    
    time_t tim = date; 
    this->m_mclDW->InsertCellText(id,2, ctime(&tim));    
//    this->listToWFMap.insert(std::make_pair(id, fileName));
}

void vtkWFDynamicWizard::SaveState()
{
    
}

void vtkWFDynamicWizard::mclDWSelectionChangedCallback()
{
    if(m_mclDW->GetIndexOfFirstSelectedRow() != -1)
    {
        this->m_pbtnSet->GetWidget(0)->SetEnabled(1);
    }
}

void vtkWFDynamicWizard::closeBtnPushCmdCallback(vtkObject* obj, unsigned long, void* param, void*)
{
    vtkWFDynamicWizard *myDW = (vtkWFDynamicWizard*)param;
    myDW->SaveState();
    myDW->closeWorkflow();
    myDW->OK();
}

void vtkWFDynamicWizard::loadBtnPushCmdCallback(vtkObject* obj, unsigned long, void* param, void*)
{
    vtkWFDynamicWizard *myDW = (vtkWFDynamicWizard*)param;
    std::string selWF = myDW->m_mclDW->GetCellText(myDW->m_mclDW->GetIndexOfFirstSelectedRow(), 1);
    std::cout<<selWF<<std::endl;
    myDW->m_wfDI->loadWorkflowFromFile(selWF);
}

void vtkWFDynamicWizard::closeWorkflow()
{
    this->m_wfDI->CloseWorkflowManager();
}
