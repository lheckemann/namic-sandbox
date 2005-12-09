// AttributeMeshSpatialObject.h: interface for the AttributeMeshSpatialObject class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ATTRIBUTEMESHSPATIALOBJECT_H__5279771E_1299_4A29_B412_EB7391D7FCF1__INCLUDED_)
#define AFX_ATTRIBUTEMESHSPATIALOBJECT_H__5279771E_1299_4A29_B412_EB7391D7FCF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <itkMeshSpatialObject.h>
#include <vector>
#include <fstream>
using namespace itk ;


template < class TMesh = Mesh<int>, class TAttributeType = float >
class AttributeMeshSpatialObject : public MeshSpatialObject<TMesh>
{
public:
  /** Run-time type information (and related methods). */
  itkTypeMacro( AttributeMeshSpatialObject, MeshSpatialObject );
  typedef AttributeMeshSpatialObject< TMesh, TAttributeType> Self;
  
  typedef MeshSpatialObject< TMesh >                 Superclass;
  typedef SmartPointer< Self >                       Pointer;
  typedef SmartPointer< const Self >                 ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );
  
  void MakeAttributeMesh () ;
  void ReadAttributes ( std::string attrFilename ) ;

  TAttributeType GetAttribute (int vertIndex) ;
  AttributeMeshSpatialObject();
  virtual ~AttributeMeshSpatialObject();

  void writeToFile(const char *filename) ;
  void loadFromFile(const char *filename) ;

private:
  typedef std::vector < TAttributeType > TAttributeListType ;
  
  TAttributeListType m_Attributes ;

  int m_AttributeDimension ;

};

#include "AttributeMeshSpatialObject.txx"

#endif // !defined(AFX_ATTRIBUTEMESHSPATIALOBJECT_H__5279771E_1299_4A29_B412_EB7391D7FCF1__INCLUDED_)
