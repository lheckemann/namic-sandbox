/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: cmnWxLogger.h,v 1.3 2005/09/26 15:41:45 anton Exp $


#ifndef _cmnWxLogger_h
#define _cmnWxLogger_h


// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/numdlg.h>

#include <cisstCommon/cmnLogger.h>


class cmnWxLogger: public wxFrame {
    wxTextCtrl *logText;
 public:
    
    cmnWxLogger(wxWindow* parent, wxWindowID id, const wxString& title, unsigned int LoD):
        wxFrame(parent, id, title) {
        logText = new wxTextCtrl(this, -1, "cisstLog:\n",
                                 wxDefaultPosition, wxSize(200, 200),
                                 wxTE_READONLY | wxHSCROLL | wxTE_MULTILINE);
        cmnLogger::GetMultiplexer()->AddChannel(logText, LoD);
    }

    ~cmnWxLogger() {
        if (logText) {
            delete logText;
        }
    }
};


#endif // _cmnWxLogger_h


// $Log: cmnWxLogger.h,v $
// Revision 1.3  2005/09/26 15:41:45  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/08/04 15:15:50  anton
// imported into cvstrac
//
// Revision 1.1.1.1  2005/07/29 21:30:45  anton
// Creation
//
