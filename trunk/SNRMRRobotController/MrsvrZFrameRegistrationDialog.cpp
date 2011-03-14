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

#include "MrsvrZFrameRegistrationDialog.h"

using namespace std;

//Maier Message Map
FXDEFMAP(MrsvrZFrameRegistrationDialog) MrsvrZFrameRegistrationDialogMap[]={
//_____________Message_Type_________________ID_________________Messgae Handler
FXMAPFUNC(SEL_PAINT,    MrsvrZFrameRegistrationDialog::ID_CANVAS2,          MrsvrZFrameRegistrationDialog::onPaint),
FXMAPFUNC(SEL_TIMEOUT,  MrsvrZFrameRegistrationDialog::ID_TIMER,            MrsvrZFrameRegistrationDialog::onCmdTimer),
FXMAPFUNC(SEL_COMMAND,  MrsvrZFrameRegistrationDialog::ID_RUN_REGISTRATION, MrsvrZFrameRegistrationDialog::onCmdRunRegistration),
};
 

FXIMPLEMENT(MrsvrZFrameRegistrationDialog,FXDialogBox,MrsvrZFrameRegistrationDialogMap,ARRAYNUMBER(MrsvrZFrameRegistrationDialogMap))


// Construct a dialog box
MrsvrZFrameRegistrationDialog::MrsvrZFrameRegistrationDialog(FXWindow* owner):
  FXDialogBox(owner,"Z-Frame Registration ",DECOR_TITLE|DECOR_MENU)
{

  // Initialize registration matrix with identity matrix
  for (int i = 0; i < 16; i ++) {
    registrationMatrix[i] = 0.0;
  }
  registrationMatrix[4*0+0] = 1.0;
  registrationMatrix[4*1+1] = 1.0;
  registrationMatrix[4*2+2] = 1.0;
  registrationMatrix[4*3+3] = 1.0;
  
  //Fill with test Data, must be connected to Slicer and Manual Reg
  valOldTarget[0]=65.9;
  valOldTarget[1]=178.2;
  valOldTarget[2]=228.7;
  for (int i = 0; i < 3; i ++) {
    dtOldTarget[i] = 
      new FXDataTarget(valOldTarget[i], this, ID_UPDATE_PARAMETER);
    
    valDeltaTarget[i]=0;
    dtDeltaTarget[i] = 
      new FXDataTarget(valDeltaTarget[i], this, ID_UPDATE_PARAMETER);
    
    valNewTarget[i]=valOldTarget[i]-valDeltaTarget[i];
    dtNewTarget[i] = 
      new FXDataTarget(valNewTarget[i], this, ID_UPDATE_PARAMETER);
    
    valTemp[i]=0;
    dtTemp[i] = 
      new FXDataTarget(valTemp[i], this, ID_UPDATE_PARAMETER);
  }
  

  // +--------------------------------matrix-------------------------------------------------------+
  // |                                                                                             |                   
  // | +---------matrix_main----------------------------+--------------------------------------+   |
  // | |                                                |                                      |   |                     
  // | |   +------------matrix_upperleft------------+   | +--------------------gbRegDist----+  |   |
  // | |   |                     |                  |   | |                                 |  |   |
  // | |   |  +--gpRAPos--+      | +--gbPlateA--+   |   | | +---matrix_upperright_main---+  |  |   |
  // | |   |  |           |      | |            |   |   | | |                            |  |  |   |
  // | |   |  |           |      | |            |   |   | | |  +--matrix_upperright---+  |  |  |   |
  // | |   |  |   CANVAS  |      | | Slider A   |   |   | | |  |                      |  |  |  |   |                  
  // | |   |  |           |      | |            |   |   | | |  | #3      #4      #5   |  |  |  |   |                       
  // | |   |  |           |      | |            |   |   | | |  |                      |  |  |  |   |                   
  // | |   |  +-----------+      | +------------+   |   | | |  | #2              #6   |  |  |  |   |
  // | |   +---------------------+------------------+   | | |  |                      |  |  |  |   |
  // | |   |  gbPlateR--------+  |                  |   | | |  | #1              #7   |  |  |  |   |                      
  // | |   |  |               |  |                  |   | | |  |                      |  |  |  |   |                    
  // | |   |  |  Slider R     |  |     Home         |   | | |  +----------------------+  |  |  |   |   
  // | |   |  |               |  |                  |   | | +----------------------------+  |  |   |         
  // | |   |  +---------------+  |                  |   | | |        Z-Frame_image       |  |  |   |
  // | |   +---------------------+------------------+   | | +----------------------------+  |  |   |                  
  // | |                                                | +---------------------------------+  |   |
  // | +------------------------------------------------+--------------------------------------+   |
  // |                                                                                             |
  // +---------------------------------------------------------------------------------------------+  
  // |                                                                                             |
  // | +------------------------------------------matrix_main2---------------------------------+   |
  // | |   +----------------mtrix_lowerleft----------------------------+                       |   |
  // | |   |                                                           |                       |   |                    
  // | |   |                                                           |                       |   |                    
  // | |   |                                                           |                       |   |                    
  // | |   |  OldTarget +  DeltaValues = NewTarget                     |                       |   |                    
  // | |   |                                                           |                       |   |                   
  // | |   |                                                           |                       |   |
  // | |   +-----------------------------------------------------------+                       |   |
  // | +---------------------------------------------------------------------------------------+   |
  // |                                                                                             |
  // +---------------------------------------------------------------------------------------------+
  
  // Main Frame
  frplPl=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|FRAME_NONE|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0,40,40,20,20);
  
  //frplPl->matrix
  FXMatrix* matrix=new FXMatrix(frplPl,1,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  //matrix->matrix_main 
  FXMatrix* matrix_main=new FXMatrix(matrix,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  //matrix_main->matrix_upperleft
  FXMatrix* matrix_upperleft=new FXMatrix(matrix_main,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y); 
  
  //matrix_main->gbRegDist
  FXGroupBox* gbRegDist = 
    new FXGroupBox(matrix_main, "Z-Frame Points for Registration",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  FXMatrix* matrix_upperright_main=new FXMatrix(gbRegDist,1,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
  FXMatrix* matrix_upperright=new FXMatrix(matrix_upperright_main,5,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
  
  //First row
  new FXLabel(matrix_upperright,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(matrix_upperright,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(matrix_upperright,"#4",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(matrix_upperright,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(matrix_upperright,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  //Second row
  new FXLabel(matrix_upperright,"#3",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(matrix_upperright,10,NULL,NULL);
  new FXTextField(matrix_upperright,10,NULL,NULL);
  new FXTextField(matrix_upperright,10,NULL,NULL);
  new FXLabel(matrix_upperright,"#5",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  //Third row
  new FXLabel(matrix_upperright,"#2",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(matrix_upperright,10,NULL,NULL);
  new FXLabel(matrix_upperright,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(matrix_upperright,10,NULL,NULL);
  new FXLabel(matrix_upperright,"#6",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  //Fourth row
  new FXLabel(matrix_upperright,"#1",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(matrix_upperright,10,NULL,NULL);
  new FXLabel(matrix_upperright,"  ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(matrix_upperright,10,NULL,NULL);
  new FXLabel(matrix_upperright,"#7",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  //Z_Frame sample image
  char buf[73728];
  //FILE* fp = fopen("icon/Z_Frame.gif", "rb");
  //fread(buf, 1, 73728, fp);
  //fclose(fp);
  
  new FXButton(matrix_upperright_main, "",
               new FXGIFIcon(this->getApp(), (void*)buf),NULL,NULL,BUTTON_TOOLBAR|
               LAYOUT_CENTER_X|LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  
  
  //Canvas RA-Position
  FXGroupBox* gpRAPos = 
    new FXGroupBox(matrix_upperleft, "RA-Position",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  canvas=new FXCanvas(gpRAPos,this,ID_CANVAS2,
                      FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|
                      LAYOUT_FILL_Y|LAYOUT_FILL_ROW,LAYOUT_FILL_COLUMN);
  
  //Font for canvas
  canvasFont0 = new FXFont(getApp(),"helvetica", 9);
  canvasFont0->create();
  
  //RA-Position Sliders 
  FXGroupBox* gbPlateA = 
    new FXGroupBox(matrix_upperleft, "A-Axis",
                   FRAME_RIDGE|LAYOUT_RIGHT|LAYOUT_FILL_Y);
  
  
  FXGroupBox* gbPlateR =
    new FXGroupBox(matrix_upperleft, "R-Axis",
                   FRAME_RIDGE|LAYOUT_BOTTOM|LAYOUT_FILL_X);
  
  new FXButton(matrix_upperleft, "Home",
               NULL,this,ID_PAINT_TARGET,BUTTON_NORMAL|LAYOUT_CENTER_X|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  
  FXRealSlider* slider=new FXRealSlider(gbPlateR,dtDeltaTarget[0],FXDataTarget::ID_VALUE,
                                        LAYOUT_LEFT|LAYOUT_FIX_HEIGHT|LAYOUT_FIX_WIDTH|
                                        SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,
                                        0,0,200,20);//(??,??,X,Y)
  slider->setRange(-valOldTarget[0],200.0-valOldTarget[0]);
  FXRealSlider* slider2=new FXRealSlider(gbPlateA,dtDeltaTarget[1],FXDataTarget::ID_VALUE,
                                         LAYOUT_TOP|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|
                                         SLIDER_VERTICAL|SLIDER_INSIDE_BAR,
                                         0,0,20,200);
  slider2->setRange(-valOldTarget[1],200.0-valOldTarget[1]);
  slider2->setIncrement(0.01);
  
  FXMatrix* matrix_main2=new FXMatrix(matrix,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  FXMatrix* matrix_lowerleft=new FXMatrix(matrix_main2,5,MATRIX_BY_COLUMNS|
                                          LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  FXGroupBox* gbSlPos = 
    new FXGroupBox(matrix_lowerleft, "Old Target",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  FXMatrix* matrix_lowerleft_target=new FXMatrix(gbSlPos,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|
                                                 LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  new FXLabel(matrix_lowerleft_target,"R",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(matrix_lowerleft_target,10,dtOldTarget[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(matrix_lowerleft_target,"A",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(matrix_lowerleft_target,10,dtOldTarget[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(matrix_lowerleft_target,"S",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(matrix_lowerleft_target,10,dtOldTarget[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(matrix_lowerleft,"+",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  FXGroupBox* gbDeltaTarget = 
    new FXGroupBox(matrix_lowerleft, "Delta Values",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  FXMatrix* matrix_lowerleft_delta=new FXMatrix(gbDeltaTarget,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|
                                                LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  new FXLabel(matrix_lowerleft_delta,"R",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(matrix_lowerleft_delta,10,dtDeltaTarget[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  new FXLabel(matrix_lowerleft_delta,"A",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(matrix_lowerleft_delta,10,dtDeltaTarget[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(matrix_lowerleft_delta,"S",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(matrix_lowerleft_delta,10,dtDeltaTarget[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(matrix_lowerleft,"=",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  FXGroupBox* gbSumTarget = 
    new FXGroupBox(matrix_lowerleft, "New Target",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  FXMatrix* matrix_lowerleft_sum=new FXMatrix(gbSumTarget,3,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|
                                              LAYOUT_FILL_X|LAYOUT_FILL_Y);

  new FXLabel(matrix_lowerleft_sum,"R",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(matrix_lowerleft_sum,10,dtNewTarget[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(matrix_lowerleft_sum,"",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXLabel(matrix_lowerleft_sum,"A",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(matrix_lowerleft_sum,10,dtNewTarget[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXButton(matrix_lowerleft_sum,"&Move to Target",NULL,this,ID_MOVETO_TARGET,FRAME_RAISED|
               FRAME_THICK| LAYOUT_CENTER_X|LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  new FXLabel(matrix_lowerleft_sum,"S",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(matrix_lowerleft_sum,10,dtNewTarget[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(matrix_lowerleft_sum,"",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  // Quit
  FXGroupBox* gbQuit = 
    new FXGroupBox(matrix_main2, "Close Window",
                   FRAME_RIDGE|LAYOUT_FILL_Y|
                   LAYOUT_TOP|LAYOUT_RIGHT|LAYOUT_SIDE_RIGHT);
  
  new FXButton(gbQuit,"&Quit",NULL,this,ID_CANCEL,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  
  //Timer for OnCmdTimer, paint all new
  getApp()->addTimeout(this,ID_TIMER,80);
  
}


// Destructor
MrsvrZFrameRegistrationDialog::~MrsvrZFrameRegistrationDialog()
{
  getApp()->removeTimeout(this,ID_TIMER);
}

//Create new dc
long MrsvrZFrameRegistrationDialog::onPaint(FXObject*,FXSelector,void* ptr)
{
  FXEvent *ev=(FXEvent*)ptr;
  FXDCWindow dc(canvas,ev);
  dc.setForeground(FXRGB(255,255,255));
  dc.fillRectangle(ev->rect.x,ev->rect.y,ev->rect.w,ev->rect.h);
  return 1;
}

//Clear Canvas
long MrsvrZFrameRegistrationDialog::onCmdClear()
{
  FXDCWindow dc(canvas);
  dc.setForeground(FXRGB(255,255,255));
  dc.fillRectangle(0,0,200,200);
  return 1;
}

long MrsvrZFrameRegistrationDialog::onPaintTarget()
{
  FXDCWindow dc(canvas);
  //Draw New Target [Zeroposition=(-5,-5)]
  dc.setForeground(FXRGB(255,0,0));
  //dc.drawEllipse((valNewTarget[0]-5),(200-valNewTarget[1]-5),10,10);
  dc.drawPoint(valNewTarget[0]-5,200-valNewTarget[1]-5);

  //Draw Old Target
  dc.setForeground(FXRGB(0,0,255));
  //dc.drawEllipse((valOldTarget[0]-5),(200-valOldTarget[1]-5),11,11);
  dc.drawPoint(valOldTarget[0]-5,200-valOldTarget[1]-5);
  
  //New Function necessary for this kind of calculations//////
  for (int i = 0; i < 3; i ++) {                       //////
    valNewTarget[i]=(valDeltaTarget[i]+valOldTarget[i]); /////
  }                                                    ////
  ////////////////////////////////////////////////////////
  
  //draw Text NewTarget
  dc.setForeground(FXRGB(255,0,0));
  dc.setFont(canvasFont0);
  dc.drawText(valNewTarget[0]-25, 200-valNewTarget[1]-5,"NewTarget", 9);
  
  //draw Text OldTarget
  dc.setForeground(FXRGB(0,0,255));
  dc.setFont(canvasFont0);
  dc.drawText(valOldTarget[0]-25, 200-valOldTarget[1]+15,"OldTarget", 9);
  
  return 1;
}

//Timed function for deleting and repainting canvas
long MrsvrZFrameRegistrationDialog::onCmdTimer(FXObject*,FXSelector,void*)
{
  
  //Next call this Funktion in 80ms
  getApp()->addTimeout(this,ID_TIMER,80);
  
  //Delete old canvas
  MrsvrZFrameRegistrationDialog::onCmdClear();
  //Paint new canvas
  MrsvrZFrameRegistrationDialog::onPaintTarget();
  
  return 1;
  
}

long MrsvrZFrameRegistrationDialog::onCmdRunRegistration(FXObject*,FXSelector,void*)
{
  return 1;
}


void MrsvrZFrameRegistrationDialog::getRegistrationMatrix(float matrix[16])
{
  for (int i = 0; i < 16; i ++) {
    matrix[i] = registrationMatrix[i];
  }
  
}

/*******************************************************************************/// End Maier




