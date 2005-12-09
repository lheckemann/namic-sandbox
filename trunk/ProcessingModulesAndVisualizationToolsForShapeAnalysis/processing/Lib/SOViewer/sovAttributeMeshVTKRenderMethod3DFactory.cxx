#ifndef LAYLAYLAY
#define LAYLAYLAY

#include <itkVersion.h>

#include <cstring>

#include "sovAttributeMeshVTKRenderMethod3D.h"
#include "sovAttributeMeshVTKRenderMethod3DFactory.h"

template <class TMeshType, class TAttributeType>
void sov::AttributeMeshVTKRenderMethod3DFactory<TMeshType, TAttributeType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent <<"Current autoload path: ";
  const char * path = getenv("ITK_AUTOLOAD_PATH");
  if( path )
  {
    os<<path<<std::endl;
  }
  else
  {
    os<<"NULL"<<std::endl;
  }
}

template < class TMeshType, class TAttributeType>
sov::AttributeMeshVTKRenderMethod3DFactory<TMeshType, TAttributeType>
::AttributeMeshVTKRenderMethod3DFactory()
{
  typedef AttributeMeshVTKRenderMethod3D<TMeshType, TAttributeType> AttributeMeshVTKRenderMethod3DType;
  this->RegisterOverride("AttributeMeshSpatialObject",
    "AttributeMeshVTKRenderMethod3D",
    "AttributeMeshSpatialObject Point Render Method",
    1,
    CreateObjectFunction<AttributeMeshVTKRenderMethod3DType>::New());
  this->RegisterOverride("MeshSpatialObject",
    "AttributeMeshVTKRenderMethod3D",
    "AttributeMeshSpatialObject Point Render Method",
    1,
    CreateObjectFunction<AttributeMeshVTKRenderMethod3DType>::New());
}

template < class TMeshType, class TAttributeType>
sov::AttributeMeshVTKRenderMethod3DFactory<TMeshType, TAttributeType>
::~AttributeMeshVTKRenderMethod3DFactory()
{
}

template < class TMeshType, class TAttributeType>
const char *
sov::AttributeMeshVTKRenderMethod3DFactory<TMeshType, TAttributeType>
::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}

template < class TMeshType, class TAttributeType>
const char *
sov::AttributeMeshVTKRenderMethod3DFactory<TMeshType, TAttributeType>
::GetDescription() const
{
  return "Create AttributeMeshVTKRenderMethod3D for the visualization of AttributeMeshSpatialObject classes";
}

#endif

