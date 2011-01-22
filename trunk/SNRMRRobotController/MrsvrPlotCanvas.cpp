//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2002-2005 by Junichi Tokuda. All Right Reserved.
// Copyright (C) 2002-2005 by The Univ. of Tokyo, All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrPlotCanvas.cpp,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description: 
//    Plotter module for the consol of MRI guided robot control system.
//====================================================================


#include <unistd.h>
#include <math.h>
#include <string.h>
#include "MrsvrPlotCanvas.h"


// Map
FXDEFMAP(MrsvrPlotCanvas) MrsvrPlotCanvasMap[] = {
  //Message_Type________ID__________________________Message_Handler__________________
  FXMAPFUNC(SEL_PAINT,  MrsvrPlotCanvas::ID_PLOT,   MrsvrPlotCanvas::onCanvasRepaint),
//FXMAPFUNC(SEL_COMMAND,MrsvrPlotCanvas::ID_GAIN,   MrsvrPlotCanvas::onCmdGain     ),
//FXMAPFUNC(SEL_COMMAND,MrsvrPlotCanvas::ID_YOFFSET,MrsvrPlotCanvas::onCmdYOffset  ),
};

// Object implementation
FXIMPLEMENT(MrsvrPlotCanvas,FXObject,MrsvrPlotCanvasMap,ARRAYNUMBER(MrsvrPlotCanvasMap))


MrsvrPlotCanvas::MrsvrPlotCanvas() : FXCanvas::FXCanvas()
{
  
}

MrsvrPlotCanvas::MrsvrPlotCanvas(FXComposite* p, FXuint opts, FXint x,
                                 FXint y,FXint w,FXint h) 
  :FXCanvas(p, this, MrsvrPlotCanvas::ID_PLOT, opts, x, y, w, h)
{


#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::MrsvrPlotCanvas()\n");
#endif //DEBUG_MRSVR_PLOT_CANVAS

  // initilaize control parameter
  //gain = 100;
  //yOffset = 0;
  // set colors
  colorBg   = FXRGB(0,0,0);
  colorAxis = FXRGB(180,180,180);

  windowSizeX = 0;
  windowSizeY = 0;
  
  xAxisWidth = 1;
  yAxisWidth = 1;

  pZeroX      = 0.0;
  //pZeroY      = 0.9;
  pZeroY      = 0.5;

  pXAxisMin   = 0.0;
  pXAxisMax   = 1.0;
  pYAxisMin   = 1.0;
  pYAxisMax   = 0.0;
  pXAxisIntv  = 0.0;
  pYAxisIntv  = 0.0;

  pXLabelX    = pXAxisMax;
  pXLabelY    = pZeroY;
  pYLabelX    = pZeroX;
  pYLabelY    = pYAxisMax;

  windowMargTop = 25;
  windowMargBot = 25;
  windowMargLt  = 30;
  windowMargRt  = 25;

  windowSizeX = 0;
  windowSizeY = 0;
  
  maxPoints    = 0;
  pointsBuffer = NULL;

  numGradX     = 0;
  numGradY     = 0;
  
  // initialize line parameters
  nLines = 0;
  lineDrawFunction = (FXFunction)BLT_SRC;
  lineCapStyle     = (FXCapStyle)CAP_BUTT;
  lineJoinStyle    = (FXJoinStyle)JOIN_ROUND;

  xLabel           = new FXString("x");
  yLabel           = new FXString("y");

  gradSpaceX       = 10;
  gradSpaceY       = 10;

  pUnitX            = (pXAxisMax-pXAxisMin) / 100.0;
  pUnitY            = (pYAxisMax-pYAxisMin) / 100.0;

  calcAxisParam();

  for (int i = 0; i < PLOT_NUM_DATA; i ++) {
    plotPoints[i] = 0;
    plotCursor[i] = 0;

    lineColor[i]  = FXRGB(255,255,255);
    lineStyle[i]  = (FXLineStyle)LINE_SOLID;
    lineWidth[i]  = 1;
    lineStepX[i]  = 0.1;
    offsetY[i]    = 0.0;
    scaleY[i]     = 1.0;
  }

  offsetYAll = 0.0;
  scaleYAll  = 1.0;

  triggerFd = -1;
  plotMode  = MODE_REFRESH_ALL;

  // GUI component
  parentComp = p;
  labelFont  = new FXFont(parentComp->getApp(), "helvetica", 9);
  
  w = ((w == 0)? 1000 : w);
  h = ((h == 0)? 1000 : w);
  offscreen  = new FXImage(parentComp->getApp(), NULL, 
                           IMAGE_SHMI|IMAGE_SHMP|IMAGE_KEEP, w, h);

}


