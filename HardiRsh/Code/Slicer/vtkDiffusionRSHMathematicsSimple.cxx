
#ifdef _WIN32
// to pick up M_SQRT2 and other nice things...
#define _USE_MATH_DEFINES
#endif

// But, if you are on VS6.0 you don't get the define...
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkImageData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "time.h"
#include "vtkLookupTable.h"

#ifndef M_SQRT2
#define M_SQRT2    1.41421356237309504880168872421      /* sqrt(2) */
#endif

#define VTK_EPS 10e-15

#include "vtkDiffusionRSHMathematicsSimple.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkDiffusionRSHMathematicsSimple, "$Revision: 1.11 $");

//----------------------------------------------------------------------------
vtkDiffusionRSHMathematicsSimple* vtkDiffusionRSHMathematicsSimple::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkDiffusionRSHMathematicsSimple");
  if(ret)
    {
    return (vtkDiffusionRSHMathematicsSimple*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkDiffusionRSHMathematicsSimple;
}

//----------------------------------------------------------------------------
vtkDiffusionRSHMathematicsSimple::vtkDiffusionRSHMathematicsSimple()
{
  this->Operation = VTK_RSH_POWER_2;

  this->ScaleFactor = 1000.0;
}

//----------------------------------------------------------------------------
 vtkDiffusionRSHMathematicsSimple::~vtkDiffusionRSHMathematicsSimple()
 {
 }

//----------------------------------------------------------------------------
int vtkDiffusionRSHMathematicsSimple::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  //vtkInformation *inInfo2 = inputVector[1]->GetInformationObject(0);

  int ext[6];

  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),ext);

  //int ext[6], *ext2, idx;


  // We want the whole extent of the tensors, and should
  // allocate the matching size image data.
  vtkDebugMacro(<<"Execute information extent: " <<
    ext[0] << " " << ext[1] << " " << ext[2] << " " <<
    ext[3] << " " << ext[4] << " " << ext[5]);


  //We want to output float!
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_FLOAT, 1);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),ext,6);
  return 1;
}



//----------------------------------------------------------------------------
int vtkDiffusionRSHMathematicsSimple::FillInputPortInformation(
  int port, vtkInformation* info)
{
  if (port == 1)
    {
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    }
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  return 1;
}


//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
// Handles the one input operations.
// Handles the ops where eigensystems are computed.
template <class T>
static void vtkDiffusionRSHMathematicsSimpleExecute1(vtkDiffusionRSHMathematicsSimple *self,
                                                       vtkImageData *input,
                                                       vtkImageData *output,
                                                       T *outPtr)
{
  int dims[3];
  input->GetDimensions(dims);

  int size = dims[0]*dims[1]*dims[2];

 // image variables
  // progress
  //unsigned long count = 0;
  //unsigned long target;

  // math operation
  int op = self->GetOperation();

  // time
  clock_t tStart=0;
  tStart = clock();

  // scaling
  double scaleFactor = self->GetScaleFactor();

  // find the input region to loop over
  vtkPointData *pd = input->GetPointData();

  vtkDataArray *inCoeffs = pd->GetScalars();
//  vtkDoubleArray *inCoeffs = vtkDoubleArray::SafeDownCast ( pd->GetScalars() );
  int numPts = input->GetNumberOfPoints();

  if ( !inCoeffs || numPts < 1 )
    {
    vtkGenericWarningMacro(<<"No input tensor data to filter!");
    return;
    }

  int numCoeffs = inCoeffs->GetNumberOfComponents();
  double coeffs[numCoeffs];

  int inPtId = 0;
  for(int n=0; n<size; n++)
  {
    inPtId = n;

    // tensor at this voxel
    inCoeffs->GetTuple(inPtId,coeffs);

    // pixel operation
    switch (op)
    {
      case vtkDiffusionRSHMathematicsSimple::VTK_RSH_POWER_0:
        *outPtr = static_cast<T> (scaleFactor*vtkDiffusionRSHMathematicsSimple::computePowerInOrder0(coeffs,numCoeffs));
        break;
      case vtkDiffusionRSHMathematicsSimple::VTK_RSH_POWER_2:
        *outPtr = static_cast<T> (scaleFactor*vtkDiffusionRSHMathematicsSimple::computePowerInOrder2(coeffs,numCoeffs));
        break;
      case vtkDiffusionRSHMathematicsSimple::VTK_RSH_POWER_4:
        *outPtr = static_cast<T> (scaleFactor*vtkDiffusionRSHMathematicsSimple::computePowerInOrder4(coeffs,numCoeffs));
        break;
      case vtkDiffusionRSHMathematicsSimple::VTK_RSH_POWER_6:
        *outPtr = static_cast<T> (scaleFactor*vtkDiffusionRSHMathematicsSimple::computePowerInOrder6(coeffs,numCoeffs));
        break;
      case vtkDiffusionRSHMathematicsSimple::VTK_RSH_POWER_8:
        *outPtr = static_cast<T> (scaleFactor*vtkDiffusionRSHMathematicsSimple::computePowerInOrder8(coeffs,numCoeffs));
        break;
    }
    
    if ( isnan(*outPtr) )
    {
      std::cerr << "vtkDiffusionRSHMathematicsSimple Returned Nan!!!" << std::endl;
      *outPtr = static_cast<T>(0.0);
    }
    outPtr++;
  }
//  trans->Delete();
}

