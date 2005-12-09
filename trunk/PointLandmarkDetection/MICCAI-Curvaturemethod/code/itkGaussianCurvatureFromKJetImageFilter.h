/*=========================================================================

author: Bryn Lloyd, Computational Radiology Laborotory (CRL), Brigham and Womans
  date: 06/30/2005

=========================================================================*/
#ifndef __itkGaussianCurvatureFromKJetImageFilter_h
#define __itkGaussianCurvatureFromKJetImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkVector.h"
//#include "vnl/vnl_math.h"

//#if defined(_MSC_VER) && (_MSC_VER >= 1300)
//#pragma function(exp)
//#endif

namespace itk
{

/** \class DetOverTraceNTensorToImageFilter
 * \brief Computes the det/trace^N from the symmetric second rank tensor
 *  pixel-wise, where N is the image Dimension.
 *
 * 
 * \ingroup IntensityImageFilters  Multithreaded
 *
 */
 
/** The input Vector follows the following convention:
 *
 * The convention used for the 3D 2Jet is the following order:
 * Ix, Iy, Iz, Ixx, Ixy, Ixz, Iyy, Iyz, Izz, i.e there are 9 elements, 
 * and the original image values I are not included.
 *
 * The convention used for the 2D 2Jet is the following order:
 * Ix, Iy, Ixx, Ixy, Iyy, Iyz, i.e there are 9 elements, 
 * and the original image values I are not included.
 *
 * A method based on (nearly) the 2D Gaussian curvature was proposed by Beaudet( det(Hessian) ), something similir by Blum
 * The Gaussian Curvature for 3D images was proposed by: Monga, Thirion, Florack
 * 
  */

namespace Function {  
template< class TValueType>
class Det
{
public:
  Det() {};
  ~Det() {};
  //The 2D function
  inline TValueType operator()( const Vector<TValueType,5> & A )
  {
    TValueType  GaussianCurvatureFromKJet = A[2]*A[4]-A[3]*A[3];
    TValueType SquaredGradientMagnitude = A[0]*A[0] + A[1]*A[1] +eps;
    return -GaussianCurvatureFromKJet/SquaredGradientMagnitude;
  }

  // The 3D function
  inline TValueType operator()( const Vector<TValueType,9> & A )
  {
  /**  should be K = [  Ix^2(IyyIzz - Iyz^2) +2IxIxz(IyIyz - IzIyy)  
   *                            +  Iy^2(IzzIxx - Ixz^2) + 2IyIxy(IzIxz - IxIzz)  
   *                            +  Iz^2(IxxIyy - Ixy^2) + 2IzIyz(IxIxy - IyIxx)  ]  / (Ix^2 + Iy^2 + Iz^2)
   * 
   *   where Ix =A[0], Iy = A[1], Iz = A[2],  Ixx = A[3], Ixy  = A[4], Ixz  = A[5], Iyy  = A[6], Iyz  = A[7], Izz  = A[8]   
   */
    TValueType  GaussianCurvatureFromKJet =   A[0]*A[0]*(A[6]*A[8]-A[7]*A[7]) + 2*A[0]*A[5]*(A[1]*A[7]-A[2]*A[6])  
                                           +  A[1]*A[1]*(A[8]*A[3]-A[5]*A[5]) + 2*A[1]*A[4]*(A[2]*A[5]-A[0]*A[8])
                                           +  A[2]*A[2]*(A[3]*A[6]-A[4]*A[4]) + 2*A[2]*A[7]*(A[0]*A[4]-A[1]*A[3]);
    
    TValueType SquaredGradientMagnitude = A[0]*A[0] + A[1]*A[1] + A[2]*A[2] +eps;
    return GaussianCurvatureFromKJet/SquaredGradientMagnitude;
  }
private:
   const static TValueType eps = 0.0000001;
};

}
template <class TInputImage, class TOutputImage>
class ITK_EXPORT GaussianCurvatureFromKJetImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage,
                        Function::Det<typename TInputImage::PixelType::ValueType>   >
{
public:
  /** Standard class typedefs. */
  typedef GaussianCurvatureFromKJetImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage,
                                  Function::Det< typename TInputImage::PixelType::ValueType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
protected:
  GaussianCurvatureFromKJetImageFilter() {}
  virtual ~GaussianCurvatureFromKJetImageFilter() {}

private:
  GaussianCurvatureFromKJetImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end namespace itk


#endif
