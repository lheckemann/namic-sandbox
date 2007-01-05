

#ifndef __itkPointSetIO_h
#define __itkPointSetIO_h

#include "stdio.h"
#include "itkProcessObject.h"
#include "itkMeshSource.h"

#include "itkPointSet.h"
#include "itkDefaultStaticMeshTraits.h"

// this class doesn't work with double values, infact it might only work with float values (points and data)


namespace itk
{

template <class TPointSetType>
class ITK_EXPORT PointSetIO : public MeshSource<TPointSetType>
{

public:
  typedef PointSetIO                            Self;
  typedef MeshSource<TPointSetType>             Superclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(PointSetIO,MeshSource);


  typedef TPointSetType    PointSetType;
//  typedef TInputPointSet       PointSetType;

  typedef typename PointSetType::PixelType        PixelType;
  typedef typename PointSetType::PointType        PointType;
  typedef typename PointSetType::Pointer          PointSetPointer;
  typedef PointSetType *                 PointSetCPointer;
  typedef typename PointSetType::ConstPointer     PointSetConstPointer;
  typedef typename PointSetType::PointsContainer  PointsContainer;
  typedef typename PointSetType::PointIdentifier  PointIdentifier;


  typedef typename PointsContainer::Pointer         PointsContainerPointer;
  typedef typename PointsContainer::Iterator        PointsContainerIterator;

  typedef typename PointSetType::PointDataContainer PointDataContainer;
  typedef typename PointDataContainer::Pointer      PointDataContainerPointer;
  typedef typename PointDataContainer::Iterator     PointDataContainerIterator;





  PointSetIO();

  ~PointSetIO();

  void WritePointSet( PointSetType *  mesh, const char * filename );

  void WritePointSetMatlab( PointSetType *  mesh, const char * filename );

  PointSetPointer  ReadPointSet( const char * filename );


private:
  PointSetIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented



//  std::string DEFORMED_MESH_SUFFIX;
  std::string DELIMS;


};

}


#include "PointSetIO.txx"

#endif
