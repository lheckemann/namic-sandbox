
#include "itkImageFileWriter.h"

#include "itkModularLevelSetImageFilter.h"
#include "itkModularLevelSetFunction.h"
#include "itkLevelSetCurvatureTerm.h"
#include "itkLevelSetThresholdTerm.h"

#include "itkDenseFiniteDifferenceImageFilter.h"
#include "itkNarrowBandSolverLevelSetImageFilter.h"
#include "itkSparseFieldLevelSetImageFilter.h"
#include "itkParallelSparseFieldLevelSetImageFilter.h"

#include "itkNarrowBandThresholdSegmentationLevelSetImageFilter.h"

int main(int argc, char* argv[])
{
  const int Dim = 2;

  typedef itk::Image<float,Dim> ImageType;
  typedef itk::Image<unsigned char,Dim> CharImageType;
  typedef ImageType SeedImageType;
  
  ImageType::RegionType reg;
  ImageType::RegionType::SizeType sz;
  ImageType::RegionType::IndexType idx;
  ImageType::RegionType::IndexType maxidx;

  ImageType::Pointer inputImage = ImageType::New();
  SeedImageType::Pointer seedImage = SeedImageType::New();

  ImageType::SpacingType spacing;

  idx[0] = idx[1] = 0;
  sz[0] = 64;
  sz[1] = 32;
  spacing[0] = 1.0;
  spacing[1] = 2.0;
  reg.SetSize(sz);
  reg.SetIndex(idx);

  inputImage->SetRegions(reg);
  seedImage->SetRegions(reg);

  inputImage->SetSpacing(spacing);
  seedImage->SetSpacing(spacing);

  inputImage->Allocate();
  seedImage->Allocate();

  maxidx[0] = sz[0];
  maxidx[1] = sz[1];

  for (idx[1]=0; idx[1]<maxidx[1]; idx[1]++)
    {
    for (idx[0]=0; idx[0]<maxidx[0]; idx[0]++)
      {
      float x[2], X[2];
      x[0] = idx[0] * spacing[0];
      x[1] = idx[1] * spacing[1];
      X[0] = sz[0] * spacing[0];
      X[1] = sz[1] * spacing[1];
      float value = - (sqrt((x[0]-X[0]/2.0) * (x[0]-X[0]/2.0) + (x[1]-X[1]/2.0) * (x[1]-X[1]/2.0)) - 0.4 * X[0]);
      seedImage->SetPixel(idx,value);
      }
    }

  double maxVal = 0.0;
  for (idx[1]=0; idx[1]<maxidx[1]; idx[1]++)
    {
    for (idx[0]=0; idx[0]<maxidx[0]; idx[0]++)
      {
      double val=0.0;
      for (unsigned int i=0; i<2; i++)
        {
        if (idx[i] < maxidx[i]/2)
          {
          val += idx[i] * spacing[i];
          }
        else
          {
          val += (sz[i] - idx[i]) * spacing[i];
          }
        }
      if (val > maxVal)
        {
        maxVal = val;
        }
      inputImage->SetPixel(idx,val);
      }
    }

//  typedef itk::DenseFiniteDifferenceImageFilter<SeedImageType,ImageType> LevelSetSolverType;
//  typedef itk::NarrowBandSolverLevelSetImageFilter<SeedImageType,ImageType> LevelSetSolverType;
//  typedef itk::SparseFieldLevelSetImageFilter<SeedImageType,ImageType> LevelSetSolverType;
  typedef itk::ParallelSparseFieldLevelSetImageFilter<SeedImageType,ImageType> LevelSetSolverType;

  typedef itk::ModularLevelSetFunction<ImageType> LevelSetFunctionType;

  typedef itk::ModularLevelSetImageFilter<LevelSetSolverType,LevelSetFunctionType> LevelSetFilterType;  

  LevelSetFilterType::Pointer levelSetFilter = LevelSetFilterType::New();
  levelSetFilter->SetInput(seedImage);
  levelSetFilter->UseImageSpacingOn();
  levelSetFilter->SetNumberOfThreads(2);

  LevelSetFunctionType::Pointer levelSetFunction = levelSetFilter->GetLevelSetFunction();

  typedef itk::LevelSetCurvatureTerm<ImageType> LevelSetCurvatureTermType;
  LevelSetCurvatureTermType::Pointer curvatureTerm = LevelSetCurvatureTermType::New();
  curvatureTerm->SetWeight(1.0);
  levelSetFunction->AddTerm(curvatureTerm);

  typedef itk::LevelSetThresholdTerm<ImageType,ImageType> LevelSetThresholdTermType;
  LevelSetThresholdTermType::Pointer thresholdTerm = LevelSetThresholdTermType::New();
  thresholdTerm->SetWeight(1.0);
  thresholdTerm->SetSpeedImage(inputImage);
  thresholdTerm->SetLowerThreshold(0.6*maxVal);
  thresholdTerm->SetUpperThreshold(0.4*maxVal);
//  levelSetFunction->AddTerm(thresholdTerm);

  typedef itk::LevelSetPropagationTerm<ImageType,ImageType> LevelSetPropagationTermType;
  LevelSetPropagationTermType::Pointer propagationTerm = LevelSetPropagationTermType::New();
  propagationTerm->SetWeight(1.0);
  propagationTerm->SetSpeedImage(inputImage);
  levelSetFunction->AddTerm(propagationTerm);

  levelSetFunction->Initialize();

  levelSetFilter->SetNumberOfIterations(100);
  levelSetFilter->SetMaximumRMSError(1E-8);

  try
    {
    levelSetFilter->Update();
    }
  catch (itk::ExceptionObject &e)
    {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
    }

  std::cout<<"Elapsed iterations: "<<levelSetFilter->GetElapsedIterations()<<std::endl;
  std::cout<<"RMS change: "<<levelSetFilter->GetRMSChange()<<std::endl;

  itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
  writer->SetInput(levelSetFilter->GetOutput());
  writer->SetFileName(argv[1]);

  try
    {
    writer->Write();
    }
  catch (itk::ExceptionObject &e)
    {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
