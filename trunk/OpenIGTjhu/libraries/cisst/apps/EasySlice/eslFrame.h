/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: eslFrame.h,v 1.3 2005/09/26 15:41:46 anton Exp $


#ifndef _eslFrame_h
#define _eslFrame_h


// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/numdlg.h>

#include <cisstCommon.h>

#include <stdio.h>
#include <map>
#include <string>


class eslFrame: public wxFrame
{
    CMN_DECLARE_SERVICES(eslFrame, CMN_NO_DYNAMIC_CREATION);
public:
    eslFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit(wxCommandEvent &);
    void OnLoadImage(wxCommandEvent &);
    void LoadImage(std::string filename);

    std::string LastDirectory;

    DECLARE_EVENT_TABLE()
};


#endif // _eslFrame_h


// $Log: eslFrame.h,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/08/04 15:15:50  anton
// imported into cvstrac
//
// Revision 1.1.1.1  2005/07/29 21:30:45  anton
// Creation
//
