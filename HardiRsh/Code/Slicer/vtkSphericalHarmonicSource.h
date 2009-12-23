
#ifndef __vtkSphericalHarmonicSource_h
#define __vtkSphericalHarmonicSource_h

#include "itkRealSymSphericalHarmonicBasis.h"

#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"

#include "vnl/vnl_matrix.h"

class vtkMatrix4x4;

class VTK_GRAPHICS_EXPORT vtkSphericalHarmonicSource : public vtkPolyDataAlgorithm
{
public:

  vtkTypeRevisionMacro(vtkSphericalHarmonicSource,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  //default constructor!
  static vtkSphericalHarmonicSource *New();

  void SetSphericalHarmonics(double *coeff);
  void SetSphericalHarmonics(float *coeff);

  double* GetSphericalHarmonics () { return SphericalHarmonics; }

  void SetSphericalHarmonicComponent( int i, double v );
  void SetSphericalHarmonicComponent( size_t i, double v[] );
  double GetSphericalHarmonicComponent( int i );

  void SetOrder(int);
  vtkGetMacro(Order,int);

  vtkGetMacro(NumberOfSphericalHarmonics,int);

  //set a Scale/zoom Factor // is one
  vtkSetMacro(ScaleFactor,float);
  vtkGetMacro(ScaleFactor,float);

  // Description:
  // Set the center of the sphere. Default is 0,0,0.
  vtkSetVector3Macro(Center,double);
  vtkGetVectorMacro(Center,double,3);

  // Description:
  // Set the number of points in the longitude direction (ranging from
  // StartTheta to EndTheta).
  vtkSetClampMacro(ThetaResolution,int,3,VTK_MAX_SPHERE_RESOLUTION);
  vtkGetMacro(ThetaResolution,int);

  // Description:
  // Set the number of points in the latitude direction (ranging
  // from StartPhi to EndPhi).
  vtkSetClampMacro(PhiResolution,int,3,VTK_MAX_SPHERE_RESOLUTION);
  vtkGetMacro(PhiResolution,int);

  /**THESE ARE DUMBIES SINCE I DON"T THINK I'LL NEED Them?*/
  vtkSetMacro(Tesselation,int);
  vtkGetMacro(Tesselation,int);

  virtual void SetRotationMatrix( vtkMatrix4x4* );
  vtkGetObjectMacro( RotationMatrix, vtkMatrix4x4 );

protected:
  vtkSphericalHarmonicSource(int inpOrder=4);
  ~vtkSphericalHarmonicSource() {std::cerr << "vtkSphericalHarmonicSource::Delete"<<std::endl;}

  /*
  * Order of the spherical harmonic basis
  */
  int Order;
  /*
  * Spherical harmonic representation of the spherical function
  */
  double *SphericalHarmonics;
  unsigned int NumberOfSphericalHarmonics;
  vtkMatrix4x4 *RotationMatrix;
  float ScaleFactor;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  void computePoint(double , double , double *, double );
  double Center[3];

  //Currently Not Used!
  int ThetaResolution;
  int PhiResolution;

  //not used!
  int Tesselation;

private:
  vtkSphericalHarmonicSource(const vtkSphericalHarmonicSource&);  // Not implemented.
  void operator=(const vtkSphericalHarmonicSource&);  // Not implemented.

  void ComputeRshMatrix();
  double EvaluateBasis(int, double, double);

  //Internal Source!
  vtkSmartPointer<vtkSphereSource>  sphereSource;
  vnl_matrix<double>                rshBasisMatrix;
  vnl_vector<double>                phiVector;
  vnl_vector<double>                thetaVector;

};
#endif  /* VTKSPHERICALHARMONICSOURCE_HPP */
