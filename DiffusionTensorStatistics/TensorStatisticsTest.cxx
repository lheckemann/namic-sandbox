#include <itkVectorContainer.h>
#include "LinearTensorGeometry.h"
#include "SymmetricSpaceTensorGeometry.h"
#include "TensorStatistics.h"

typedef itk::SymmetricSecondRankTensor<double, 3> TensorType;
typedef itk::Matrix<double, 3> MatrixType;

typedef TensorStatistics<double>::TensorListType TensorListType;
typedef TensorStatistics<double>::TensorListPointerType TensorListPointerType;
typedef TensorStatistics<double>::ScalarListType ScalarListType;
typedef TensorStatistics<double>::ScalarListPointerType ScalarListPointerType;
typedef TensorStatistics<double>::CovarianceType CovarianceType;
typedef TensorStatistics<double>::PGAVariancesArrayType PGAVariancesArrayType;
typedef TensorStatistics<double>::PGAVectorsMatrixType PGAVectorsMatrixType;

void runCommonTests(TensorGeometry<double, 3> * geometry)
{
  TensorStatistics<double> * stats;
  TensorType tensor, mean;
  TensorListPointerType tensorList;
  ScalarListPointerType weightList;
  CovarianceType covariance;
  PGAVariancesArrayType pgaVariances;
  PGAVectorsMatrixType pgaVectorsMatrix;
  int i;

  tensorList = TensorListType::New();
  weightList = ScalarListType::New();
  stats = new TensorStatistics<double>(geometry);

  tensor.SetIdentity();

  tensorList->InsertElement(0, tensor);
  weightList->InsertElement(0, 1.0);

  stats->ComputeMean(tensorList, mean);
  std::cout << "Mean of single tensor = " << mean << std::endl;
  stats->ComputeWeightedAve(weightList, tensorList, mean);
  std::cout << "Weighted average of single tensor = " << mean << std::endl;
  stats->ComputeMeanAndCovariance(tensorList, mean, covariance);
  std::cout << "Mean and covariance of a single tensor = " << mean << std::endl
            << covariance << std::endl;
  stats->ComputeMeanAndPGA(tensorList, mean, pgaVariances, pgaVectorsMatrix);
  std::cout << "Mean and PGA of a single tensor = " << mean << std::endl
            << pgaVariances << std::endl << pgaVectorsMatrix << std::endl;

  tensorList->InsertElement(1, tensor*2);
  tensorList->InsertElement(2, tensor*4);

  weightList->SetElement(0, 1/3.0);
  weightList->InsertElement(1, 1/3.0);
  weightList->InsertElement(2, 1/3.0);

  stats->ComputeMean(tensorList, mean);
  std::cout << "Mean of 3 tensors = " << mean << std::endl;
  stats->ComputeWeightedAve(weightList, tensorList, mean);

  std::cout << "Weighted average of 3 tensors (equal weights) = " << mean
            << std::endl;
  weightList->SetElement(0, 0.5);
  weightList->SetElement(1, 0.3);
  weightList->SetElement(2, 0.2);
  stats->ComputeWeightedAve(weightList, tensorList, mean);
  std::cout << "Weighted average of 3 tensors (unequal weights) = " << mean
            << std::endl;
  stats->ComputeMeanAndCovariance(tensorList, mean, covariance);

  std::cout << "Mean and covariance of 3 tensors = " << mean << std::endl
            << covariance << std::endl;
  stats->ComputeMeanAndPGA(tensorList, mean, pgaVariances, pgaVectorsMatrix);
  std::cout << "Mean and PGA of 3 tensors = " << mean << std::endl
            << pgaVariances << std::endl << pgaVectorsMatrix << std::endl;

  std::cout << "Random Gaussian Tensor = "
            << stats->RandomGaussianTensor(mean, 4.0) << std::endl;

  std::cout << "Random Log Tensor = "
            << geometry->LogMap(mean, stats->RandomGaussianTensor(mean, 4.0))
            << std::endl;

  // Find stats of a list of random tensors
  tensorList->Initialize();
  weightList->Initialize();
  mean.SetIdentity();
  for(i = 0; i < 100; i++)
  {
    tensorList->InsertElement(i, stats->RandomGaussianTensor(mean, 1.0));
    weightList->InsertElement(i, 0.01);
  }

  stats->ComputeMean(tensorList, mean);
  std::cout << "Mean of 100 random tensors = " << mean << std::endl;
  stats->ComputeWeightedAve(weightList, tensorList, mean);
  std::cout << "Weighted average of 100 random tensors (equal weights) = "
            << mean << std::endl;
  stats->ComputeMeanAndCovariance(tensorList, mean, covariance);
  std::cout << "Mean and covariance of 100 random tensors = "
            << mean << std::endl << covariance << std::endl;
  stats->ComputeMeanAndPGA(tensorList, mean, pgaVariances, pgaVectorsMatrix);
  std::cout << "Mean and PGA of 100 random tensors = " << mean << std::endl
            << pgaVariances << std::endl << pgaVectorsMatrix << std::endl;
}

int main(int, char**)
{
  LinearTensorGeometry<double> * linearTensorGeometry;
  SymmetricSpaceTensorGeometry<double> * ssTensorGeometry;

  linearTensorGeometry = new LinearTensorGeometry<double>;
  ssTensorGeometry = new SymmetricSpaceTensorGeometry<double>;


  srand(10);


  std::cout << "Running Linear Statistics Tests" << std::endl;
  runCommonTests(linearTensorGeometry);

  std::cout << "Running Symmetric Space Statistics Tests" << std::endl;
  runCommonTests(ssTensorGeometry);


  // More symmetric space tests
  TensorType tensor, mean;
  TensorListPointerType tensorList;
  itk::Matrix<double, 3, 3> m;
  TensorStatistics<double> * stats;
  stats = new TensorStatistics<double>(ssTensorGeometry);

  tensorList = TensorListType::New();

  m.Fill(0.0);
  m(0,0) = cos(1);
  m(1,1) = cos(1);
  m(0,1) = -sin(1);
  m(1,0) = sin(1);
  m(2,2) = 1.0;

  m.SetIdentity();
  tensor.SetIdentity();
  tensor(1,1) = 2.0;
  tensor(2,2) = 0.5;
  std::cout << "Group action = " << ssTensorGeometry->GroupAction(tensor, m)
            << std::endl;

  tensorList->Initialize();
  tensorList->InsertElement(0, ssTensorGeometry->GroupAction(tensor, m));
  tensorList->InsertElement(1, ssTensorGeometry->GroupAction(tensor*2.0, m));
  tensorList->InsertElement(2, ssTensorGeometry->GroupAction(tensor*4.0, m));

  stats->ComputeMean(tensorList, mean);
  std::cout << "Mean = " << mean << std::endl;
  return EXIT_SUCCESS;
}
