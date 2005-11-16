/** This is a draft prototype of the DataModelManager to be used as the core element of Slicer 3.0 */ 

#include <string> 
#include "itkTransform.h" 
#include "itkImageBase.h" 

namespace Slicer
{

class DataModelManager
{
public:

    
  /** Type for naming the nodes */
  typedef std::string   NodeNameType;

  /** Base Type for Transform hierarchy */
  typedef itk::Transform   NodeNameType;

  /** Base Type for Image hierarchy */
  typedef itk::ImageBase<3>   NodeNameType;

  /** Request to add a name with a node */
  void RequestAddNode( const NodeNameType & nodeName );

  /** Request attaching a data type with a name to a named node */
  void RequestAddTransformToNode( const NodeNameType & nodeName,
                                  const ElementNameType & name,
                                  const TransformType * transform );

  /** Request attaching a dataset with a name to a named node */
  void RequestAddTransformToNode( const NodeNameType & nodeName,
                                  const ElementNameType & name,
                                  const ImageType * transform );


  /** Request Transform between named nodes */
  void RequestGetTransformBetweenNodes( const NodeNameType & nodeName,
                                        const NodeNameType & nodeName )

private:

  /** Send Transform between two nodes in the payload of an Event */
  void SendTransformBetweenNodes() const;



};



}



