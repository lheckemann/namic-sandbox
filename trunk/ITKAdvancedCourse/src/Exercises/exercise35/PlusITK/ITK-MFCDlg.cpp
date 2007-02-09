// ITK-MFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ITK-MFC.h"
#include "ITK-MFCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
  enum { IDD = IDD_ABOUTBOX };

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CITKMFCDlg dialog



CITKMFCDlg::CITKMFCDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CITKMFCDlg::IDD, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  
  m_Reader = ReaderType::New();
  m_Writer = WriterType::New();
  m_Filter = FilterType::New();

  m_Filter->SetInput( m_Reader->GetOutput() );
  m_Writer->SetInput( m_Filter->GetOutput() );

  m_Filter->SetTimeStep( 0.1 ); // for 2D images
  m_Filter->SetNumberOfIterations( 5 );
  
  m_NumberOfIterationsSlider.SetRange( 1, 50 );
  m_NumberOfIterationsSlider.SetPos( 5 );
  m_NumberOfIterationsSlider.SetTic(true);
  m_NumberOfIterationsSlider.SetTicFreq(5);
}

void CITKMFCDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SLIDER1, m_NumberOfIterationsSlider);
}

BEGIN_MESSAGE_MAP(CITKMFCDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()

  ON_BN_CLICKED(IDC_BUTTON1, LoadInputImage)
  ON_BN_CLICKED(IDC_BUTTON3, RunImageFilter)
  ON_BN_CLICKED(IDC_BUTTON4, SaveOutputImage)

  ON_WM_HSCROLL(IDC_SLIDER1, ChangeNumberOfIterations)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CITKMFCDlg message handlers

BOOL CITKMFCDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty())
    {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);      // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  // TODO: Add extra initialization here
  
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CITKMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else
  {
    CDialog::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CITKMFCDlg::OnPaint() 
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CITKMFCDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void CITKMFCDlg::LoadInputImage()
{
  CString strFilter = "*.png";

  CFileDialog fileDialog( TRUE, NULL, NULL, OFN_FILEMUSTEXIST, strFilter );
  HRESULT hResult = (int)fileDialog.DoModal();
  if( FAILED( hResult ) ) 
    {
    return;
    }

  m_Reader->SetFileName( fileDialog.GetFileName() );

  try
    {
    m_Reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    MessageBox( _T( excp.GetDescription() ), 0, 0 );
    }
}

void CITKMFCDlg::RunImageFilter()
{
  try
    {
    m_Filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    MessageBox( _T( excp.GetDescription() ), 0, 0 );
    }
}

void CITKMFCDlg::SaveOutputImage()
{
  CString strFilter = "*.png";

  CFileDialog fileDialog( FALSE, NULL, NULL, OFN_PATHMUSTEXIST, strFilter );
  HRESULT hResult = (int)fileDialog.DoModal();
  if( FAILED( hResult ) ) 
    {
    return;
    }

  m_Writer->SetFileName( fileDialog.GetFileName() );

  try
    {
    m_Writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    MessageBox( _T( excp.GetDescription() ), 0, 0 );
    }
}


void CITKMFCDlg::ChangeNumberOfIterations( 
  UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  std::cout << "number of iterations = " << nPos << std::endl;
  if( nPos != m_NumberOfIterationsSlider.GetPos() )
    {
    MessageBox( _T("Wrong Number of Iterations"), 0, 0 );
    }

  m_Filter->SetNumberOfIterations( nPos );
  this->SaveOutputImage();
}
