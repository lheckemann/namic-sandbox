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
namespace sov 
{
  
  template < class TMesh = Mesh<int>, class TAttributeType = float >
    class AttributeMeshVTKRenderMethod3D : public SurfaceMeshVTKRenderMethod3D <TMesh>
  {
public:
  typedef AttributeMeshVTKRenderMethod3D Self;
  typedef SurfaceMeshVTKRenderMethod3D Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  typedef typename TMesh::PixelType PixelType;
  typedef AttributeMeshSpatialObject<TMesh, TAttributeType> AttributeMeshSOType ;
  
  itkNewMacro( AttributeMeshVTKRenderMethod3D );
  itkTypeMacro( AttributeMeshVTKRenderMethod3D, SurfaceMeshVTKRenderMethod3D );
  

  AttributeMeshVTKRenderMethod3D();
  virtual ~AttributeMeshVTKRenderMethod3D();
protected:
  void Update () ;
  };
}

#include "sovAttributeMeshVTKRenderMethod3D.txx"
#endif // !defined(AFX_SOVSURFACEATTRIBUTEMESHVTKRENDERMETHOD3D_H__E7042275_725A_442D_B0F2_FEA2B61E7AA3__INCLUDED_)
