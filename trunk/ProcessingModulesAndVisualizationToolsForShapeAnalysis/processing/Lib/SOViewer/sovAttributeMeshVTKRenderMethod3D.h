// sovSurfaceAttributeMeshVTKRenderMethod3D.h: interface for the sovSurfaceAttributeMeshVTKRenderMethod3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOVSURFACEATTRIBUTEMESHVTKRENDERMETHOD3D_H__E7042275_725A_442D_B0F2_FEA2B61E7AA3__INCLUDED_)
#define AFX_SOVSURFACEATTRIBUTEMESHVTKRENDERMETHOD3D_H__E7042275_725A_442D_B0F2_FEA2B61E7AA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sovSurfaceMeshVTKRenderMethod3D.h>
#include <vtkLoopSubdivisionFilter.h>
#include "AttributeMeshSpatialObject.h"
#include "ColorScheme.h"
#include <vtkFloatArray.h>
#include <vtkPointData.h>

namespace sov 
{
  
  template < class TMesh = Mesh<int>, class TAttributeType = float >
    class AttributeMeshVTKRenderMethod3D : public SurfaceMeshVTKRenderMethod3D <TMesh>
  {
public:
  typedef AttributeMeshVTKRenderMethod3D Self;
  typedef SurfaceMeshVTKRenderMethod3D<TMesh> Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  typedef typename TMesh::PixelType PixelType;
  typedef AttributeMeshSpatialObject<TMesh, TAttributeType> AttributeMeshSOType ;

  typedef TMesh                                MeshType;
  typedef typename Superclass::SurfaceMeshType SurfaceMeshType;
  typedef typename Superclass::ObjectType      ObjectType;
  typedef typename Superclass::ObjectListType  ObjectListType;
 
  
  itkNewMacro( AttributeMeshVTKRenderMethod3D );
  itkTypeMacro( AttributeMeshVTKRenderMethod3D, SurfaceMeshVTKRenderMethod3D );
  
  enum colorMode {P_VALUE, DISTANCE_MAP} ;

  AttributeMeshVTKRenderMethod3D();
  virtual ~AttributeMeshVTKRenderMethod3D();

  void ChangeColorMode (enum colorMode newMode) ;
  void ChangePValueSettings ( float t, float h0, float h1, float h2 ) ;
protected:
  void Update () ;
private:
  enum colorMode m_ColorSchemeType ;
  class ColorScheme *m_ColorScheme ;
  };
}

#include "sovAttributeMeshVTKRenderMethod3D.txx"
#endif // !defined(AFX_SOVSURFACEATTRIBUTEMESHVTKRENDERMETHOD3D_H__E7042275_725A_442D_B0F2_FEA2B61E7AA3__INCLUDED_)
