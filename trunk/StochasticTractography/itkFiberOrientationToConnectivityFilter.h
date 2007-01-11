#include "itkFiberOrientationToConnectivityFilter.h"

namespace itk{

template< class TFiberOrientationPosteriorImage, class TConnectivityImage >
class ITK_EXPORT TFiberOrientationToConnectivityFilter :
  public ImageToImageFilter< TFiberOrientationImage,
                    TConnectivityImage >{
public:
  typedef FiberOrientationPosteriorToConnectivityFilter Self;
  typedef 
