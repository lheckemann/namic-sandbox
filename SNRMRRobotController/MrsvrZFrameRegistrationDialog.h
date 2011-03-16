//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
// Copyright (C) 2005-2009 by Shiga University of Medical Science,
// All Right Reserved.
// Copyright (C) 2009-2011 by Brigham and Women's Hospital
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrNavigationWindow.cpp,v $
// $Revision: 1.19 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:47 $
//====================================================================


#ifndef _INC_MRSVR_NAVIGATION_DIALOG
#define _INC_MRSVR_NAVIGATION_DIALOG

#include <fx.h>


class MrsvrZFrameRegistrationDialog : public FXDialogBox {
  FXDECLARE(MrsvrZFrameRegistrationDialog)
protected:
  FXHorizontalFrame* frplPl;

private:
  MrsvrZFrameRegistrationDialog(){}
  
  FXDataTarget *dtNewTarget[3];
  float valNewTarget[3];

  FXDataTarget *dtOldTarget[3];
  float valOldTarget[3];

  FXDataTarget *dtDeltaTarget[3];
  float valDeltaTarget[3];

  FXDataTarget *dtTemp[3];
  float valTemp[3];

  //Font for Canvas Text
  FXFont *canvasFont0;

  float registrationMatrix[16];


  FXString     valZFramePosition[7];
  FXDataTarget *dtZFramePosition[7];

  
public:
  MrsvrZFrameRegistrationDialog(FXWindow* owner);
  long onCmdRunRegistration(FXObject*, FXSelector,void*);
  void getRegistrationMatrix(float matrix[16]);

private: 
   FXCanvas *canvas;

public: //Messages
  enum{
   ID_CANVAS2=FXDialogBox::ID_LAST,
   ID_MOVETO_TARGET,
   ID_PAINT_TARGET,
   ID_UPDATE_PARAMETER,
   ID_TIMER,
   ID_LAST,
   ID_RUN_REGISTRATION,
   };

  virtual ~MrsvrZFrameRegistrationDialog();
};




#endif //  _INC_MRSVR_NAVIGATION_DIALOG


