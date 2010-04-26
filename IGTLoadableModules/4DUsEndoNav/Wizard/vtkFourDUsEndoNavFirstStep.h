/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkFourDUsEndoNavFirstStep_h
#define __vtkFourDUsEndoNavFirstStep_h


#include "vtkFourDUsEndoNavStepBase.h"
#include "vtkCommand.h"

#include <cv.h>
#include <highgui.h>


class vtkKWLoadSaveButtonWithLabel;
class vtkKWFrame;
class vtkKWEntry;
class vtkKWCheckButton;
class vtkKWPushButton;
class vtkKWLabel;

class vtkMRMLModelNode;
class vtkMultiThreader;
class vtkImageImport;
class vtkImageMapper;
class vtkActor2D;
class vtkRenderer;
class vtkRenderWindow;

class VTK_FourDUsEndoNav_EXPORT vtkFourDUsEndoNavFirstStep :
  public vtkFourDUsEndoNavStepBase
{
public:
  static vtkFourDUsEndoNavFirstStep *New();
  vtkTypeRevisionMacro(vtkFourDUsEndoNavFirstStep,vtkFourDUsEndoNavStepBase);
  void                PrintSelf(ostream& os, vtkIndent indent);

  virtual void        ShowUserInterface();
  virtual void        ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData);

  //Set/Get Methods
  CvCapture*          GetCapture(){return this->Capture;};

  IplImage*           GetCaptureImage(){return this->CaptureImage;};

  void                SetCaptureImageTmp(IplImage* CaptureImageTmp)
                        {this->CaptureImageTmp = CaptureImageTmp;};
  IplImage*           GetCaptureImageTmp(){return this->CaptureImageTmp;};

  IplImage*           GetRGBImage(){return this->RGBImage;};

  void                SetImageSize(CvSize ImageSize)
                        {this->ImageSize = ImageSize;};
  CvSize              GetImageSize(){return this->ImageSize;};

  void                SetIdata(unsigned char* Idata)
                        {this->Idata = Idata;};
  unsigned char*      GetIdata(){return this->Idata;};

  vtkRenderWindow*    GetCaptureRenWin(){return this->CaptureRenWin;};

  vtkRenderer*        GetCaptureRen(){return this->CaptureRen;};

  vtkImageImport*     GetImporter(){return this->Importer;};

protected:
  vtkFourDUsEndoNavFirstStep();
  ~vtkFourDUsEndoNavFirstStep();

  // GUI Widgets
  vtkKWFrame*         RobotFrame;
  vtkKWLabel*         RobotLabel1;
  vtkKWLabel*         RobotLabel2;
  vtkKWEntry*         RobotAddressEntry;
  vtkKWEntry*         RobotPortEntry;
  vtkKWPushButton*    RobotConnectButton;
  
  vtkKWFrame*         ScannerFrame;
  vtkKWLabel*         ScannerLabel1;
  vtkKWLabel*         ScannerLabel2;
  vtkKWEntry*         ScannerAddressEntry;
  vtkKWEntry*         ScannerPortEntry;
  vtkKWPushButton*    ScannerConnectButton;

  //Multithreader to run a thread of to acquire and display the data
  vtkMultiThreader*   PlayerThreader;
  int                 PlayerThreadId;
  bool                Capturing;
  vtkMRMLModelNode*   CaptureNode;
  vtkImageMapper*     CaptureMapper;
  vtkActor2D*         CaptureActor;
  CvCapture*          Capture;
  IplImage*           CaptureImage;
  IplImage*           CaptureImageTmp;
  IplImage*           RGBImage;
  CvSize              ImageSize;
  unsigned char*      Idata;
  vtkRenderWindow*    CaptureRenWin;
  vtkRenderer*        CaptureRen;
  vtkImageImport*     Importer;

  // for renderer window capture
  vtkRenderWindow*    RenWinForCapture;
  vtkRenderer*        RenForCapture;


  vtkMRMLModelNode*   SetVisibilityOfCaptureData(const char* nodeName, int v);
  vtkMRMLModelNode*   initCapture(const char* nodeName, double r, double g, double b);
  void                cvGetImageFromCamera(void);
  vtkMRMLModelNode*   SetCloseOfCaptureData(const char* nodeName);
  int                 StopCapture();


private:
  vtkFourDUsEndoNavFirstStep(const vtkFourDUsEndoNavFirstStep&);
  void operator=(const vtkFourDUsEndoNavFirstStep&);
};

#endif