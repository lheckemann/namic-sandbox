#include "imgRegByPtSet3D.h"

#include <csignal>
#include <iostream>

#include "vnl/vnl_random.h"
#include <vnl/algo/vnl_matrix_inverse.h>

#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"


#include "ICP3.h"

#include "imageProcessing/imageProcessing.h"


/*============================================================*/
CImgRegByPtSet3D::CImgRegByPtSet3D()
{
  m_lambda = 1;

  m_maxIteration = 50;

  m_numerOfPointForFixedImage = 1000;
  m_numerOfPointForMovingImage = 1000;

  m_maxNumberOfFunctionEvaluation = 1000;

  m_computationDone = false;

  m_fixedImage = 0;
  m_movingImage = 0;

  m_minimumCost = 100000.0;


  m_finalAffineMatrix.set_identity();
  m_finalTranslationVector.fill(0.0);

  computeInitialParameters();
}


/*============================================================*/
CImgRegByPtSet3D::CImgRegByPtSet3D(DoubleImage3DPointerType fixedImg, DoubleImage3DPointerType movingImg)
{
  m_lambda = 1;

  m_maxIteration = 50;

  m_numerOfPointForFixedImage = 1000;
  m_numerOfPointForMovingImage = 1000;

  m_maxNumberOfFunctionEvaluation = 1000;

  m_computationDone = false;

  m_fixedImage = fixedImg;
  m_movingImage = movingImg;

  m_minimumCost = 100000.0;

  m_finalAffineMatrix.set_identity();
  m_finalTranslationVector.fill(0.0);

  computeInitialParameters();
}


/*==================================================*/
void CImgRegByPtSet3D::computeInitialParameters()
{
  m_parameters.set_size(ICP3::m_numOfParameters);
  m_parameters.fill(0.0);

  // affine mtx is be the identity mtx
  m_parameters[0] = 1.0;
  m_parameters[4] = 1.0;
  m_parameters[8] = 1.0;
}


/*============================================================*/
DoubleImage3DPointerType CImgRegByPtSet3D::getFeatureMap(DoubleImage3DPointerType image, \
                                                         double alpha, double beta)
{
  typedef itk::CurvatureAnisotropicDiffusionImageFilter< DoubleImage3DType, DoubleImage3DType >  SmoothingFilterType;
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< DoubleImage3DType, DoubleImage3DType >  GradientFilterType;
  typedef itk::SigmoidImageFilter< DoubleImage3DType, DoubleImage3DType >  SigmoidFilterType;

  /*
   * smooth input image
   */
  SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
  smoothing->SetInput( image );
  smoothing->SetTimeStep( 0.05 );
  smoothing->SetNumberOfIterations(  5 );
  smoothing->SetConductanceParameter( 9.0 );

  /*
   * get gradient magnitude map
   */
  GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
  gradientMagnitude->SetInput( smoothing->GetOutput() );
  gradientMagnitude->SetSigma( 1.0 );

  /*
   * sigmoid transform
   */
  SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
  sigmoid->SetInput( gradientMagnitude->GetOutput() );
  sigmoid->SetOutputMinimum(  0.0  );
  sigmoid->SetOutputMaximum(  1.0  );
  //  const double alpha =  1;
  //  const double beta  =  20;

  sigmoid->SetAlpha( alpha );
  sigmoid->SetBeta(  beta  );

  sigmoid->Update();

  return sigmoid->GetOutput();
}


/*============================================================*/
void CImgRegByPtSet3D::imageToPointSet3D( DoubleImage3DPointerType image, unsigned long numOfPt, PointSet3DType& ptSet)
{
  ptSet.clear();

  // 1st check image > 0, and pick max one 
  typedef itk::ImageRegionIterator< DoubleImage3DType > DoubleImageIteratorType;
  //   DoubleImage3DType::SpacingType spacing = image->GetSpacing();
  //   std::cout<<"spacing is "<<spacing<<std::endl;

  DoubleImageIteratorType it( image, image->GetLargestPossibleRegion() );
  it.GoToBegin();
  
  double maxVal = it.Get();
  double minVal = it.Get();
  double norm1 = 0;

  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      double v = it.Get();

      maxVal = maxVal>v?maxVal:v;
      minVal = minVal<v?minVal:v;

      norm1 += v;
    }

  //  std::cout<<"max and min = "<<maxVal<<"\t"<<minVal<<std::endl;

  if (minVal < 0)
    {
      std::cerr<<"Image has negative values. abort. \n";
      raise(SIGABRT);
    }


  // normalize
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      double v = it.Get();

      it.Set(v/norm1);
    }

  maxVal /= norm1;

  unsigned long nx = image->GetLargestPossibleRegion().GetSize(0);
  unsigned long ny = image->GetLargestPossibleRegion().GetSize(1);
  unsigned long nz = image->GetLargestPossibleRegion().GetSize(2);

  //  std::cout<<"nx, ny, nz = "<<nx<<"\t"<<ny<<"\t"<<nz<<std::endl;

  //  std::cout<<"norm1 = "<<norm1<<std::endl;
  
  double cx = nx/2.0;
  double cy = ny/2.0;
  double cz = nz/2.0;

  vnl_random rg;

  VnlDoubleVector3DType onePt;

  unsigned char interpType = 0;  // nn interp

  double ptX = 0;
  double ptY = 0;
  double ptZ = 0;

  double u = 0;

  double f = 0;

  for (unsigned long is = 0; is < numOfPt; )
    {
      ptX = rg.drand32(nx-1);
      ptY = rg.drand32(ny-1);
      ptZ = rg.drand32(nz-1);

      u = rg.drand32();

      //      std::cout<<"u: "<<u<<std::endl;

      f = douher::offGridImageValue< double >(image, ptX, ptY, ptZ, interpType);

      if (u < (f/maxVal))
        {
          ++is;
          //           onePt[0] = spacing[0]*(ptX - cx);
          //           onePt[1] = spacing[1]*(ptY - cy);
          //           onePt[2] = spacing[2]*(ptZ - cz);
          onePt[0] = ptX - cx;
          onePt[1] = ptY - cy;
          onePt[2] = ptZ - cz;

          //          std::cout<<"idx picked is "<<idx<<std::endl;

          ptSet.push_back(onePt);
          //          std::cout<<is<<std::endl;
        }
    }  

  return;
}