MrsvrPlotCanvas::~MrsvrPlotCanvas()
{
}


void MrsvrPlotCanvas::setTriggerFd(int fd)
{
  triggerFd = fd;
}

void MrsvrPlotCanvas::repaintTrigger()
{
  if (triggerFd > 0)
    write(triggerFd, "r", 1);
}


void MrsvrPlotCanvas::create()
{
  FXCanvas::create();
  labelFont->create();
  offscreen->create();
}

void MrsvrPlotCanvas::update()
{
  FXCanvas::update();
}


long MrsvrPlotCanvas::onCanvasRepaint(FXObject* sender, FXSelector, void* ptr)
{
  int fResize;

#ifdef DEBUG_MRSVR_PLOT_CANVAS
  //printf("MrsvrPlotCanvas::onCanvasRepaint()\n");
#endif //DEBUG_MRSVR_PLOT_CANVAS

  FXEvent  *ev   = (FXEvent*) ptr;
  int w = getWidth();
  int h = getHeight();

  // If the window size has been changed,
  // update all parameters for drawing axes.
  if (windowSizeX != w || windowSizeY != h) {
    fResize = 1;
    windowSizeX = w;
    windowSizeY = h;
  } else {
    fResize = 0;
  }

  if (ev->synthetic || fResize) { // update offscreen
#ifdef DEBUG_MRSVR_PLOT_CANVAS
    printf("MrsvrPlotCanvas::onCanvasRepaint(): update offscreen\n");
#endif //DEBUG_MRSVR_PLOT_CANVAS


    if (fUpdateAxisParam) {
      calcAxisParam();
      fResize = TRUE;
    }

    FXfloat fw = (FXfloat) w;
    FXfloat fh = (FXfloat) h;

    if (fResize) { 
      offscreen->resize(w, h);
      FXfloat mw = fw - (float)(windowMargLt+windowMargRt);
      FXfloat mh = fh - (float)(windowMargTop+windowMargBot);
      
      zeroX      = (FXuint) (pZeroX * mw) + windowMargLt;
      zeroY      = (FXuint) (pZeroY * mh) + windowMargTop;
      xAxisMin   = (FXuint) (pXAxisMin * mw) + windowMargLt;
      xAxisMax   = (FXuint) (pXAxisMax * mw) + windowMargLt;
      yAxisMin   = (FXuint) (pYAxisMin * mh) + windowMargTop;
      yAxisMax   = (FXuint) (pYAxisMax * mh) + windowMargTop;
      xAxisIntv  = (FXuint) (pXAxisIntv * mw);
      yAxisIntv  = (FXuint) (pYAxisIntv * mh);

      xLabelW    = labelFont->getTextWidth(xLabel->text(), xLabel->length());
      xLabelH    = labelFont->getTextHeight(xLabel->text(), xLabel->length());
      yLabelW    = labelFont->getTextWidth(yLabel->text(), yLabel->length());
      yLabelH    = labelFont->getTextHeight(yLabel->text(), yLabel->length());

      xLabelX    = xAxisMax - xLabelW;
      xLabelY    = zeroY + xLabelH*2;
      yLabelX    = zeroX + 2;
      yLabelY    = yAxisMax;

      // Calcurate the position of graduations for each axis.

      // pixel / unit
      unitX = ((float)(xAxisMax - xAxisMin)) * pUnitX / (pXAxisMax - pXAxisMin);
      float dx  = unitX * gradSpaceX;
      maxXGradNumHeight = labelFont->getTextHeight(xGrad[numGradX-1]->text(),
                                                   xGrad[numGradX-1]->length());
      maxXGradNumWidth  = labelFont->getTextWidth(xGrad[numGradX-1]->text(),
                                                  xGrad[numGradX-1]->length());
      for (int i = 0; i < numGradX; i ++) {
        xGradPos[i] = zeroX + (int) dx * (i-gradOrgX);
      }
      
      unitY = ((float)(yAxisMax - yAxisMin)) * pUnitY / (pYAxisMax - pYAxisMin);
      float dy  = unitY * gradSpaceY;
      maxYGradNumHeight = labelFont->getTextHeight(yGrad[numGradY-1]->text(),
                                                   yGrad[numGradY-1]->length());
      maxYGradNumWidth  = labelFont->getTextWidth(yGrad[0]->text(),
                                                  yGrad[0]->length());
      for (int i = 0; i < numGradY; i ++) {
        yGradPos[i] = zeroY + (int) dy * (i-gradOrgY);
      }
      
      legendTotalW = legendTotalH = 0;
      for (int i = 0; i < nLines; i ++) {
        if (legend[i]) {
          legendW[i] = labelFont->getTextWidth(legend[i]->text(),
                                               legend[i]->length());
          if (legendW[i] > legendTotalW) {
            legendTotalW = legendW[i];
          }
          legendH[i] = labelFont->getTextHeight(legend[i]->text(),
                                                legend[i]->length());
          legendTotalH += legendH[i] +1;
        }
      }
      
    }

    FXDCWindow dc(offscreen);
    //dc.setTextFont(labelFont);
    dc.setFont(labelFont);    // for fox 1.1
    
    dc.setForeground(colorBg);
    dc.fillRectangle(0, 0, w, h);
    
    // Draw Axes
    dc.setForeground(colorAxis);
    dc.setLineStyle(LINE_SOLID);
    dc.setLineCap(CAP_BUTT);
    dc.setFunction(BLT_SRC);
    dc.setLineJoin(JOIN_ROUND);
    
    dc.setLineWidth(xAxisWidth);
    dc.drawLine(xAxisMin, zeroY, xAxisMax, zeroY); // x axis
    for (int i = 0; i < numGradX; i ++) {
      dc.drawLine(xGradPos[i], zeroY+2, xGradPos[i], zeroY-1);
      dc.drawText(xGradPos[i], zeroY+maxXGradNumHeight,
                  xGrad[i]->text(), xGrad[i]->length());
    }

    dc.setLineWidth(yAxisWidth);
    dc.drawLine(zeroX, yAxisMin, zeroX, yAxisMax); // y axis
    for (int i = 0; i < numGradY; i ++) {
      dc.drawLine(zeroX-2, yGradPos[i], zeroX+1, yGradPos[i]);
      dc.drawText(zeroX-maxYGradNumWidth-2, yGradPos[i]+maxYGradNumHeight/2, 
                  yGrad[i]->text(), yGrad[i]->length());
    }

    dc.drawText(xLabelX, xLabelY, xLabel->text(), xLabel->length());
    dc.drawText(yLabelX, yLabelY, yLabel->text(), yLabel->length());

    int x = windowSizeX - legendTotalW - windowMargRt;
    int y = windowSizeY - legendTotalH - windowMargBot;
    int cy = y;
    for (int i = 0; i < nLines; i ++) {
      if (legend[i]) {
        dc.drawText(x, cy, legend[i]->text(), legend[i]->length());
        cy += legendH[i] + 1;
      }
    }

    // Plot data
    dc.setLineCap(lineCapStyle);
    dc.setFunction(lineDrawFunction);
    dc.setLineJoin(lineJoinStyle);
    cy = y - legendH[0]/2;
#ifdef DEBUG_MRSVR_PLOT_CANVAS
    printf("MrsvrPlotCanvas::onCanvasRepaint(): nLines = %d\n", nLines);
#endif //DEBUG_MRSVR_PLOT_CANVAS

    for (int i = 0; i < nLines; i ++) {
#ifdef DEBUG_MRSVR_PLOT_CANVAS    
      printf("MrsvrPlotCanvas::onCanvasRepaint(): plotPoints[%d] = %d\n", i, plotPoints[i]);
      printf("MrsvrPlotCanvas::onCanvasRepaint(): scaleY[%d] = %f\n", i, scaleY[i]);
#endif //DEBUG_MRSVR_PLOT_CANVAS
      dc.setForeground(lineColor[i]);
      dc.setLineWidth(lineWidth[i]);
      dc.setLineStyle(lineStyle[i]);
      
      // for legend
      if (legend[i]) {
        dc.drawLine(x-30, cy, x-10, cy);
        cy += legendH[i] + 1;
      }

      //float sy = scaleY[i] * (float)(yAxisMin - yAxisMax);
      float sy = scaleY[i]*scaleYAll*unitY;
      float oy = offsetYAll + offsetY[i];
      float* dptr = data[i];
      float intv = lineStepX[i] * unitX;
      FXPoint* pptr = pointsBuffer;
      int np = plotPoints[i];
      for (int n = 0; n < np; n ++) {
        pptr->x = (FXshort) (((float)n) * intv) + zeroX;
        pptr->y = (FXshort) (sy * (dptr[n]-oy)) + zeroY;
        //printf("MrsvrPlotCanvas::onCanvasRepaint(): (x, y) = (%d, %d)\n", pptr->x, pptr->y);
        pptr ++;
      }
      if (plotPoints[i] > 2) {
        //dc.drawLines(pointsBuffer, plotPoints[i]);
        dc.drawLines(pointsBuffer, plotCursor[i]);
        if (plotMode == MODE_SWEEP && plotCursor[i] < np) {}
        //dc.drawLines(pointsBuffer+plotCursor[i]+1, np - plotCursor[i]);
      }
    }
  }

  //FXDCWindow sdc(canvas, ev);
  FXDCWindow sdc(this, ev);
  //sdc.fillRectangle(0, 0, w, h);
  sdc.drawImage(offscreen, 0,0);

  return 1;
}


