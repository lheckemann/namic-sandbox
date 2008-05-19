/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */


// $Id: eslApplication.h,v 1.3 2005/09/26 15:41:46 anton Exp $


#ifndef _eslApplication_h
#define _eslApplication_h


// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/numdlg.h>

#include <cisstCommon/cmnLogger.h>

#include <map>
#include <string>

#include "eslImageInformation.h"
#include "cmnWxLogger.h"


class eslApplication: public wxApp
{
public:
    typedef std::map<std::string, std::string> uidFilenamesType;
    typedef uidFilenamesType::iterator fileIterator;

    virtual bool OnInit(void);

    virtual void Quit(void);

    const uidFilenamesType& GetFilenames(void) const {
        return uidFilenames;
    }

    uidFilenamesType& GetFilenames(void) {
        return uidFilenames;
    }

    const wxImage& GetImage(void) const {
        return currentImage;
    }

    wxImage& GetImage(void) {
        return currentImage;
    }

    const eslImageInformation& GetImageInfo(void) const {
        return currentImageInfo;
    }

    eslImageInformation& GetImageInfo(void) {
        return currentImageInfo;
    }

    fileIterator currentFile;

private:
    uidFilenamesType uidFilenames;
    wxImage currentImage;
    eslImageInformation currentImageInfo;
    cmnWxLogger *LogWindow;
};


DECLARE_APP(eslApplication);


#endif // _eslApplication_h


// $Log: eslApplication.h,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/08/04 15:15:50  anton
// imported into cvstrac
//
// Revision 1.1.1.1  2005/07/29 21:30:45  anton
// Creation
//
