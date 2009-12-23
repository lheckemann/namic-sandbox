
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMatrix4x4.h"

#include <vector>
#include <complex>
#include <math.h>

#include "vtkSphericalHarmonicSource.h"

#include <vnl/vnl_matrix_fixed.txx>
VNL_MATRIX_FIXED_INSTANTIATE(double,15,15);
VNL_MATRIX_FIXED_INSTANTIATE(double,28,28);
VNL_MATRIX_FIXED_INSTANTIATE(double,45,45);
VNL_MATRIX_FIXED_INSTANTIATE(double,66,66);

vtkCxxRevisionMacro(vtkSphericalHarmonicSource, "$Revision: 0 $");
vtkStandardNewMacro(vtkSphericalHarmonicSource);

vtkSphericalHarmonicSource::vtkSphericalHarmonicSource(int inpOrder)
{

  this->Center[0] = 0.0;
  this->Center[1] = 0.0;
  this->Center[2] = 0.0;
  this->RotationMatrix = NULL;
  this->SetNumberOfInputPorts(0);

  NumberOfSphericalHarmonics = (inpOrder+1)*(inpOrder+2)/2;
  SphericalHarmonics = new double[NumberOfSphericalHarmonics];
  SphericalHarmonics[0] = 1.0;
  for(unsigned int i = 0; i < NumberOfSphericalHarmonics; i++)
    SphericalHarmonics[i] = 0.0;

  //~ Order = (int)(-3/2 + std::sqrt((float)(9/4 - 2*(1 - 15))));
  Order = inpOrder;
  ScaleFactor = 1;

  sphereSource = vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->SetRadius(1.0);
  sphereSource->SetThetaResolution(20);
  sphereSource->SetPhiResolution(10);
  sphereSource->Update();
  this->ComputeRshMatrix();

}


void vtkSphericalHarmonicSource::SetSphericalHarmonicComponent( int i, double v )
{
  if (SphericalHarmonics[i]!=v)
  {
    vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Spherical Harmonics coefficient "<< i <<" to "<< v);
    this->Modified();
    SphericalHarmonics[i] = v;
  }
}

//It seems bad to reassign the internal pointer.
void vtkSphericalHarmonicSource::SetSphericalHarmonics(float *_arg)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Spherical Harmonics to " << _arg);
  for (unsigned int i = 0; i < NumberOfSphericalHarmonics; i++)
  {
    this->SetSphericalHarmonicComponent(i,static_cast<double>(_arg[i]));
  }
}

//this seems dangerous... other methods can free/change contents of _arg
//with out forcing a redraw?
void vtkSphericalHarmonicSource::SetSphericalHarmonics(double *_arg) {
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Spherical Harmonics to " << _arg);
  this->SphericalHarmonics = _arg;
  this->Modified();
}

double vtkSphericalHarmonicSource::GetSphericalHarmonicComponent( int i ) {
  return SphericalHarmonics[i];
}

//Would rather use a constructor for this but am unable to call my constructer defined above?
void vtkSphericalHarmonicSource::SetOrder(int inpOrder)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Order to " << inpOrder);

  //Make new arrays
  unsigned int newNumberOfSphericalHarmonics = (inpOrder+1)*(inpOrder+2)/2;
  double *newSphericalHarmonics = new double[newNumberOfSphericalHarmonics];

  //Copy the old coeffs over to the new array
  for(unsigned int i = 0; i < this->NumberOfSphericalHarmonics; i++)
     newSphericalHarmonics[i] = SphericalHarmonics[i];

  //reset stuff
  this->Order = inpOrder;
  this->NumberOfSphericalHarmonics = newNumberOfSphericalHarmonics;
  this->SphericalHarmonics = newSphericalHarmonics;
  this->Modified();

}
vtkCxxSetObjectMacro( vtkSphericalHarmonicSource, RotationMatrix, vtkMatrix4x4 );

//----------------------------------------------------------------------------
void vtkSphericalHarmonicSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "SH Basis Order: " << Order << "\n";
  os << indent << "Length of SH coefficient vector: " << NumberOfSphericalHarmonics << "\n";
  os << indent << "SH Coefficients:\n[";
  for(unsigned int i = 0; i <  NumberOfSphericalHarmonics; i++) {
    os << indent << SphericalHarmonics[i] << " ";
  }
  os << indent << "]\n";
}

//----------------------------------------------------------------------------
//----       updated to use the RSH class from itk ---------------------------
//----------------------------------------------------------------------------
int vtkSphericalHarmonicSource::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
 // get the info object
  vtkSmartPointer<vtkInformation> outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkSmartPointer<vtkPolyData> output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Get the Source points
  vtkSmartPointer<vtkPolyData> source  = sphereSource->GetOutput();
  vtkSmartPointer<vtkPoints> sourcePts = source->GetPoints( );
  vtkIdType numPts = sourcePts->GetNumberOfPoints( );

  vtkSmartPointer<vtkPoints> newPts = vtkSmartPointer<vtkPoints>::New();
  newPts->Allocate(numPts);

  vtkSmartPointer<vtkFloatArray> newValues = vtkSmartPointer<vtkFloatArray>::New();
  newValues->SetNumberOfTuples(numPts);
  newValues->SetNumberOfComponents(1);

  //Make sure the basis is the right size...
  if (rshBasisMatrix.rows() != (unsigned int)numPts ||
    rshBasisMatrix.cols() != this->NumberOfSphericalHarmonics )
  {
    this->ComputeRshMatrix();
  }
  double pt[3];
  vnl_vector< double > coeffs(SphericalHarmonics,NumberOfSphericalHarmonics);
  vnl_vector< double > values = rshBasisMatrix * coeffs;
  double radius;
  double phi,theta;

  for (vtkIdType i=0;i<numPts; i++)
  {
    phi     = this->phiVector[i];
    theta   = this->thetaVector[i];
    radius  = this->ScaleFactor * values[i];
    computePoint(theta,phi,pt,radius);
    sourcePts->SetPoint(i,pt);
//    newValues->InsertNextTuple(&radius);
  }
  std::cerr << std::endl;
  std::cerr << std::endl;

  output->SetPoints(sourcePts);
  output->SetPolys(source->GetPolys());
  newValues->Squeeze();
  output->GetPointData()->SetScalars(newValues);

  return 1;

}

