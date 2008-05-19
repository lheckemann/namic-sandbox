/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: eslCanvas.h,v 1.3 2005/09/26 15:41:46 anton Exp $


#ifndef _eslCanvas_h
#define _eslCanvas_h


// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/numdlg.h>

#include <cisstCommon.h>


class eslCanvas: public wxWindow
{
    CMN_DECLARE_SERVICES(eslCanvas, CMN_NO_DYNAMIC_CREATION);
public:
    eslCanvas(wxWindow* parent);

    double scaleX;
    double scaleY;

    void OnMouseLeftDown(wxMouseEvent & event);
    void OnMouseRightDown(wxMouseEvent & event);
    void OnPaint(wxPaintEvent & event);
    void OnKey(wxKeyEvent & event);

private:

    DECLARE_EVENT_TABLE()
};


#endif // _eslCanvas_h


// $Log: eslCanvas.h,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/08/04 15:15:50  anton
// imported into cvstrac
//
// Revision 1.1.1.1  2005/07/29 21:30:45  anton
// Creation
//
