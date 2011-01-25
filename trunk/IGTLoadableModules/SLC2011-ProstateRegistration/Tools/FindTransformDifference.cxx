#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkVersorRigid3DTransform.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
//#include "itkResampleImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include <iostream>

#include "FindTransformDifferenceCLP.h"
#include <sstream>

#define DEG(x) ((x)*180./vnl_math::pi)

typedef itk::TransformFileReader TransformReaderType;
typedef itk::VersorRigid3DTransform<double> VersorTransformType;
typedef itk::Image<unsigned char,3> MaskImageType;
typedef itk::ImageFileReader<MaskImageType> ReaderType;
//typedef itk::ResampleImageFilter<MaskImageType,MaskImageType> ResamplerType;
typedef itk::NearestNeighborInterpolateImageFunction<MaskImageType, double> NNInterpolatorType;
typedef itk::ImageRegionConstIteratorWithIndex<MaskImageType> IteratorType;

// input: average rigid transform, case id, experiment id
// output: for each case: initial misalignment transform parameters, final
//   rigid transform parameters
// idea: look how the initial misalignment affects the success of rigid
// registration
// if rigid is ok, follow up with the more detailed error analysis of bspline
// final tfm

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  TransformReaderType::Pointer tfm1Reader = TransformReaderType::New();
  tfm1Reader->SetFileName(Tfm1Name.c_str());
  TransformReaderType::Pointer tfm2Reader = TransformReaderType::New();
  tfm2Reader->SetFileName(Tfm2Name.c_str());

  try{
    tfm1Reader->Update();
    tfm2Reader->Update();
  } catch( itk::ExceptionObject & excep ) {
    std::cerr << "Exception : " << excep << std::endl;   
    return -1;
  }

  if(!dynamic_cast<VersorTransformType*>(tfm1Reader->GetTransformList()->front().GetPointer()) ||
     !dynamic_cast<VersorTransformType*>(tfm2Reader->GetTransformList()->front().GetPointer())){
    std::cerr << "Input transform(s) must be of type VersorRigid3DTransform!" << std::endl;
    return -1;
  }

  const VersorTransformType::ConstPointer tfm1 =
    dynamic_cast<VersorTransformType const *const>(tfm1Reader->GetTransformList()->front().GetPointer());
  const VersorTransformType::ConstPointer tfm2 =
    dynamic_cast<VersorTransformType const *const>(tfm2Reader->GetTransformList()->front().GetPointer());

  VersorTransformType::ParametersType tfm1p = tfm1->GetParameters();
  VersorTransformType::ParametersType tfm2p = tfm2->GetParameters();

  // compute eular angle and displacement
  double dRoll;          // roll
  double dPitch;        // pitch
  double dYaw;          // yaw
  double dR, dA, dS; // displacement

  dR = fabs(tfm1p[3]-tfm2p[3]);
  dA = fabs(tfm1p[4]-tfm2p[4]);
  dS = fabs(tfm1p[5]-tfm2p[5]);

  dRoll = fabs(atan(2*(tfm1p[0]*tfm1p[1] + tfm1p[2])/(1-2*(tfm1p[1]*tfm1p[1] + tfm1p[2]*tfm1p[2]))) - 
    atan(2*(tfm2p[0]*tfm2p[1] + tfm2p[2])/(1-2*(tfm2p[1]*tfm2p[1] + tfm2p[2]*tfm2p[2]))));
  dPitch = fabs( asin(2*(tfm1p[0]*tfm1p[2] - tfm1p[1])) - asin(2*(tfm2p[0]*tfm2p[2] - tfm2p[1])));
  dYaw = fabs(atan(2*(tfm1p[0] + tfm1p[1]*tfm1p[2])/(1-2*(tfm1p[2]*tfm1p[2] + 1))) -
    atan(2*(tfm2p[0] + tfm2p[1]*tfm2p[2])/(1-2*(tfm2p[2]*tfm2p[2] + 1))));
  
  dRoll *= 180/3.14159265;
  dPitch *= 180/3.14159265;
  dYaw *= 180/3.14159265;

  std::cout << "dR = " << dR << " dA = " << dA << " dS = " << dS <<
    " dRoll = " << dRoll << " dPitch = " << dPitch << " dYaw = " << dYaw << std::endl;

  if(MaskImageName != ""){
    try{
      std::cout << "Computing the error over the segmented region" << std::endl;
      ReaderType::Pointer maskReader = ReaderType::New();
      maskReader->SetFileName(MaskImageName.c_str());
      maskReader->Update();

      /*
      ReaderType::Pointer refReader = ReaderType::New();
      refReader->SetFileName(RefImageName.c_str());
      refReader->Update();

      MaskImageType::Pointer ref = refReafer->GetOutput();
      */

      MaskImageType::Pointer mask = maskReader->GetOutput();
      double meanDist = 0, maxDist = -1, minDist = 1000, count = 0;
      IteratorType maskIt(mask, mask->GetBufferedRegion());
      for(maskIt.GoToBegin();!maskIt.IsAtEnd();++maskIt){
        MaskImageType::PointType pt, pt0, pt1, dpt;
        MaskImageType::IndexType id;
        id = maskIt.GetIndex();
        mask->TransformIndexToPhysicalPoint(id, pt);
        pt0 = tfm1->TransformPoint(pt);
        pt1 = tfm2->TransformPoint(pt);
        
        double dist = sqrt( (pt0[0]-pt1[0])*(pt0[0]-pt1[0])+
                          (pt0[1]-pt1[1])*(pt0[1]-pt1[1])+
                          (pt0[2]-pt1[2])*(pt0[2]-pt1[2]) );
        meanDist += dist;
        maxDist = dist>maxDist ? dist : maxDist;
        minDist = dist<minDist ? dist : minDist;
        count ++;
        //dpt = pt1-pt2;
      }
      meanDist /= double(count);
      std::cout << "Mean: " << meanDist << " Max: " << maxDist << " Min: " << minDist << std::endl;

    } catch (itk::ExceptionObject& e){
      std::cerr << "Failed to read input images" << std::endl;
    }
  }
  return EXIT_SUCCESS;
}