#if 0
long MrsvrPlotCanvas::onCmdGain(FXObject* sender,FXSelector sel,void* ptr)
{
  //printf("MrsvrPlotCanvas::onCmdGainDial()\n");
  for (int i = 0; i < nLines; i ++) {
    scaleY[i] = (float)valGain / 100.0;
#ifdef DEBUG_MRSVR_PLOT_CANVAS
    printf("MrsvrPlotCanvas::onCmdGain(): scaleY[%d] = %f\n", i, scaleY[i]);
#endif //DEBUG_MRSVR_PLOT_CANVAS
  }
  this->update();
  //rootWindow->repaintTrigger();
  repaintTrigger();
  return 1;
}


long MrsvrPlotCanvas::onCmdYOffset(FXObject* sender,FXSelector sel,void* ptr)
{
  return 1;
}

#endif


void MrsvrPlotCanvas::setAxisStyle(FXColor color, FXuint width)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::setAxisStyle()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
  xAxisWidth = yAxisWidth = width;
  colorAxis  = color;
}


void MrsvrPlotCanvas::setAxisScaleX(float unit, int gsp, float vpu)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::setAxisScaleX()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
  pUnitX = unit;
  gradSpaceX = gsp;
  valuePerUnitX = vpu;
  for (int i = 0; i < nLines; i ++) {
    lineStepX[i]  = ((pXAxisMax - pXAxisMin)/(pUnitX*(float)plotPoints[i])); 
  }
  fUpdateAxisParam = 1;
}


