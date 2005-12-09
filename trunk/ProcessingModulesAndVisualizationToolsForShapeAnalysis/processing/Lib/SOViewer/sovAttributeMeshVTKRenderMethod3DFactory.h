#ifndef SURFACE_ATTRIBUTE_MESH_VTK_RENDER_METHOD_3D_FACTORY
#define SURFACE_ATTRIBUTE_MESH_VTK_RENDER_METHOD_3D_FACTORY

#include "sovSurfaceMeshVTKRenderMethod3D.h"

#include <sovViewerFactoryBase.h>
#include <itkIndent.h>

namespace sov 
{
template < class MeshType = Mesh<int>, class TAttributeType = float >
class AttributeMeshVTKRenderMethod3DFactory : public ViewerFactoryBase
{
public:

  /* Typedefs */
  typedef AttributeMeshVTKRenderMethod3DFactory      Self;
  typedef ViewerFactoryBase                      Superclass;
  typedef itk::SmartPointer< Self >              Pointer;
  typedef itk::SmartPointer< const Self >        ConstPointer;
  
  itkFactorylessNewMacro( Self );
  
  /** Run-time type information (and related methods). */
  itkTypeMacro( AttributeMeshVTKRenderMethod3DFactory, ViewerFactoryBase );
  
  /** */
  virtual const char *  GetITKSourceVersion( void ) const; 

  /** */
  virtual const char *  GetDescription( void ) const;

  static void RegisterOneFactory(void)
  {
    itk::SmartPointer<AttributeMeshVTKRenderMethod3DFactory> renderMethodFactory;
    renderMethodFactory = AttributeMeshVTKRenderMethod3DFactory<MeshType>::New();
    ViewerFactoryBase::RegisterFactory(renderMethodFactory);
  }

protected:

  //virtual LightObject::Pointer CreateObject( const char * );

  /** Constructor */
  AttributeMeshVTKRenderMethod3DFactory( void );

  /** Destructor */
  ~AttributeMeshVTKRenderMethod3DFactory( void );

  /** Print out the object properties. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;

private:

  AttributeMeshVTKRenderMethod3DFactory( const Self & );  //purposely not implemented 
  void operator=( const Self & );         //purposely not implemented

};

}
//#ifndef ITK_MANUAL_INSTANTIATION
  #include "sovAttributeMeshVTKRenderMethod3DFactory.cxx"
//#endif


#endif

