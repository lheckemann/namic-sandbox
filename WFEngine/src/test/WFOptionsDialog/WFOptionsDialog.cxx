#include "WFOptionsDialog.h"

WFOptionsDialog::WFOptionsDialog()
{
}

WFOptionsDialog::~WFOptionsDialog()
{
}

int WFOptionsDialog::initializeDialog()
{
//  vtkKWApplication *app = vtkKWApplication::New();
//   app->SetName("WFOptionsDialog");
//   
//   app->RestoreApplicationSettingsFromRegistry();
//
//   // Set a help link. Can be a remote link (URL), or a local file
//
//   // vtksys::SystemTools::GetFilenamePath(__FILE__) + "/help.html";
//   //app->SetHelpDialogStartingPage("http://www.kwwidgets.org");
//
//   // Add a window
//   // Set 'SupportHelp' to automatically add a menu entry for the help link
//
//   vtkKWWindowBase *win = vtkKWWindowBase::New();
//   //win->SupportHelpOn();
//   app->AddWindow(win);
//   win->Create();
//   win->SetStatusFrameVisibility(0);
//   
//   // Add a notebook to the testClient
//   vtkKWNotebook *tclNB = vtkKWNotebook::New();
//   tclNB->SetParent(win->GetViewFrame());
//   tclNB->Create();
//   std::cout<<win->GetViewFrame()->GetWidth()<<std::endl;
//   tclNB->SetMinimumHeight(win->GetViewFrame()->GetHeight());
//   tclNB->SetMinimumWidth(win->GetViewFrame()->GetWidth());
//   
//   app->Script(
//        "pack %s -side top -anchor nw -expand y -fill both -padx 0 -pady 0", 
//        tclNB->GetWidgetName());
//   
//   tclNB->Delete();
//   int wfPage_id = tclNB->AddPage("Workflow");
//   int consolePage_id = tclNB->AddPage("Console");
//   // Add a label, attach it to the view frame, and pack
//     
//   vtkKWLabel *hello_label = vtkKWLabel::New();
//   hello_label->SetParent(tclNB->GetFrame(wfPage_id));
//   hello_label->Create();
//   hello_label->SetText("Hello, World!");
//   app->Script("pack %s -side left -anchor c -expand y", 
//                 hello_label->GetWidgetName());
//   hello_label->Delete();
//   
//   vtkKWText *consoleText = vtkKWText::New();
//   consoleText->SetParent(tclNB->GetFrame(consolePage_id));
//   consoleText->Create();
//  // consoleText->SetBackgroundColor(0,0,0);
//   consoleText->SetForegroundColor(128,128,128);
//   consoleText->SetText("Network - Console\n-----------------------------------------------");
//   app->Script("pack %s -side top -anchor c -expand y -fill both", 
//     consoleText->GetWidgetName());
//   consoleText->Delete();
//     
//   int ret = 0;
//   win->Display();
//     
//   app->Start(1, NULL);
//   ret = app->GetExitStatus();
//     
//   win->Close();
//
//   // Deallocate and exit
//
//   win->Delete();
//   app->Delete();
//     
//   return ret;
}