void vtkSphericalHarmonicSource::ComputeRshMatrix()
{

  // Get the Source points
  vtkPolyData *source  = sphereSource->GetOutput();
  vtkPoints *sourcePts = source->GetPoints( );
  vtkIdType numSourcePts = sourcePts->GetNumberOfPoints( );

  rshBasisMatrix.set_size( numSourcePts, this->NumberOfSphericalHarmonics );
  thetaVector.set_size( numSourcePts );
  phiVector.set_size( numSourcePts );

  double pt[3];

  for (vtkIdType m = 0; m < numSourcePts; m++)
  {
    sourcePts->GetPoint(m,pt);
    /*** Grad directions relation to theta,phi
    * x = sin(phi) * cos(theta)
    * y = sin(phi) * sin(theta)
    * z = cos(phi)
    */
    double theta, phi;
    if ( pt[2] == 1)
    {
      theta =0.0;
      phi   =0.0;
    }
    else
    {
      phi     = acos(pt[2]);
      theta   = atan2(pt[1],pt[0]); // atan2(y,x) = atan(y/x);
      theta = theta < 0 ? theta + 2*vtkMath::Pi() : theta;
    }

    for (unsigned int c = 0; c < this->NumberOfSphericalHarmonics; c++)
    {
      rshBasisMatrix[m][c]  = EvaluateBasis(c,theta,phi);
      if ( vcl_abs(theta - 5.96903) < .01 )
        std::cerr << theta << " , " << phi << "  " << EvaluateBasis(c,theta,phi) << std::endl;
    }
    phiVector[m]    = phi;
    thetaVector[m]  = theta;
  }
}

double vtkSphericalHarmonicSource::EvaluateBasis(
  int c,
  double theta,
  double phi
  )
{
  /**
   * There is potential for confusion here.
   * VTK code is using phi measured as the azmuthial angle. and theta measured off the x-axis
   * However the itk SphereHarmonic Code defines theta as the azmuthial angle and phi measured of the x-axis
   * this note should explain the difference between the notation in the sphereRep API.
   */
  switch ( this->Order )
  {
    case 0:
    {
      return itk::RealSymSphericalHarmonicBasis<0>::Instance().Y(c+1,(double)phi,(double)theta);
      break;
    }
    case 2:
    {
      return itk::RealSymSphericalHarmonicBasis<2>::Instance().Y(c+1,(double)phi,(double)theta);
      break;
    }
    case 4:
    {
      return itk::RealSymSphericalHarmonicBasis<4>::Instance().Y(c+1,(double)phi,(double)theta);
      break;
    }
    case 6:
    {
      return itk::RealSymSphericalHarmonicBasis<6>::Instance().Y(c+1,(double)phi,(double)theta);
      break;
    }
    case 8:
    {
      return itk::RealSymSphericalHarmonicBasis<8>::Instance().Y(c+1,(double)phi,(double)theta);
      break;
    }
    case 10:
    {
      return itk::RealSymSphericalHarmonicBasis<10>::Instance().Y(c+1,(double)phi,(double)theta);
      break;
    }
    default:
    {
      std::cout << "Unsupported RSH ORDER : " << this->Order << std::endl;
      return -1.0;
      //EXCEPTION!!!
    }
  }

}

void vtkSphericalHarmonicSource::computePoint(
  double theta,
  double phi,
  double d[3],
  double radius
  )
{
  //INGENERAL WE USE LAS as opposed to RAS so we need an xFlip...
  //This should be handeled by the diffusionEditor matrix in general!

  if (RotationMatrix != NULL)
  {
    double pt[4];
    double *tpt;
    pt[0] = -radius * sin((double)phi) * cos((double)theta);
    pt[1] = radius * sin((double)phi) * sin((double)theta);
    pt[2] = radius * cos((double)phi);
    pt[3] = 1.0;
    tpt = RotationMatrix->MultiplyDoublePoint(pt);
    d[0] = tpt[0]+this->Center[0];
    d[1] = tpt[1]+this->Center[1];
    d[2] = tpt[2]+this->Center[2];
  }
  else
  {
    d[0] = this->Center[0] - radius * sin((double)phi) * cos((double)theta);
    d[1] = this->Center[1] + radius * sin((double)phi) * sin((double)theta);
    d[2] = this->Center[2] + radius * cos((double)phi);
  }

  //~ std::cerr << theta << " , " << phi << "  " << radius << std::endl;

}


