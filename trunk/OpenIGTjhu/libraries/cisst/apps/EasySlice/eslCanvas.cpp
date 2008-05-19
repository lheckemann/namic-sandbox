/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: eslCanvas.cpp,v 1.3 2005/09/26 15:41:46 anton Exp $


#include "eslCanvas.h"

#include <dcmdata/dctk.h>
#include <dcmimgle/dcmimage.h>

#include <stdio.h>
#include <map>
#include <string>

#include "eslApplication.h"
#include "eslFrame.h"


BEGIN_EVENT_TABLE(eslCanvas, wxWindow)
    EVT_LEFT_DOWN(eslCanvas::OnMouseLeftDown)
    EVT_RIGHT_DOWN(eslCanvas::OnMouseRightDown)
    EVT_PAINT(eslCanvas::OnPaint)
    EVT_KEY_DOWN(eslCanvas::OnKey)
END_EVENT_TABLE()


CMN_IMPLEMENT_SERVICES(eslCanvas, 5);


eslCanvas::eslCanvas(wxWindow* parent):
    wxWindow(parent, wxID_ANY),
    scaleX(2.0),
    scaleY(2.0)
{
    SetBackgroundColour(wxColour (0, 0, 0));
    ClearBackground();
}


void eslCanvas::OnPaint(wxPaintEvent & event) 
{
    ClearBackground();
    const wxImage& img = wxGetApp().GetImage();
    if (img != NULL) {
        wxImage img2 = img.Scale(int(img.GetWidth() * scaleX), int(img.GetHeight() * scaleY));
        wxBitmap bmp(img2);
        wxClientDC dc(this);
        dc.DrawBitmap(bmp, 0, 0, false);
    }
}


void eslCanvas::OnKey(wxKeyEvent & event) 
{
    eslFrame* frame = (eslFrame*) GetParent();

    switch (event.GetKeyCode()) {

    case 312: // page up
        // previous image
        if (wxGetApp().currentFile == (wxGetApp().GetFilenames().begin())) {
            wxGetApp().currentFile = (wxGetApp().GetFilenames().end());
        }
        wxGetApp().currentFile--;
        frame->LoadImage((wxGetApp().currentFile)->second);
        break;

    case 313: // page down
        // next image
        wxGetApp().currentFile++;
        // if past last, start at first
        if (wxGetApp().currentFile == (wxGetApp().GetFilenames().end())) {
            wxGetApp().currentFile = (wxGetApp().GetFilenames().begin());
        }
        frame->LoadImage((wxGetApp().currentFile)->second);
        break;

    default:
        CMN_LOG_CLASS(1) << "Key code " << event.GetKeyCode() << " not supported" << endl;
    }
}


void eslCanvas::OnMouseLeftDown(wxMouseEvent & event)
{
    // cout << "mouse left down : " << event.m_x << " " << event.m_y << endl;

    const wxImage& img = wxGetApp().GetImage();
    eslImageInformation *imgInfo = &(wxGetApp().GetImageInfo());

    if (img != NULL) {
        cout << "pix mouse" << event.m_x << " " << event.m_y << endl;
        const double XpixImage = event.m_x / scaleX;
        const double YpixImage = event.m_y / scaleY;
        cout << "pix image" << XpixImage << " " << YpixImage << endl;

        const double cost = cos(imgInfo->tilt);
        const double sint = sin(imgInfo->tilt);
        const double Xmm = XpixImage * imgInfo->dx + imgInfo->x;
        const double Ymm = YpixImage * imgInfo->dy + (imgInfo->y * cost + imgInfo->z * sint);
        const double Zmm = (imgInfo->y * -sint + imgInfo->z * cost); 
        
        cout << "Position in space (" << Xmm << ", " << Ymm << ", " << Zmm << ") [mm]" << endl;
    }
}


void eslCanvas::OnMouseRightDown(wxMouseEvent & event)
{
    eslFrame* frame = (eslFrame*) GetParent();

    const wxImage& img = wxGetApp().GetImage();
    if (img != NULL) {
        wxImage img2 = img.Rotate(0.23, wxPoint(img.GetWidth()/2, img.GetHeight()/2), true);
        wxBitmap bmp(img2);
        wxClientDC dc (this);
        dc.DrawBitmap (bmp, event.m_x, event.m_y, true);
    }
    CMN_LOG_CLASS(5) << "mouse right down : " << event.m_x << " " << event.m_y << endl;
}


// $Log: eslCanvas.cpp,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/08/04 15:15:50  anton
// imported into cvstrac
//
// Revision 1.1.1.1  2005/07/29 21:30:45  anton
// Creation
//
