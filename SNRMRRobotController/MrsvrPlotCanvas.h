//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2002-2005 by Junichi Tokuda. All Right Reserved.
// Copyright (C) 2002-2005 by The Univ. of Tokyo, All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrPlotCanvas.h,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description: 
//    Plotter module for the consol of MRI guided robot control system.
//
//====================================================================


#ifndef  _INC_MRSVR_PLOT_CANVS
#define _INC_MRSVR_PLOT_CANVS

#ifdef DEBUG
//#define DEBUG_MRSVR_PLOT_CANVAS
#endif

#define PLOT_NUM_DATA     10

#include "fx.h"

class MrsvrPlotCanvas : public FXCanvas {

  FXDECLARE(MrsvrPlotCanvas);
 public:
  
  //Messages
  enum {
    ID_PLOT,
    ID_GAIN,
    ID_YOFFSET,
    ID_LAST
  };

  // plot mode
  enum {
    MODE_REFRESH_ALL,
    MODE_SWEEP,
  };

 protected:

  //FXCanvas*      canvas;
  FXComposite*   parentComp;
  FXImage*       offscreen;   // offscreen buffer

  FXColor        colorBg;
  FXColor        colorAxis;

  // Parameters for drawing waveforms
  // - common parameters
  FXFunction     lineDrawFunction;
  FXCapStyle     lineCapStyle;
  FXJoinStyle    lineJoinStyle;

  int            plotMode;
  int            nLines;              // < PLOT_NUM_DATA
  float          offsetYAll;
  float          scaleYAll;
  int            numGradX;            // number of graduations on x axis
  int            numGradY;            // number of graduations on y axis
  float          pUnitX;              // unit length for 1st dim.
  float          pUnitY;              // unit length for 2nd dim. (usually < 0)
  int            gradSpaceX;          // spacing of graduations on x axis (# of units)
  int            gradSpaceY;          // spacing of graduations on y axis (# of units)
  float          valuePerUnitX;        // value per unit for x axis graduations 
  int            gradOrgX;            // origin of graduation
  int            gradOrgY;            // origin of graduation

  int            maxXGradNumHeight;   // height of graduation label
  int            maxXGradNumWidth;    // width of graduation label
  int            maxYGradNumHeight;
  int            maxYGradNumWidth;
  
  float          unitX;               // unit length for 1st dim. (in pixel)
  float          unitY;               // unit length for 2nd dim. (in pixel)

  FXString*      xLabel;
  FXString*      yLabel;
  FXFont*        labelFont;

  FXString**     xGrad;
  FXString**     yGrad;
  int*           xGradPos;
  int*           yGradPos;

  // - parameters for each plot
  int            plotPoints[PLOT_NUM_DATA];
  int            plotCursor[PLOT_NUM_DATA];  // currentn point in sweep mode
  float          scaleX[PLOT_NUM_DATA];
  float          scaleY[PLOT_NUM_DATA];
  float          offsetY[PLOT_NUM_DATA];
  float*         data[PLOT_NUM_DATA];

  FXColor        lineColor[PLOT_NUM_DATA];
  FXLineStyle    lineStyle[PLOT_NUM_DATA];
  FXuint         lineWidth[PLOT_NUM_DATA];
  FXfloat        lineStepX[PLOT_NUM_DATA];
  FXString*      legend[PLOT_NUM_DATA];

  int            legendW[PLOT_NUM_DATA];
  int            legendH[PLOT_NUM_DATA];

 private:
  // Graph Layout Parameters in pixel
  int    zeroX;
  int    zeroY;
  int    xAxisMin;
  int    xAxisMax;
  int    yAxisMin;
  int    yAxisMax;
  int    xAxisIntv;
  int    yAxisIntv;
  int    xAxisWidth;
  int    yAxisWidth;

  int    xLabelX;
  int    xLabelY;
  int    xLabelW;
  int    xLabelH;

  int    yLabelX;
  int    yLabelY;
  int    yLabelW;
  int    yLabelH;

  int    windowSizeX;
  int    windowSizeY;

  int    windowMargTop;   // Top margin (in pixel)
  int    windowMargBot;   // Bottom margin
  int    windowMargLt;    // Left margin
  int    windowMargRt;    // Right margin

  int    legendTotalW;
  int    legendTotalH;

  // proportional
  FXfloat   pZeroX;
  FXfloat   pZeroY;
  FXfloat   pXAxisMin;
  FXfloat   pXAxisMax;
  FXfloat   pYAxisMin;
  FXfloat   pYAxisMax;
  FXfloat   pXAxisIntv;
  FXfloat   pYAxisIntv;

  FXfloat   pXLabelX;
  FXfloat   pXLabelY;
  FXfloat   pYLabelX;
  FXfloat   pYLabelY;

  //FXfloat   pLegendX;
  //FXfloat   pLegendY;

  // for plot
  FXPoint*  pointsBuffer;
  int       maxPoints;
  int       triggerFd;

  // flag for updates in axis parameters
  int       fUpdateAxisParam;


 public:   // Event handler
  long onCmdGain(FXObject*, FXSelector, void*);
  long onCmdYOffset(FXObject*, FXSelector, void*);
  
 private:
  void calcAxisParam();

 protected:
  MrsvrPlotCanvas();

 public:
  MrsvrPlotCanvas(FXComposite* p, FXuint opts=FRAME_NORMAL,
                  FXint x=0,FXint y=0,FXint w=0,FXint h=0);
  ~MrsvrPlotCanvas();

  long onCanvasRepaint(FXObject*, FXSelector, void*);

  void setBgColor(FXColor color) {colorBg = color;};
  void setAxisStyle(FXColor, FXuint);
  void setAxisScaleX(float, int, float);
  void setAxisScaleY(float, int);
  void setLineProperty(int, FXColor, FXLineStyle, FXuint);
  inline void setPlotScale(int id, float scale)
    {scaleY[id] = scale;};
  inline void setPlotOffset(int id, float offset)
    {offsetY[id] = offset;};
  inline void setPlotScaleAll(float scale)
    {scaleYAll = scale;};
  inline void setPlotOffsetAll(float offset)
    {offsetYAll = offset;};
  void setPlotMode(int);
  void resetSweep();
  void setLabel(FXchar*, FXchar*);
  int  addData(short*, int, int);  // to be erased
  //int  addData(short*, int, int, FXString, FXString);
  int  newData(int);
  int  newData(int, FXchar*);

  void setLegend(int, FXchar*);
  void updateData(short*, int);
  void updateData(float*, int);
  void updateData(short, int);     // for sweep mode
  void updateData(float, int);     // for sweep mode
  void create();
  void update();
  void setTriggerFd(int);
  void repaintTrigger();
};


#endif // _INC_MRSVR_PLOT_CANVS











