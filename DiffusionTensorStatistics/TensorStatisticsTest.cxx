#include <itkVectorContainer.h>
#include "LinearTensorGeometry.h"
#include "SymmetricSpaceTensorGeometry.h"
#include "TensorStatistics.h"

typedef itk::SymmetricSecondRankTensor<double, 3> TensorType;
typedef itk::Matrix<double, 3> MatrixType;

typedef TensorStatistics<double>::TensorListType TensorListType;
typedef TensorStatistics<double>::TensorListPointerType TensorListPointerType;
typedef TensorStatistics<double>::CovarianceType CovarianceType;
typedef TensorStatistics<double>::PGAVariancesArrayType PGAVariancesArrayType;
typedef TensorStatistics<double>::PGAVectorsMatrixType PGAVectorsMatrixType;

void runCommonTests(TensorGeometry<double, 3> * geometry)
{
  TensorStatistics<double> * stats;
  TensorType tensor, mean;
  TensorListPointerType tensorList;
  CovarianceType covariance;
  PGAVariancesArrayType pgaVariances;
  PGAVectorsMatrixType pgaVectorsMatrix;

  tensorList = TensorListType::New();
  stats = new TensorStatistics<double>(geometry);

  tensor.SetIdentity();

  tensorList->InsertElement(0, tensor);

  stats->ComputeMean(tensorList, mean);
  std::cout << "Mean of single tensor = " << mean << std::endl;
  stats->ComputeMeanAndCovariance(tensorList, mean, covariance);
  std::cout << "Mean and covariance of a single tensor = " << mean << std::endl
            << covariance << std::endl;
  stats->ComputeMeanAndPGA(tensorList, mean, pgaVariances, pgaVectorsMatrix);
  std::cout << "Mean and PGA of a single tensor = " << mean << std::endl
            << pgaVariances << std::endl << pgaVectorsMatrix << std::endl;

  tensorList->InsertElement(1, tensor*2);
  tensorList->InsertElement(2, tensor*4);

  stats->ComputeMean(tensorList, mean);
  std::cout << "Mean of 3 tensors = " << mean << std::endl;
  stats->ComputeMeanAndCovariance(tensorList, mean, covariance);
  std::cout << "Mean and covariance of 3 tensors = " << mean << std::endl
            << covariance << std::endl;
  stats->ComputeMeanAndPGA(tensorList, mean, pgaVariances, pgaVectorsMatrix);
  std::cout << "Mean and PGA of 3 tensors = " << mean << std::endl
            << pgaVariances << std::endl << pgaVectorsMatrix << std::endl;

  std::cout << "Random Gaussian Tensor = "
            << stats->RandomGaussianTensor(mean, 1.0) << std::endl;
}

int main(int, char**)
{
  LinearTensorGeometry<double> * linearTensorGeometry;
  SymmetricSpaceTensorGeometry<double> * ssTensorGeometry;

  linearTensorGeometry = new LinearTensorGeometry<double>;
  ssTensorGeometry = new SymmetricSpaceTensorGeometry<double>;

  std::cout << "Running Linear Statistics Tests" << std::endl;
  runCommonTests(linearTensorGeometry);

  std::cout << "Running Symmetric Space Statistics Tests" << std::endl;
  runCommonTests(ssTensorGeometry);

  return EXIT_SUCCESS;
}
