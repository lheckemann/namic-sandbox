
/*=========================================================================
Module:$RCSfile: vtkKWNIREPImageViewerWithScrollbar.h,v $
Copyright (c) NIREP, The University of Iowa
All rights reserved
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#pragma once

#include <vtkKWCompositeWidget.h> 
#include <vtkKWApplication.h>
#include <vtkImageViewer2.h> 
#include <vtkKWRenderWidget.h>
#include <vtkXMLImageDataReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkKWSplitFrame.h> 
#include <vtkKWFrame.h>
#include <vtkObjectFactory.h>
#include <vtkKWTkUtilities.h>
#include <vtkKWScale.h>

#include <vtkCornerAnnotation.h>
#include <vtkKWLabel.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>

#include "vtkImageCanvasSource2D.h"

#include <vtksys/SystemTools.hxx>

//----------------------------------------------------------------------------

#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkImage.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPolyDataReader.h"

#include "vtkImageData.h"
#include "vtkImageViewer.h"

#include <stdio.h>
#include <stdlib.h>

using namespace std;
//----------------------------------------------------------------------------
class vtkKWFrame;
// .NAME vtkKWNIREPImageViewerWithScrollbar - Create window with imageviewer and scrollbar
// TODO: Replace the SliceScale and RenderWidget with the vtkKWWindowWithScrollBar
// .SECTION Description
//----------------------------------------------------------------------------
  typedef unsigned short                            PixelType;
  typedef itk::Image< PixelType, 2 >                ImageType;
  typedef itk::ImageFileReader< ImageType >         ReaderType;
  typedef itk::ImageToVTKImageFilter< ImageType >   ConnectorFilterType;
  
class KWWidgets_EXPORT vtkKWNIREPImageViewerWithScrollbar : public vtkKWCompositeWidget 
{
public:
static vtkKWNIREPImageViewerWithScrollbar* New();
vtkTypeRevisionMacro(vtkKWNIREPImageViewerWithScrollbar,vtkKWCompositeWidget);
// Description:
// This method prints all the member variables for debugging. 
void PrintSelf(ostream& os, vtkIndent indent); //not fully implemeneted
// Description:
// Set the filename before invoking vtkNIREPGripImageViewer::Create method. This method loads
  // the current file into the RenderWindow.
  void SetFileName(std::string &str);
  // Transverse Slice
  void AxisXY();
  void Tranverse(){AxisXY();}
  // Coronal Slice
void AxisXZ();
  void Coronal(){AxisXZ();}
  // Sagittal Slice
void AxisYZ();
  void Sagittal(){AxisYZ();}
// Description: 
  // The slice scale callback function
  void SliceScaleCallback(double value);

  // Hold the string of the filename
  std::string m_sStr;

  // VTK imageviewer, reference
vtkImageViewer2 *m_pImageViewer; 
  // KWwidget RenderWidget and SliceScale
  vtkKWScale *m_pSliceScale;
  vtkKWRenderWidget *m_pRenderWidget;
  // ITK smart pointer and ConnectorFilter
  ReaderType::Pointer  m_pReader;
  ConnectorFilterType::Pointer m_pConnector;
protected:
  vtkKWNIREPImageViewerWithScrollbar();
  ~vtkKWNIREPImageViewerWithScrollbar();
  // Description:
  // Create the widget.
  virtual void CreateWidget();
};
/*=========================================================================
Module:  $RCSfile: vtkKWNIREPGridImageViewer.h,v $
Copyright (c) NIREP, The University of Iowa
All rights reserved
 This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.
========================================================================*/
// .NAME vtkKWNIREPGridImageViewer - A ImageView that contains multiple RenderWidets of adjustable sub frames.
// .SECTION Description
class KWWidgets_EXPORT vtkKWNIREPGridImageViewer : public vtkKWCompositeWidget 
{
public:
static vtkKWNIREPGridImageViewer* New();
vtkTypeRevisionMacro(vtkKWNIREPGridImageViewer,vtkKWCompositeWidget);
void PrintSelf(ostream& os, vtkIndent indent); //not implemeneted
// Description:
// (description needed)
virtual void SetRowSize(int val){this->m_iRowSize = val; m_prowlabels = new char*[val];}
vtkGetMacro(m_iRowSize, int);
// Description:
// (description needed)
virtual void SetColSize(int val){this->m_iColSize = val; m_pcolumnlabels = new char*[val];}
vtkGetMacro(m_iColSize, int);
// Description:
// Override the Superclass Delete method to free ImageViewer2 buffers 
// before unregistering inhereting objects and invoking their destructs
 // Description:
 // 
virtual void SetColumnLabel(char * label, int col){m_pcolumnlabels[col] = new char[strlen(label) + 1];strcpy(m_pcolumnlabels[col],label);}
// Description:
  // 
  virtual char * GetColumnLabel(int i){return m_pcolumnlabels[i];}
  //Description
  //
  void Update();
  // Description:
  // Set the number of rows. Call this method with vtkKWNIREPGridImageViewer::SetColumnLabel before invoking
  // the vtkNIREPGridImageViewer::Create
virtual void SetRowLabel(char * label, int row)
  {m_prowlabels[row] = ReformatRowLabel(label);}
  // Description:
 // 
virtual char * GetRowLabel(int i){return m_prowlabels[i];}
  // Description:
  //
  virtual void Add(std::string &p_sStr, int p_iRow, int p_iCol)
  {m_pViewWithScrollbar[ p_iCol * m_iRowSize + p_iRow ]->SetFileName( p_sStr );}
  // Description:
  //
  virtual void Remove(vtkKWNIREPImageViewerWithScrollbar &p_pViewWithScrollbar ){;}
  // Description:
  // Set the size of the grid before invoking the vtkNIREPGridImageViewer::Create method.
  virtual void SetSize(const int width, const int height)
    {SetRowSize(height);SetColSize(width);}
  // Description:
protected:
vtkKWNIREPGridImageViewer();
~vtkKWNIREPGridImageViewer();
// Description:
// Create the widget.
virtual void CreateWidget();
vtkKWNIREPImageViewerWithScrollbar *(* m_pViewWithScrollbar);
int m_iRowSize;
int m_iColSize;
char * (* m_pcolumnlabels);
char * (* m_prowlabels);
private:
char * ReformatRowLabel(char * str);
};
