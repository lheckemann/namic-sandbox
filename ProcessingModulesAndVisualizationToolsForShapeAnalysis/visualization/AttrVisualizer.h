// AttrVisualizer.h: interface for the AttrVisualizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTRVISUALIZER_H__9EF037EF_02EC_4470_B4F0_0F8B9B320B71__INCLUDED_)
#define AFX_ATTRVISUALIZER_H__9EF037EF_02EC_4470_B4F0_0F8B9B320B71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Fl/Fl_Box.h>
#include <FL/Fl_File_Browser.H>
#include <FL/Fl_File_Chooser.H>
#include <sovFlVTKDisplay.h>
#include <itkDefaultDynamicMeshTraits.h>
#include "AttributeMeshSpatialObject.h"
#include <itkSceneSpatialObject.h>
#include <itkMetaMeshConverter.h>

#include "vtkActor.h"
#include <sovVTKRenderer3D.h>
#include "sovAttributeMeshVTKRenderMethod3D.h"

using namespace itk ;

class AttrVisualizer : public Fl_Box
{
public:
  void Save(const char *filename);
  void LoadMeshAttr(const char *filename);
  void Radio(int active);
  void Click(const char *filename);
  void ChangeColor();
  void LoadAttr(const char *filename);
  void LoadMesh(const char *filename);
  void SetDisplay(sov::FlVTKDisplay *sovDisplay);
  AttrVisualizer(int x, int y, int w, int h);
  virtual ~AttrVisualizer();
private:
  typedef float real ;
  typedef DefaultDynamicMeshTraits < real, 3, 3 > MeshTraitsType ; 
  typedef Mesh < real, 3, MeshTraitsType > MeshType ;
  typedef MeshSpatialObject < MeshType > MeshSOType ;
  typedef AttributeMeshSpatialObject < MeshType, float > AttributeMeshSOType ;
  typedef MetaMeshConverter < 3, real, MeshTraitsType > MeshConverterType ;

  typedef SceneSpatialObject <> SceneType ;

  typedef sov::VTKRenderer3D RendererType ;
  typedef sov::AttributeMeshVTKRenderMethod3D < MeshType > SurfaceRenderMethodType ;  

  SceneType::Pointer m_Scene ;
  AttributeMeshSOType::Pointer m_AttributeMeshObject ;  

  RendererType::Pointer m_Renderer3D ;
  SurfaceRenderMethodType::Pointer m_SurfaceRenderMethod3D ;

  sov::FlVTKDisplay *m_sovDisplay ;

  int m_mode ;
};

#endif // !defined(AFX_ATTRVISUALIZER_H__9EF037EF_02EC_4470_B4F0_0F8B9B320B71__INCLUDED_)