void MrsvrPlotCanvas::setAxisScaleY(float unit, int gsp)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::setAxisScaleY()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
  pUnitY = -unit;
  gradSpaceY = gsp;
  fUpdateAxisParam = 1;
}


void MrsvrPlotCanvas::calcAxisParam()
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::calcAxisParam()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS

  int ngx = numGradX;
  int ngy = numGradY;

  numGradX  = (int)((pXAxisMax-pXAxisMin) / (pUnitX*(float)gradSpaceX)) + 1;
  numGradY  = (int)((pYAxisMax-pYAxisMin) / (pUnitY*(float)gradSpaceY)) + 1;
  gradOrgX  = (int)((float)(numGradX-1) * (pZeroX-pXAxisMin) / (pXAxisMax-pXAxisMin));
  gradOrgY  = (int)((float)(numGradY-1) * (pZeroY-pYAxisMin) / (pYAxisMax-pYAxisMin));
  
  if (((float)((numGradX-gradOrgX-1) * gradSpaceX) * pUnitX) / (pXAxisMax-pZeroX) > 1.0) {
    numGradX --;
  }
  if (((float)((numGradY-gradOrgY-1) * gradSpaceY) * pUnitY) / (pYAxisMax-pZeroY) > 1.0) {
    // note : pUnitY is usually less than 0
    numGradY --;
  }

  if (ngx != numGradX && ngx != 0) {
    delete xGradPos;
    for (int i = 0; i < ngx; i ++) {
      delete xGrad[i];
    }
  }
  if (ngy != numGradY && ngy != 0) {
    delete yGradPos;
    for (int i = 0; i < ngy; i ++) {
      delete yGrad[i];
    }
  }
  printf("%d, %d \n", numGradX, numGradY);

  xGrad     = new FXString*[numGradX];
  yGrad     = new FXString*[numGradY];
  xGradPos  = new int[numGradX];
  yGradPos  = new int[numGradY];
  
  // label graduation
  char lbstr[256];
  for (int i = 0; i < numGradX; i ++) {
    sprintf(lbstr, "%d",(int)((float)(gradSpaceX*(i-gradOrgX))*valuePerUnitX));
    xGrad[i] = new FXString(lbstr);
    //xGrad[i] = new FXString(" a ");
  }

  for (int i = 0; i < numGradY; i ++) {
    sprintf(lbstr, "%d",gradSpaceY*(i-gradOrgY));
    yGrad[i] = new FXString(lbstr);
    //yGrad[i] = new FXString(" b ");
  }
  fUpdateAxisParam = 0;
}