/*============================================================*/
void CImgRegByPtSet3D::savePointSetToTextFile3D(const PointSet3DType& ptSet, const char* fileName)
{
  std::ofstream theFile( fileName );
  if( theFile.fail() )
    {
      std::cerr << "Error opening points file with name : " << std::endl;
      std::cerr << fileName << std::endl;
      raise(SIGABRT);
    }

  unsigned long numPt = ptSet.size();

  for (unsigned long pointId = 0; pointId < numPt; ++pointId)
    {
      VnlDoubleVector3DType onePoint = ptSet[pointId];
      unsigned int dim = onePoint.size(); // should be 3;

      for (unsigned int id = 0; id < dim; ++id)
        {
          theFile << onePoint[id] <<"  ";
        }
      theFile<<std::endl;          

    }

  theFile.close();
}


/*============================================================*/
DoubleImage3DPointerType CImgRegByPtSet3D::getRegisteredMovingImage()
{
  if (m_computationDone == false)
    {
      std::cerr<<"computation not done, abort.\n";
      raise(SIGABRT);
    }

  double fillValue = 0;
  unsigned char interpType = 1;

  DoubleImage3DPointerType resultImg = douher::affineTransform3< double >(m_movingImage, \
                                                                          m_finalAffineMatrix, m_finalTranslationVector, \
                                                                          fillValue, interpType);
  resultImg->SetSpacing( m_movingImage->GetSpacing() );

  return resultImg;
}

/*============================================================*/
void CImgRegByPtSet3D::pointSetRegistration(VnlDoubleVectorType initParam)
{
  ICP3 icp(&m_fixedPointSet, &m_movingPointSet);

  icp.setLambda(m_lambda);
  
  icp.setParameters(initParam);

  icp.setMaxIteration(m_maxIteration);
  icp.setMaxFunctionEvaluationInOptimization(m_maxNumberOfFunctionEvaluation);

  icp.gogogo();

  double c = icp.getCost();
  if ( c < m_minimumCost)
    {
      m_minimumCost = c;

      // use the parameters from icp to transform images
      this->m_parameters = icp.getParameters();

      VnlDoubleMatrix3x3Type A = icp.getAffineMatrix();
      VnlDoubleVector3DType t = icp.getTranslation();

      VnlDoubleMatrixType invAtmp = vnl_matrix_inverse<double>(A);

      for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
          m_finalAffineMatrix(i, j) = invAtmp(i, j);

      m_finalTranslationVector = -m_finalAffineMatrix*t;
    }

  return;
}

/*============================================================*/
void CImgRegByPtSet3D::gogogo()
{
  // 1. convert to pt sets

  /* use the feature image to get the point set */
  //DoubleImage3DPointerType featureFixedImage = getFeatureMap(m_fixedImage);

  /* use the image itself to get the point set */
  DoubleImage3DPointerType featureFixedImage = m_fixedImage;

  imageToPointSet3D( featureFixedImage, m_numerOfPointForFixedImage, m_fixedPointSet);
  //douher::writeImage3<double>(featureFixedImage, "fixFeature.nrrd");


  /* use the feature image to get the point set */
  //DoubleImage3DPointerType featureMovingImage = getFeatureMap(m_movingImage);

  /* use the image itself to get the point set */
  DoubleImage3DPointerType featureMovingImage = m_movingImage;
  imageToPointSet3D( featureMovingImage, m_numerOfPointForMovingImage, m_movingPointSet);
  //douher::writeImage3<double>(featureMovingImage, "movFeature.nrrd");
//   savePointSetToTextFile3D(&ptSet1, "pt1.txt");
//   savePointSetToTextFile3D(&ptSet2, "pt2.txt");

  // 2. icp to reg pt sets
  pointSetRegistration(m_parameters);


  // 3. set done tag
  m_computationDone = true;

  return;
}

