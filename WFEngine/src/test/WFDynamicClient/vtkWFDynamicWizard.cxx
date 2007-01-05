#include "vtkWFDynamicWizard.h"

#include <vtkObjectFactory.h>
#include <vtkKWNotebook.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWFrame.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWPushButtonSet.h>
#include <vtkKWPushButton.h>
#include <vtkKWApplication.h>

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
    
//    nbDW->SetPageEnabled(selWF, 0);
    
//    vtkKWFrameWithLabel *frameDW = vtkKWFrameWithLabel::New();
//    frameDW->SetParent(nbDW->GetFrame(wfSel));
//    frameDW->Create();
//    frameDW->SetBackgroundColor(255,0,0);
//    
//    app->Script("pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2", 
//            frameDW->GetWidgetName());
    
    vtkKWMultiColumnList *mclDW = vtkKWMultiColumnList::New();
    mclDW->SetParent(nbDW->GetFrame(wfSel));
    mclDW->Create();
    
    mclDW->AddColumn("Workflow-Name");
    mclDW->AddColumn("File-Name");
    mclDW->AddColumn("Created");
    
    app->Script("pack %s -side top -anchor nw -expand y -fill both -padx 2 -pady 2", 
            mclDW->GetWidgetName());
    
    vtkKWPushButtonSet *pbtnSet = vtkKWPushButtonSet::New();
    pbtnSet->SetParent(nbDW->GetFrame(wfSel));
    pbtnSet->PackHorizontallyOn();
    
    pbtnSet->Create();
    
    vtkKWPushButton *pbtn = pbtnSet->AddWidget(0);
    pbtn->SetText("Load");
    pbtn->SetEnabled(0);
   
    pbtn = pbtnSet->AddWidget(1);
    pbtn->SetText("Close");
    pbtn->SetEnabled(1);
    
    app->Script("pack %s -side top -anchor se -expand n -fill none -padx 2 -pady 2", 
            pbtnSet->GetWidgetName());
}