void MrsvrPlotCanvas::setLineProperty(int id, FXColor color, FXLineStyle style, FXuint width)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::setLineProperty(id = %d)\n", id);
#endif // DEBUG_MRSVR_PLOT_CANVAS

  if (id < nLines) {
    lineColor[id] = color;
    lineStyle[id] = style;
    lineWidth[id] = width;
  }
}


void MrsvrPlotCanvas::setPlotMode(int mode)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::setPlotMode()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
  plotMode = mode;
  if (mode == MODE_SWEEP) {
    resetSweep();
  }
}


void MrsvrPlotCanvas::resetSweep()
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::resetSweep()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS

  for (int i = 0; i < nLines; i ++) {
    plotCursor[i] = 0;
  }
}  


void MrsvrPlotCanvas::setLabel(FXchar* xl, FXchar* yl)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::setLabel()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
  if (xLabel != NULL) delete xLabel;
  if (yLabel != NULL) delete yLabel;
  xLabel = new FXString(xl);
  yLabel = new FXString(yl);
}


int MrsvrPlotCanvas::addData(short* newDataPtr, int size, int amax)
{
  
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::addData()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS

  if (nLines > PLOT_NUM_DATA) {
#ifdef DEBUG_MRSVR_PLOT_CANVAS
    printf("MrsvrPlotCanvas::addData(): # of lines is exceeded \n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
    return 0;
  }
  
  // Calculate max and min value
  float famax = (float)amax;
  if (amax == 0) {
    int max = newDataPtr[0];
    int min = max;
    short* ptr = newDataPtr;
    
    for (int i = 0; i < size; i ++) {
      if (*ptr > max) {
        max = *ptr;
      } else if (*ptr < min) {
        min = *ptr;
      }
      ptr ++;
    }
    famax = (float) 
      ((abs(max) > abs(min))? abs(max) : abs(min));
  } 

  // Allocate memory for new data.
  data[nLines] = new float[size];
  float* dptr = data[nLines];
  for (int i = 0; i < size; i ++) {
    dptr[i] = (float) newDataPtr[i] / famax;
  }
  //scaleY[nLines]   = 1.0 / famax;
  scaleY[nLines]     = 1.0;
  plotPoints[nLines] = size;
  plotCursor[nLines] = size;
  lineStepX[nLines]  = (pXAxisMax - pXAxisMin) / (pUnitX*(float)size); 
  if (maxPoints < size) {
    maxPoints = size;
    if (maxPoints) {
      delete pointsBuffer;
    }
    pointsBuffer = new FXPoint[size];
  }
  legend[nLines] = NULL;
  return nLines++;
}


int MrsvrPlotCanvas::newData(int size)
{
  
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::newData()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS

  if (nLines > PLOT_NUM_DATA) {
#ifdef DEBUG_MRSVR_PLOT_CANVAS
    printf("MrsvrPlotCanvas::newData(): # of lines is exceeded \n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
    return 0;
  }

  // Allocate memory for new data.
  data[nLines] = new float[size];

  float* dptr = data[nLines];
  for (int i = 0; i < size; i ++) {
    dptr[i] = 0.0;
  }
  scaleY[nLines]     = 1.0;
  plotPoints[nLines] = size;
  if (plotMode == MODE_SWEEP) {
    plotCursor[nLines] = 0;
  } else {
    plotCursor[nLines] = size;
  }
  lineStepX[nLines]  = ((pXAxisMax - pXAxisMin)/(pUnitX*(float)size)); 
  if (maxPoints < size) {
    maxPoints = size;
    if (maxPoints) {
      delete pointsBuffer;
    }
    pointsBuffer = new FXPoint[size];
  }
  legend[nLines] = NULL;
  return nLines++;
}

int MrsvrPlotCanvas::newData(int size, FXchar* str)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::newData()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
  
  int r = newData(size);
  setLegend(r, str);
  return r;
}



void MrsvrPlotCanvas::setLegend(int id, FXchar* str)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::setLegend()\n");
#endif // DEBUG_MRSVR_PLOT_CANVAS
    
  if (id < nLines) {
    if (legend[id]) 
      delete legend[id];
    legend[id] = new FXString(str);
  }
} 

 
void MrsvrPlotCanvas::updateData(short* newDataPtr, int id)
{
  
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::updateData(short*, int): id = %d\n", id);
#endif // DEBUG_MRSVR_PLOT_CANVAS

  float* dptr = data[id];
  int size = plotPoints[id];
  for (int i = 0; i < size; i ++) {
    dptr[i] = (float) newDataPtr[i];
    //printf("%d->%f", newDataPtr[i], dptr[i]);
  }
  //canvas->update();
  //repaintTrigger();
}


void MrsvrPlotCanvas::updateData(float* newDataPtr, int id)
{
  
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::updateData(short*, int): id = %d\n", id);
#endif // DEBUG_MRSVR_PLOT_CANVAS

  float* dptr = data[id];
  int size = plotPoints[id];
  for (int i = 0; i < size; i ++) {
    dptr[i] = newDataPtr[i];
  }
  //canvas->update();
  //repaintTrigger();
}



void MrsvrPlotCanvas::updateData(short value, int id)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::updateData(short,int): id = %d\n", id);
#endif // DEBUG_MRSVR_PLOT_CANVAS
 
  data[id][plotCursor[id]] = (float)value;
  plotCursor[id] = (plotCursor[id] + 1) % plotPoints[id];
}


void MrsvrPlotCanvas::updateData(float value, int id)
{
#ifdef DEBUG_MRSVR_PLOT_CANVAS
  printf("MrsvrPlotCanvas::updateData(value,int): id = %d\n", id);
#endif // DEBUG_MRSVR_PLOT_CANVAS
 
  data[id][plotCursor[id]] = value;
  plotCursor[id] = (plotCursor[id] + 1) % plotPoints[id];
}


