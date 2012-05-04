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

#include "ZLinAlg.h"
#include "newmatap.h"
#include "newmat.h"

#include "MrsvrZFrameRegistrationDialog.h"

#include "ZFrameCalibration.h"

using namespace std;

//Maier Message Map
FXDEFMAP(MrsvrZFrameRegistrationDialog) MrsvrZFrameRegistrationDialogMap[]={
//_____________Message_Type_________________ID_________________Messgae Handler
FXMAPFUNC(SEL_COMMAND,  MrsvrZFrameRegistrationDialog::ID_RUN_REGISTRATION, MrsvrZFrameRegistrationDialog::onCmdRunRegistration),
};
 

FXIMPLEMENT(MrsvrZFrameRegistrationDialog,FXDialogBox,MrsvrZFrameRegistrationDialogMap,ARRAYNUMBER(MrsvrZFrameRegistrationDialogMap))




// Construct a dialog box
MrsvrZFrameRegistrationDialog::MrsvrZFrameRegistrationDialog(FXWindow* owner):
  FXDialogBox(owner,"Z-Frame Registration ",DECOR_TITLE|DECOR_CLOSE|DECOR_BORDER)
{

  for (int i = 0; i < 7; i ++) {
    valZFramePosition[i] = "0.0, 0.0, 0.0";
    dtZFramePosition[i]  = new FXDataTarget(valZFramePosition[i], this, (FXSelector)NULL);  
  }


  // Initialize registration matrix with identity matrix
  for (int i = 0; i < 16; i ++) {
    registrationMatrix[i] = (i%5 == 0) ? 1.0 : 0.0;
    dtRegistrationMatrix[i] = 
      new FXDataTarget(registrationMatrix[i], this, ID_UPDATE_PARAMETER);
  }

  // Main Frame
  frplPl=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|FRAME_NONE|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0,40,40,20,20);
  
  //frplPl->matrix
  FXMatrix* matrix=new FXMatrix(frplPl,1,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  //matrix_main->gbRegDist
  FXGroupBox* gbRegDist = 
    new FXGroupBox(matrix, "Z-Frame Points for Registration",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  FXMatrix* mtZFrameRegistration=new FXMatrix(gbRegDist,1,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
  FXMatrix* mtZFramePosition=new FXMatrix(mtZFrameRegistration,5,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
  
  // First row
  new FXLabel(mtZFramePosition,"#1",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(mtZFramePosition,10,dtZFramePosition[0],FXDataTarget::ID_VALUE);
  new FXLabel(mtZFramePosition,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(mtZFramePosition,10,dtZFramePosition[6],FXDataTarget::ID_VALUE);
  new FXLabel(mtZFramePosition,"#7",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  // Second row
  new FXLabel(mtZFramePosition,"#2",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(mtZFramePosition,10,dtZFramePosition[1],FXDataTarget::ID_VALUE);
  new FXLabel(mtZFramePosition,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(mtZFramePosition,10,dtZFramePosition[5],FXDataTarget::ID_VALUE);
  new FXLabel(mtZFramePosition,"#6",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  // Third Row
  new FXLabel(mtZFramePosition,"#3",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(mtZFramePosition,10,dtZFramePosition[2],FXDataTarget::ID_VALUE);
  new FXTextField(mtZFramePosition,10,dtZFramePosition[3],FXDataTarget::ID_VALUE);
  new FXTextField(mtZFramePosition,10,dtZFramePosition[4],FXDataTarget::ID_VALUE);
  new FXLabel(mtZFramePosition,"#5",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);

  // Forth Row
  new FXLabel(mtZFramePosition,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(mtZFramePosition,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(mtZFramePosition,"#4",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(mtZFramePosition,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(mtZFramePosition,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  //Z_Frame sample image
  char buf[73728];
  //FILE* fp = fopen("icon/Z_Frame.gif", "rb");
  //fread(buf, 1, 73728, fp);
  //fclose(fp);
  
  new FXButton(mtZFrameRegistration, "",
               new FXGIFIcon(this->getApp(), (void*)buf),0,0,BUTTON_TOOLBAR|
               LAYOUT_CENTER_X|LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  new FXButton(mtZFrameRegistration, "Calculate Transform Matrix", NULL, this,
               ID_RUN_REGISTRATION,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  //matrix_main->gbRegDist
  FXGroupBox* gpMatrix = 
    new FXGroupBox(matrix, "Transform Matrix",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);

  FXMatrix* mtMatrix  = 
    new FXMatrix(gpMatrix,4,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_TOP|LAYOUT_CENTER_X);

  for (int i = 0; i < 16; i ++) {
    new FXTextField(mtMatrix,4,dtRegistrationMatrix[i],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_RIGHT|FRAME_SUNKEN,
                  0, 0, 20, 15);
  }


  FXMatrix* mxButtons
    = new FXMatrix(matrix,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_SIDE_RIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y);

  new FXButton(mxButtons, "Set", NULL, this,
               ID_ACCEPT,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  new FXButton(mxButtons, "Cancel", NULL, this,
               ID_CANCEL,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  
  //Timer for OnCmdTimer, paint all new
  getApp()->addTimeout(this,ID_TIMER,80);
  
}


// Destructor
MrsvrZFrameRegistrationDialog::~MrsvrZFrameRegistrationDialog()
{
  getApp()->removeTimeout(this,ID_TIMER);
}


long MrsvrZFrameRegistrationDialog::onCmdRunRegistration(FXObject*,FXSelector,void*)
{
  float Zcoordinate[7][2];
  Column3Vector Zposition;
  Quaternion Zorientation;
  Quaternion ZorientationBase;

  float ZquaternionBase[4];
  float imagePositionSI[7];
  
  zf::Matrix4x4 ZmatrixBase;
  zf::IdentityMatrix(ZmatrixBase);
  ZmatrixBase[0][0] = -1.0;
  ZmatrixBase[1][1] = -1.0;
  ZmatrixBase[2][2] = 1.0;
  zf::MatrixToQuaternion(ZmatrixBase, ZquaternionBase);

  for (int i = 0; i < 7; i ++) {
    valZFramePosition[i].scan("%f, %f, %f", &Zcoordinate[6-i][0], &Zcoordinate[6-i][1], &imagePositionSI[6-i]);
  }

  float imageSI = (imagePositionSI[0] + imagePositionSI[2] + imagePositionSI[4] + imagePositionSI[6]) / 4;

  zf::ZFrameCalibration * calibration;
  calibration = new zf::ZFrameCalibration();
  calibration->SetOrientationBase(ZquaternionBase);
  calibration->LocalizeFrame(Zcoordinate, Zposition, Zorientation);

  ZorientationBase.setX( ZquaternionBase[0] );
  ZorientationBase.setY( ZquaternionBase[1] );
  ZorientationBase.setZ( ZquaternionBase[2] );
  ZorientationBase.setW( ZquaternionBase[3] );

  Zorientation = Zorientation / ZorientationBase;

  float quaternion[4];
  quaternion[0] = Zorientation.getX();
  quaternion[1] = Zorientation.getY();
  quaternion[2] = Zorientation.getZ();
  quaternion[3] = Zorientation.getW();

  zf::Matrix4x4 Zmatrix;  
  zf::QuaternionToMatrix(quaternion, Zmatrix);

  registrationMatrix[0] = Zmatrix[0][0];
  registrationMatrix[1] = Zmatrix[0][1];
  registrationMatrix[2] = Zmatrix[0][2];
  registrationMatrix[3] = Zposition.getX();
  registrationMatrix[4] = Zmatrix[1][0];
  registrationMatrix[5] = Zmatrix[1][1];
  registrationMatrix[6] = Zmatrix[1][2];
  registrationMatrix[7] = Zposition.getY();
  registrationMatrix[8] = Zmatrix[2][0];
  registrationMatrix[9] = Zmatrix[2][1];
  registrationMatrix[10] = Zmatrix[2][2];
  registrationMatrix[11] = Zposition.getZ() + imageSI;
  registrationMatrix[12] = 0;
  registrationMatrix[13] = 0;
  registrationMatrix[14] = 0;
  registrationMatrix[15] = 1;

  //int r = calibration->Register(range, Zposition, Zorientation);
  //calibration->Localize()

  return 1;
}


void MrsvrZFrameRegistrationDialog::getRegistrationMatrix(float matrix[16])
{
  for (int i = 0; i < 16; i ++) {
    matrix[i] = registrationMatrix[i];
  }
  
}


/*******************************************************************************/// End Maier




