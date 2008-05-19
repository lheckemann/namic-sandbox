/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: eslApplication.cpp,v 1.3 2005/09/26 15:41:45 anton Exp $


#include "eslApplication.h"

#include <iostream>

#include "eslFrame.h"


IMPLEMENT_APP(eslApplication)


bool eslApplication::OnInit() {
    currentImage = NULL;

    // set log level high and add cout
    cmnLogger::SetLoD(10);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(10);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, 10);
    // set the lod for the classes we use
    cmnClassRegister::SetLoD("eslFrame", 5);
    cmnClassRegister::SetLoD("eslCanvas", 5);
    eslFrame *frame = new eslFrame(_T("EasySlice"),
                                 wxPoint(20, 20), wxSize(1024, 1024));

    // add a graphic log window
    // LogWindow = new cmnWxLogger(NULL, -1, "EasySlice: Log", 10);

    // show both frames
    // LogWindow->Show();
    frame->Show();
    SetTopWindow(frame);
    return true;
}


void eslApplication::Quit(void) {
    // LogWindow->Close();
}


// $Log: eslApplication.cpp,v $
// Revision 1.3  2005/09/26 15:41:45  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/08/04 15:15:50  anton
// imported into cvstrac
//
// Revision 1.1.1.1  2005/07/29 21:30:45  anton
// Creation
//