double vtkDiffusionRSHMathematicsSimple::computePowerInOrder0(double *coeffs, int nComps)
{
  return coeffs[0];
}

double vtkDiffusionRSHMathematicsSimple::computePowerInOrder2(double *coeffs, int nComps)
{
  double sum = 0.0;
  if (nComps < 6) //Not enough componants for this order. It shouldn't have been in the menu!
  {
    return 0;
  }
  for (int i=1;i<6;++i)
  {
    sum += coeffs[i]*coeffs[i];
  }
  return sum;
}

double vtkDiffusionRSHMathematicsSimple::computePowerInOrder4(double *coeffs, int nComps)
{
  double sum = 0.0;
  if (nComps < 15) //Not enough componants for this order. It shouldn't have been in the menu!
  {
    return 0;
  }
  for (int i=6;i<14;++i)
  {
    sum += coeffs[i]*coeffs[i];
  }
  return sum;

}

double vtkDiffusionRSHMathematicsSimple::computePowerInOrder6(double *coeffs, int nComps)
{
  double sum = 0.0;
  if (nComps < 28) //Not enough componants for this order. It shouldn't have been in the menu!
  {
    return 0;
  }
  for (int i=15;i<27;++i)
  {
    sum += coeffs[i]*coeffs[i];
  }
  return sum;

}

double vtkDiffusionRSHMathematicsSimple::computePowerInOrder8(double *coeffs, int nComps)
{
  double sum = 0.0;
  if (nComps < 45) //Not enough componants for this order. It shouldn't have been in the menu!
  {
    return 0;
  }
  for (int i=28;i<44;++i)
  {
    sum += coeffs[i]*coeffs[i];
  }
  return sum;

}

double vtkDiffusionRSHMathematicsSimple::computePowerInOrder10(double *coeffs, int nComps)
{
  double sum = 0.0;
  if (nComps < 66) //Not enough componants for this order. It shouldn't have been in the menu!
  {
    return 0;
  }
  for (int i=45;i<65;++i)
  {
    sum += coeffs[i]*coeffs[i];
  }
  return sum;

}

void vtkDiffusionRSHMathematicsSimple::SimpleExecute(vtkImageData* input,
                                                        vtkImageData* output)
{

  //void* inPtr = input->GetScalarPointer();
  void* outPtr = output->GetScalarPointer();

  switch (this->GetOperation())
    {

      // Operations where eigenvalues are not computed
    case VTK_RSH_POWER_0:
    case VTK_RSH_POWER_2:
    case VTK_RSH_POWER_4:
    case VTK_RSH_POWER_6:
    case VTK_RSH_POWER_8:
      switch (output->GetScalarType())
      {
      // we set the output data scalar type depending on the op
      // already.  And we only access the input tensors
      // which are float.  So this switch statement on output
      // scalar type is sufficient.
      vtkTemplateMacro4(vtkDiffusionRSHMathematicsSimpleExecute1,
                this, input, output, (VTK_TT *)(outPtr));
      default:
        vtkErrorMacro(<< "Execute: Unknown ScalarType");
        return;
      }
      break;
  }
}


void vtkDiffusionRSHMathematicsSimple::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Operation: " << this->Operation << "\n";
}
