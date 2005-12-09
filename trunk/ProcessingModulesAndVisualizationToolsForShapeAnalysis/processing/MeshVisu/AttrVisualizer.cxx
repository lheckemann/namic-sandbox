// AttrVisualizer.cxx: implementation of the AttrVisualizer class.
//
//////////////////////////////////////////////////////////////////////
 
#include "AttrVisualizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AttrVisualizer::AttrVisualizer(int x, int y, int w, int h) : Fl_Box (x, y, w, h) 
{
  m_AttributeMeshObject = 0 ;
  m_FileMode = 1 ;
  m_ColorMode = 0 ;

  pt = 0.05 ;
  ph0 = 0 ;
  ph1 = 0.25 ;
  ph2 = 0.67 ;
}

AttrVisualizer::~AttrVisualizer()
{

}

void AttrVisualizer::LoadMesh(const char *filename)
{
  MeshConverterType * converter = new MeshConverterType () ;
  MeshSOType::Pointer mesh ;
  
  mesh = converter->ReadMeta (filename) ;

  m_AttributeMeshObject = AttributeMeshSOType::New () ;
  m_AttributeMeshObject->SetMesh ( mesh->GetMesh() ) ;
  m_AttributeMeshObject->MakeAttributeMesh () ;

  m_Scene = SceneType::New () ;
  m_Scene->AddSpatialObject ( m_AttributeMeshObject ) ;
 
  m_Renderer3D = RendererType::New();
  m_SurfaceRenderMethod3D = SurfaceRenderMethodType::New () ;
  
  m_Renderer3D->AddRenderMethod ( m_SurfaceRenderMethod3D ) ;
  m_Renderer3D->SetScene ( m_Scene ) ;
  
  m_sovDisplay->AddRenderer ( m_Renderer3D ) ;
  m_sovDisplay->Update () ;

}

void AttrVisualizer::LoadAttr(const char *filename)
{
  if ( m_AttributeMeshObject )
  {
    m_AttributeMeshObject->ReadAttributes (filename) ;
    m_Renderer3D->GetVisualObject(m_AttributeMeshObject)->Modified () ;
    m_sovDisplay->Update () ;
  }
}

void AttrVisualizer::ChangeColor()
{

}

void AttrVisualizer::SetDisplay(sov::FlVTKDisplay *sovDisplay)
{
  m_sovDisplay = sovDisplay ;
}


void AttrVisualizer::Click(const char *filename)
{
  switch ( m_FileMode )
  {
  case 1: LoadMesh ( filename ) ;
          break ;
  case 2: LoadAttr ( filename ) ;
          break;
  case 3: LoadMeshAttr ( filename ) ;
          break ;
  default: Save ( filename ) ;
  };
}

void AttrVisualizer::Radio(int active)
{
  m_FileMode = active ;
}

void AttrVisualizer::LoadMeshAttr(const char *filename)
{  
  m_AttributeMeshObject = AttributeMeshSOType::New () ;
  m_AttributeMeshObject->loadFromFile ( filename ) ;
 
  m_Scene = SceneType::New () ;
  m_Scene->AddSpatialObject ( m_AttributeMeshObject ) ;
 
  m_Renderer3D = RendererType::New();
  m_SurfaceRenderMethod3D = SurfaceRenderMethodType::New () ;
  
  m_Renderer3D->AddRenderMethod ( m_SurfaceRenderMethod3D ) ;
  m_Renderer3D->SetScene ( m_Scene ) ;
  
  m_sovDisplay->AddRenderer ( m_Renderer3D ) ;
  m_sovDisplay->Update () ;
}

void AttrVisualizer::Save(const char *filename)
{
  if ( m_AttributeMeshObject )
  {
    m_AttributeMeshObject->writeToFile ( filename ) ;
  }
}

void AttrVisualizer::ColorScheme (int colorMode)
{
  m_ColorMode = colorMode ;

  if (!m_SurfaceRenderMethod3D)
    return ;
  
  if ( colorMode == 0 )
    m_SurfaceRenderMethod3D->ChangeColorMode (m_SurfaceRenderMethod3D->P_VALUE) ;
  else
    m_SurfaceRenderMethod3D->ChangeColorMode (m_SurfaceRenderMethod3D->DISTANCE_MAP) ;

  m_Renderer3D->GetVisualObject(m_AttributeMeshObject)->Modified () ;
  m_sovDisplay->Update () ;
}

void AttrVisualizer::SetPValue (float t, float hue0, float hue1, float hue2)
{
  if ( m_SurfaceRenderMethod3D )
  {
    m_SurfaceRenderMethod3D->ChangeColorMode (m_SurfaceRenderMethod3D->P_VALUE) ;
    m_SurfaceRenderMethod3D->ChangePValueSettings ( t, hue0, hue1, hue2 ) ;

    
    m_Renderer3D->GetVisualObject(m_AttributeMeshObject)->Modified () ;
    m_sovDisplay->Update () ;
  }
  pt = t ;
  ph0 = hue0 ;
  ph1 = hue1 ;
  ph2 = hue2 ;
}

void AttrVisualizer::GetPValue (float &t, float &hue0, float &hue1, float &hue2)
{
  t = pt ;
  hue0 = ph0 ;
  hue1 = ph1 ;
  hue2 = ph2 ;
}
