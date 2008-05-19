/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: eslFrame.cpp,v 1.3 2005/09/26 15:41:46 anton Exp $


#include "eslFrame.h"

#include <dcmdata/dctk.h>
#include <dcmimgle/dcmimage.h>

#include <stdio.h>
#include <map>
#include <string>


#include "eslCanvas.h"
#include "eslApplication.h"


enum {
    ID_Quit = 1,
    ID_LoadImage,
};

BEGIN_EVENT_TABLE(eslFrame, wxFrame)
    EVT_MENU (ID_Quit, eslFrame::OnQuit)
    EVT_MENU (ID_LoadImage, eslFrame::OnLoadImage)
END_EVENT_TABLE()


CMN_IMPLEMENT_SERVICES(eslFrame, 5);


eslFrame::eslFrame(const wxString& title, const wxPoint& pos, const wxSize& size):
    wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size),
    LastDirectory("/home/dicom/incoming/CT/")
{
    new eslCanvas(this);

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_LoadImage, _T("L&oad Image\tAlt-L"));
    menuFile->Append(ID_Quit, _T("E&xit\tAlt-X"));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));

    SetMenuBar(menuBar);
}


void eslFrame::OnQuit(wxCommandEvent &)
{
    Close();
    wxGetApp().Quit();
}


void eslFrame::OnLoadImage(wxCommandEvent &)
{
    CMN_LOG_CLASS(3) << "Load image" << endl;

    DcmFileFormat *fileFormat;
    OFCondition status;
    OFString currentField;
    bool foundOneDicom = false;

    eslFrame* frame = (eslFrame*) GetParent();
    wxFileDialog* openFileDialog =
        new wxFileDialog(frame, "Dicom file", LastDirectory.c_str(), "", "*.*", wxMULTIPLE);
    if (openFileDialog->ShowModal() == wxID_OK) {

        // empty the current list
        (wxGetApp().GetFilenames()).erase((wxGetApp().GetFilenames()).begin(),
                                          (wxGetApp().GetFilenames()).end());

        // retrieve the list of files
        wxArrayString filenames;
        LastDirectory = openFileDialog->GetDirectory().c_str();
        openFileDialog->GetFilenames(filenames);
        // check that at least one file has been selected
        if (!filenames.IsEmpty()) {
            unsigned int index;
            for (index = 0; index < filenames.GetCount(); index++) {
                // check that the file is indeed a dicom image
                fileFormat = new DcmFileFormat();
                status = fileFormat->loadFile((LastDirectory + "/" + filenames[index].c_str()).c_str());
                if (status.good()) {
                    if (fileFormat->getDataset()->findAndGetOFStringArray(DCM_SOPInstanceUID, currentField).good()) {
                        CMN_LOG_CLASS(4) << "SOPInstanceUID: " << currentField << endl;
                        // add the file to the list, indexed by SOP uid
                        (wxGetApp().GetFilenames())[currentField.c_str()] = LastDirectory + "/" + filenames[index].c_str();
                        foundOneDicom = true;
                    } else {
                        CMN_LOG_CLASS(1) << "Error: cannot access SOPInstanceUID in "
                                         << LastDirectory + "/" + filenames[index].c_str() << endl;
                    }
                } else {
                    CMN_LOG_CLASS(4) << "File " << filenames[index] << " doesn't seem to be Dicom compatible" << endl;
                }
                delete fileFormat;
            } // end of for loop
        } else {
            CMN_LOG_CLASS(1) << "No file selected!" << endl;
        }
    }
    delete openFileDialog;

    // since we have found at least one image, display it
    if (foundOneDicom) {
        wxGetApp().currentFile = (wxGetApp().GetFilenames()).begin();
        LoadImage((wxGetApp().currentFile)->second);
    }
}


