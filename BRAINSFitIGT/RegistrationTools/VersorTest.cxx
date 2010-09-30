#include "itkVersorRigid3DTransform.h"
#include "itkAffineTransform.h"
#include "itkTransformFileWriter.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include <iostream>

typedef itk::TransformFileWriter TransformWriterType;
typedef itk::VersorRigid3DTransform<double> VersorTransformType;
typedef VersorTransformType::VersorType VersorType;
typedef itk::AffineTransform<double> AffineTransformType;

int main(int arc, char **argv){
  VersorTransformType::Pointer vtfm = VersorTransformType::New();
  AffineTransformType::Pointer atfm = AffineTransformType::New();
  VersorTransformType::VersorType versor;
  
  VersorType::VectorType vaxis;
  vaxis[0] = 0;
  vaxis[1] = 0;
  vaxis[2] = 1.;
  // if versor is set with 4 doubles, this is setting directly the versor components, which we don't want
  versor.Set(vaxis,vnl_math::pi/3.);

  atfm->SetIdentity();
  atfm->Rotate(0,1,vnl_math::pi/3.);

  //VersorTransformType::ParametersType vpar = vtfm->GetParameters();
  //vpar.Fill(0);
  //vpar[0] = 10.;
  //vtfm->SetParameters(vpar);
  vtfm->SetMatrix(versor.GetMatrix());
  
  std::cout << "Versor: " << versor << std::endl;
  std::cout << "Versor angle: " << versor.GetAngle() << std::endl;
  std::cout << "Versor axis: " << versor.GetAxis() << std::endl;
  std::cout << "Versor matrix: " << versor.GetMatrix() << std::endl;
  std::cout << "Versor transform center: " << vtfm->GetCenter() << std::endl;
  std::cout << "Affine transform center: " << atfm->GetCenter() << std::endl;
  std::cout << "Affine Transform matrix: " << std::endl << atfm->GetMatrix() << std::endl;
  std::cout << "Versor Transform matrix: " << std::endl << vtfm->GetMatrix() << std::endl;
  std::cout << "Versor Transform versor: " << vtfm->GetVersor() << std::endl;
  //std::cout << "Transform versor: " << atfm->GetVersor() << std::endl;

  TransformWriterType::Pointer tfmw = TransformWriterType::New();
  tfmw->SetInput(atfm);
  tfmw->SetFileName("/d/scratchbuild/fedorov/affine.tfm");
  tfmw->Update();

  tfmw->SetInput(vtfm);
  tfmw->SetFileName("/d/scratchbuild/fedorov/versor.tfm");
  tfmw->Update();

  // randomly sample the sphere
  // Algorithm #381 by Knop, Comm ACM, 1970
  itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed(123);
  double X, Y, Z, S;
  for(int i=0;i<100;i++){
    X = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->GetUniformVariate(-1.,1.);
    Y = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->GetUniformVariate(-1.,1.);
    S = X*X+Y*Y;
    if(S>1.){
        i--;
        continue;
    }
    Z = 2.*S-1.;
    S = sqrt((1.-Z*Z)/S);
    X = X*S;
    Y = Y*S;
    VersorType::VectorType newAxis;
    newAxis[0] = X;
    newAxis[1] = Y;
    newAxis[2] = Z;
    if(newAxis*versor.GetAxis() < cos(vnl_math::pi/90.)){
        i--;
        continue;
    }
    std::cerr << acos(newAxis*versor.GetAxis())*(180./vnl_math::pi) << std::endl;
    std::cout << "P," << X*10. << "," << Y*10. << "," << Z*10. << ",1,1" << std::endl;


    // check if the angle between the generated vector and versor axis is less than something, if not, reject
    }
  return 0;
}
