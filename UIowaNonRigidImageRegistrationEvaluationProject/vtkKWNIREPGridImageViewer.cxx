#include "vtkKWNIREPGridImageViewer.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkLODActor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkImageMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCornerAnnotation.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWNIREPImageViewerWithScrollbar );
vtkCxxRevisionMacro(vtkKWNIREPImageViewerWithScrollbar, "$Revision: 0.9 $");
//----------------------------------------------------------------------------
vtkKWNIREPImageViewerWithScrollbar::vtkKWNIREPImageViewerWithScrollbar()
{
  this->m_pRenderWidget = NULL;
  this->m_pImageViewer = NULL;
  this->m_pSliceScale = NULL;
}
//----------------------------------------------------------------------------
vtkKWNIREPImageViewerWithScrollbar::~vtkKWNIREPImageViewerWithScrollbar()
{
if (this->m_pSliceScale)
{
this->m_pSliceScale->Delete();
}
if (this->m_pImageViewer)
{
this->m_pImageViewer->Delete();
}
  if(this->m_pRenderWidget)
  {
    this->m_pRenderWidget->Delete();
  }
}
//----------------------------------------------------------------------------
void vtkKWNIREPImageViewerWithScrollbar::SetFileName(std::string &str)
{
  m_sStr = str;
}
//----------------------------------------------------------------------------
void vtkKWNIREPImageViewerWithScrollbar::AxisXY()
{
  m_pImageViewer->SetSliceOrientationToXY();
  m_pSliceScale->SetRange(
    m_pImageViewer->GetSliceMin(), m_pImageViewer->GetSliceMax() );
  m_pSliceScale->SetValue( m_pImageViewer->GetSlice() );
}
//----------------------------------------------------------------------------
void vtkKWNIREPImageViewerWithScrollbar::AxisXZ()
{
m_pImageViewer->SetSliceOrientationToXZ();
m_pSliceScale->SetRange( m_pImageViewer->GetSliceMin(), 
  m_pImageViewer->GetSliceMax());
  m_pSliceScale->SetValue(
   m_pImageViewer->GetSlice());
}
//----------------------------------------------------------------------------
void vtkKWNIREPImageViewerWithScrollbar::AxisYZ()
{
m_pImageViewer->SetSliceOrientationToYZ();
  m_pSliceScale->SetRange(
m_pImageViewer->GetSliceMin(), m_pImageViewer->GetSliceMax() );
m_pSliceScale->SetValue( m_pImageViewer->GetSlice() );
}
//----------------------------------------------------------------------------
void vtkKWNIREPImageViewerWithScrollbar::CreateWidget()
{
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  // Add a render widget, attach it to the view frame, and pack
  if (!this->m_pRenderWidget)
  {
    this->m_pRenderWidget = vtkKWRenderWidget::New();
  }
  this->m_pRenderWidget->SetParent(this->GetParent());
  this->m_pRenderWidget->Create();
  this->m_pRenderWidget->CornerAnnotationVisibilityOn();

  this->Script("grid %s -column 1 -row 1 -sticky news -in %s",
   this->m_pRenderWidget->GetWidgetName(), this->GetWidgetName());

  this->Script("grid columnconfigure %s 1 -weight 1",
   this->GetWidgetName());

  this->Script("grid rowconfigure %s 1 -weight 1",
    this->GetWidgetName());

  //start

  m_pReader = ReaderType::New();
  m_pConnector = ConnectorFilterType::New();

  m_pReader->SetFileName( m_sStr.c_str() );
  m_pReader->Update();

  m_pConnector->SetInput( m_pReader->GetOutput() );

  m_pImageViewer = vtkImageViewer2::New();
      
  this->m_pImageViewer->SetRenderWindow(this->m_pRenderWidget->GetRenderWindow());
  this->m_pImageViewer->SetRenderer(this->m_pRenderWidget->GetRenderer());
  this->m_pImageViewer->SetInput(m_pConnector->GetOutput());
  this->m_pImageViewer->SetupInteractor( this->m_pRenderWidget->GetRenderWindow()->GetInteractor());

  // Reset the window/level and the camera

  this->m_pRenderWidget->ResetCamera();

   //The corner annotation has the ability to parse "tags"
  vtkCornerAnnotation *l_pCa = this->m_pRenderWidget->GetCornerAnnotation();
  l_pCa->SetImageActor(this->m_pImageViewer->GetImageActor());
  l_pCa->SetWindowLevel(this->m_pImageViewer->GetWindowLevel());
  l_pCa->SetText(1, "<slice>");
  l_pCa->SetText(3, "<window>\n<level>");

  // Create a scale to control the slice
  if (!this->m_pSliceScale)
  {
    this->m_pSliceScale = vtkKWScale::New();
  }
  this->m_pSliceScale->SetParent(this->GetParent());
  this->m_pSliceScale->Create();
  this->m_pSliceScale->SetRange(0, 255);//this->m_pImageViewer->GetSliceMin(), this->m_pImageViewer->GetSliceMax());
  this->m_pSliceScale->SetValueVisibility(false);
  this->m_pSliceScale->SetValue(128);//this->m_pImageViewer->GetSliceMax() - this->m_pImageViewer->GetSliceMin()/2);
  this->m_pImageViewer->SetSlice(128);
  this->m_pSliceScale->SetCommand(this->m_pImageViewer, "SetSlice");

  //TODO: FIX callback
  this->Script("grid %s -column 1 -row 2 -sticky news -in %s",
    this->m_pSliceScale->GetWidgetName(), this->GetWidgetName());

  //this->m_pSliceScale->SetCommand(this, "SliceScaleCallback");
}
//----------------------------------------------------------------------------
void vtkKWNIREPImageViewerWithScrollbar::SliceScaleCallback(double value)
{
  this->m_pImageViewer->SetSlice((int)value);
}
//----------------------------------------------------------------------------
void vtkKWNIREPImageViewerWithScrollbar::PrintSelf(ostream& os, vtkIndent indent)//Not Implemented
{
this->Superclass::PrintSelf(os,indent);
//os << indent << "[Name]: " << [Value] << endl;
}
//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWNIREPGridImageViewer );
vtkCxxRevisionMacro( vtkKWNIREPGridImageViewer, "$Revision: 0.9 $");
//----------------------------------------------------------------------------
vtkKWNIREPGridImageViewer::vtkKWNIREPGridImageViewer(void)
{
this->m_iRowSize = 0;
this->m_iColSize = 0;
this->m_pViewWithScrollbar = NULL;
}
//----------------------------------------------------------------------------
vtkKWNIREPGridImageViewer::~vtkKWNIREPGridImageViewer(void)
{
//Free viewer objects
for(int n = 0; n < this->m_iColSize * this->m_iRowSize; n++)
{
m_pViewWithScrollbar[n]->Delete();
}
}
//----------------------------------------------------------------------------
void vtkKWNIREPGridImageViewer::CreateWidget()
{
// Check if already created
if (this->IsCreated())
{
vtkErrorMacro(<< this->GetClassName() << " already created");
return;
}

// Call the superclass to create the whole widget
this->Superclass::CreateWidget();

int l_iSize = this->m_iRowSize * this->m_iColSize;

if(l_iSize >= 1)
{
//Allocate viewer list
m_pViewWithScrollbar = new vtkKWNIREPImageViewerWithScrollbar*[l_iSize];
  for(int fn = 0; fn < l_iSize; fn++)
    m_pViewWithScrollbar[fn] = vtkKWNIREPImageViewerWithScrollbar::New();
}
}
//----------------------------------------------------------------------------
void vtkKWNIREPGridImageViewer::Update()
{
//insert the column labels
for( int c = 1; c < this->m_iColSize + 1; c++ )
{
vtkKWLabel *l_kwLabel = vtkKWLabel::New();
l_kwLabel->SetParent(this->GetParent());
l_kwLabel->Create();
   //TODO: Skip invalid columnlabel entries
l_kwLabel->SetText(m_pcolumnlabels[c-1]);
l_kwLabel->SetJustificationToCenter();
l_kwLabel->SetFont("times 9");
l_kwLabel->SetWidth(1);
l_kwLabel->SetBackgroundColor(0.882, 0.882, 0.882);
l_kwLabel->SetBorderWidth(2);
l_kwLabel->SetReliefToGroove();
this->Script("grid %s -column %d -row %d -sticky news ",
l_kwLabel->GetWidgetName(),c,0);
this->Script("grid columnconfigure %s %d -weight 1", 
this->GetParent()->GetWidgetName(),c);
l_kwLabel->Delete();
}
//insert the row labels
for( int r = 1; r < this->m_iRowSize + 1; r++ )
{
vtkKWLabel *l_kwLabel = vtkKWLabel::New();
l_kwLabel->SetParent(this->GetParent());
l_kwLabel->Create();
  //TODO: Skip invalid rowlabels entries
l_kwLabel->SetText(m_prowlabels[r-1]);
l_kwLabel->SetJustificationToCenter();
l_kwLabel->SetWidth(2);
l_kwLabel->SetAnchorToCenter();
l_kwLabel->SetFont("times 7");
l_kwLabel->SetBackgroundColor(0.882, 0.882, 0.882);
l_kwLabel->SetBorderWidth(2);
l_kwLabel->SetReliefToGroove();
this->Script("grid %s -column %d -row %d -sticky news", 
   l_kwLabel->GetWidgetName(),0,r);
this->Script("grid rowconfigure %s %d -weight 1",
this->GetParent()->GetWidgetName(),r);
l_kwLabel->Delete();
}
//start creating the grid
for( int i = 0; i < this->m_iColSize; i++ )
{
for( int j = 0; j < this->m_iRowSize; j++ )
{
int n = i * m_iRowSize + j;
m_pViewWithScrollbar[n]->SetParent(this->GetParent());
m_pViewWithScrollbar[n]->Create();
this->Script("grid %s -column %d -row %d -sticky news",
m_pViewWithScrollbar[n]->GetWidgetName(),i + 1,j + 1);
this->Script("grid columnconfigure %s %d -weight 1",
this->GetParent()->GetWidgetName(),i + 1);
this->Script("grid rowconfigure %s %d -weight 1",
this->GetParent()->GetWidgetName(),j + 1);
}
}
}
//----------------------------------------------------------------------------
char * vtkKWNIREPGridImageViewer::ReformatRowLabel(char * str)
{
int i = 0;
while(str[++i]);
int size = i - 1;
char * tmp = new char[size * 2 + 1];
do {
tmp[ ((i-1)<<1) + 1] = '\n'; 
tmp[ (i-1)<<1 ] = str[ i-1 ]; 
}while( --i );
tmp[ size * 2 + 1 ] = 0;
return tmp;
}
//----------------------------------------------------------------------------
void vtkKWNIREPGridImageViewer::PrintSelf(ostream& os, vtkIndent indent)
{
this->Superclass::PrintSelf(os,indent);
os << indent << "RowSize: " << this->m_iRowSize << endl;
os << indent << "ColSize: " << this->m_iColSize << endl;
}
