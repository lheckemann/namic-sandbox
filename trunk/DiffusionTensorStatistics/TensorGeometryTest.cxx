#include "LinearTensorGeometry.h"
#include "SymmetricSpaceTensorGeometry.h"

typedef itk::SymmetricSecondRankTensor<double, 3> TensorType;
typedef itk::Matrix<double, 3> MatrixType;

void runCommonTests(TensorGeometry<double, 3> * geometry)
{
  TensorType tensor, tensor2;
  TensorType tangentTensor, tangentTensor2;

  // InnerProduct / Norm / NormSquared tests
  tensor.SetIdentity();
  tangentTensor.Fill(0.0);
  std::cout << "Inner Product (<0,0>, at identity) = "
            << geometry->InnerProduct(tensor, tangentTensor, tangentTensor)
            << std::endl;
  std::cout << "Norm = " << geometry->Norm(tensor, tangentTensor)
            << std::endl;
  std::cout << "NormSquared = " << geometry->NormSquared(tensor, tangentTensor)
            << std::endl;

  tangentTensor.SetIdentity();
  std::cout << "Inner Product (<1,1>, at identity) = "
            << geometry->InnerProduct(tensor, tangentTensor, tangentTensor)
            << std::endl;
  std::cout << "Norm = " << geometry->Norm(tensor, tangentTensor)
            << std::endl;
  std::cout << "NormSquared = " << geometry->NormSquared(tensor, tangentTensor)
            << std::endl;

  tangentTensor(0, 1) = 1.0;
  std::cout << "Inner Product with off-diagonal = "
            << geometry->InnerProduct(tensor, tangentTensor, tangentTensor)
            << std::endl;
  std::cout << "Norm = " << geometry->Norm(tensor, tangentTensor)
            << std::endl;
  std::cout << "NormSquared = " << geometry->NormSquared(tensor, tangentTensor)
            << std::endl;

  tangentTensor2.SetIdentity();
  tangentTensor(1, 1) = 1.0;
  std::cout << "Inner Product with different vectors = "
            << geometry->InnerProduct(tensor, tangentTensor, tangentTensor2)
            << std::endl;

  tensor(0,0) = 2.0;
  tensor(1,1) = 2.0;
  tensor(2,2) = 2.0;
  tangentTensor.Fill(0.0);
  std::cout << "Inner Product (<0,0>, at 2*identity) = "
            << geometry->InnerProduct(tensor, tangentTensor, tangentTensor)
            << std::endl;
  std::cout << "Norm = " << geometry->Norm(tensor, tangentTensor)
            << std::endl;
  std::cout << "NormSquared = " << geometry->NormSquared(tensor, tangentTensor)
            << std::endl;

  tangentTensor.SetIdentity();
  std::cout << "Inner Product (<1,1>, at 2*identity) = "
            << geometry->InnerProduct(tensor, tangentTensor, tangentTensor)
            << std::endl;
  std::cout << "Norm = " << geometry->Norm(tensor, tangentTensor)
            << std::endl;
  std::cout << "NormSquared = " << geometry->NormSquared(tensor, tangentTensor)
            << std::endl;

  tangentTensor(0, 1) = 1.0;
  std::cout << "Inner Product with off-diagonal = "
            << geometry->InnerProduct(tensor, tangentTensor, tangentTensor)
            << std::endl;
  std::cout << "Norm = " << geometry->Norm(tensor, tangentTensor)
            << std::endl;
  std::cout << "NormSquared = " << geometry->NormSquared(tensor, tangentTensor)
            << std::endl;

  tangentTensor2.SetIdentity();
  tangentTensor(1, 1) = 1.0;
  std::cout << "Inner Product with different vectors = "
            << geometry->InnerProduct(tensor, tangentTensor, tangentTensor2)
            << std::endl;


  // Exp, Log, and Distance Tests
  tensor.SetIdentity();
  tensor2.Fill(0.0);
  tensor2(0,0) = 2.0;
  tensor2(1,1) = 2.0;
  tensor2(2,2) = 2.0;
  std::cout << "Log (1 -> 2) = " << geometry->LogMap(tensor, tensor2)
            << std::endl;
  std::cout << "Distance(1 -> 2) = " << geometry->Distance(tensor, tensor2)
            << std::endl;

  tangentTensor.SetIdentity();
  std::cout << "Exp(1, 1) = " << geometry->ExpMap(tensor, tangentTensor)
            << std::endl;
}

int main(int, char **)
{
  TensorType tensor;
  MatrixType matrix;

  LinearTensorGeometry<double, 3> * linearTensorGeometry;
  SymmetricSpaceTensorGeometry<double, 3> * ssTensorGeometry;

  linearTensorGeometry = new LinearTensorGeometry<double, 3>;
  ssTensorGeometry = new SymmetricSpaceTensorGeometry<double, 3>;

  std::cout << "Running LinearTensorGeometry Tests" << std::endl;
  runCommonTests(linearTensorGeometry);
  std::cout << std::endl;

  std::cout << "Running SymmetricSpaceGeometry Tests" << std::endl;
  runCommonTests(ssTensorGeometry);

  // In addition, test group action for symmetric space
  matrix.SetIdentity();
  tensor.SetIdentity();
  std::cout << "Group action (1, 1) = "
            << ssTensorGeometry->GroupAction(tensor, matrix) << std::endl;

  matrix(0,0) = 2.0;
  tensor(0,0) = 2.0;
  std::cout << "Group action diagonal = "
            << ssTensorGeometry->GroupAction(tensor, matrix) << std::endl;

  return EXIT_SUCCESS;
}