void eslFrame::LoadImage(std::string filename)
{
    CMN_LOG_CLASS(4) << "Loading " << filename << endl;

    eslImageInformation *imgInfo = &(wxGetApp().GetImageInfo());
    DcmFileFormat fileFormat;
    OFCondition status = fileFormat.loadFile(filename.c_str());
    if (status.good()) {

        imgInfo->filename = filename.c_str();
        
        OFString currentField;

        if (fileFormat.getDataset()->findAndGetOFStringArray(DCM_ImagePositionPatient, currentField).good()) {
            CMN_LOG_CLASS(4) << "ImagePositionPatient: " << currentField << endl;
            sscanf(currentField.c_str(),
                   "%lf\\%lf\\%lf",
                   &(imgInfo->x),
                   &(imgInfo->y),
                   &(imgInfo->z));
            CMN_LOG_CLASS(5) << "Found " << imgInfo->x << " " <<  imgInfo->y << " " << imgInfo->z << endl;
        } else {
            CMN_LOG_CLASS(1) << "Error: cannot access ImagePositionPatient!" << endl;
        }
        
        if (fileFormat.getDataset()->findAndGetOFStringArray(DCM_PixelSpacing, currentField).good()) {
            CMN_LOG_CLASS(4) << "PixelSpacing: " << currentField << endl;
            sscanf(currentField.c_str(),
                   "%lf\\%lf",
                   &(imgInfo->dx),
                   &(imgInfo->dy));
            CMN_LOG_CLASS(5) << "Found " << imgInfo->dx << " " << imgInfo->dy << endl;
        } else {
            CMN_LOG_CLASS(1) << "Error: cannot access PixelSpacing!" << endl;
        }

        if (fileFormat.getDataset()->findAndGetOFStringArray(DCM_SliceThickness, currentField).good()) {
            CMN_LOG_CLASS(4) << "SliceThickness: " << currentField << endl;
            sscanf(currentField.c_str(),
                   "%lf",
                   &(imgInfo->thick));
            CMN_LOG_CLASS(5) << "Found "<< imgInfo->thick << endl;
        } else {
            CMN_LOG_CLASS(1) << "Error: cannot access SliceThickness!" << endl;
        }

        if (fileFormat.getDataset()->findAndGetOFStringArray(DCM_PixelSpacing, currentField).good()) {
            CMN_LOG_CLASS(4) << "PixelSpacing: " << currentField << endl;
            sscanf(currentField.c_str(),
                   "%lf\\%lf",
                   &(imgInfo->dx),
                   &(imgInfo->dy));
            CMN_LOG_CLASS(5) << "Found " << imgInfo->dx << " " << imgInfo->dy << endl;
        } else {
            CMN_LOG_CLASS(1) << "Error: cannot access PixelSpacing!" << endl;
        }

        if (fileFormat.getDataset()->findAndGetOFStringArray(DCM_GantryDetectorTilt, currentField).good()) {
            CMN_LOG_CLASS(4) << "GantryDectectorTilt: " << currentField << endl;
            sscanf(currentField.c_str(),
                   "%lf",
                   &(imgInfo->tilt));
            CMN_LOG_CLASS(5) << "Found "<< imgInfo->tilt << endl;
            // convert to radians and display
            imgInfo->tilt = (imgInfo->tilt * M_PI) / 180.0;
        } else {
            CMN_LOG_CLASS(1) << "Error: cannot access GantryDectectorTilt!" << endl;
            imgInfo->tilt = 0.0;
        }

        if (fileFormat.getDataset()->findAndGetOFStringArray(DCM_Rows, currentField).good()) {
            CMN_LOG_CLASS(4) << "Rows: " << currentField << endl;
            sscanf(currentField.c_str(),
                   "%lf",
                   &(imgInfo->nbRows));
            CMN_LOG_CLASS(5) << "Found " << imgInfo->nbRows << endl;
        } else {
            CMN_LOG_CLASS(1) << "Error: cannot access Rows!" << endl;
        }

        if (fileFormat.getDataset()->findAndGetOFStringArray(DCM_Columns, currentField).good()) {
            CMN_LOG_CLASS(4) << "Columns: " << currentField << endl;
            sscanf(currentField.c_str(),
                   "%lf",
                   &(imgInfo->nbCols));
            CMN_LOG_CLASS(5) << "Found " << imgInfo->nbCols << endl;
        } else {
            CMN_LOG_CLASS(1) << "Error: cannot access Rows!" << endl;
        }

        imgInfo->FovWidth = imgInfo->nbCols * imgInfo->dx;
        imgInfo->FovHeight = imgInfo->nbRows * imgInfo->dy;
        CMN_LOG_CLASS(5) << "Fov: " << imgInfo->FovWidth << " x " << imgInfo->FovHeight << endl;

    } else {
        CMN_LOG_CLASS(1) << "Error: cannot read DICOM file (" << status.text() << ")" << endl;
    }

    std::cout << *imgInfo << std::endl;
    
    DicomImage *image = new DicomImage(imgInfo->filename.c_str());

    if (image != NULL) {
        if (image->getStatus() == EIS_Normal) {
            if (image->isMonochrome()) {
                image->setMinMaxWindow();
                image->writeBMP("tmp.bmp");
            } else {
                CMN_LOG_CLASS(1) << "Error: cannot handle color yet!" << endl;
            }
        } else {
            CMN_LOG_CLASS(1) << "Error: cannot load DICOM image (" << DicomImage::getString(image->getStatus()) << ")" << endl;
        }
    }

    delete image;


    wxGetApp().GetImage() = wxImage(_T("tmp.bmp"), wxBITMAP_TYPE_BMP);

    // any unused colour will do
    wxGetApp().GetImage().SetMaskColour(0, 255, 255);

    if (!wxGetApp().GetImage().Ok()) {
        CMN_LOG_CLASS(1) << "Can't load the image" << endl;
    }

    Refresh();
}


// $Log: eslFrame.cpp,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/08/04 15:15:50  anton
// imported into cvstrac
//
// Revision 1.1.1.1  2005/07/29 21:30:45  anton
// Creation
//
