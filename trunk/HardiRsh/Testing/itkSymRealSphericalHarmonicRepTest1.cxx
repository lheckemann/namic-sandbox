
#include "itkRealSymSphericalHarmonicBasis.h"
#include "itkSymRealSphericalHarmonicRep.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_random.h"

#include <iostream>
#include <iomanip>

#include "itkTimeProbe.h"
using namespace std;

vnl_matrix<double>
mkRotMat(double a, double b, double g)
{
    vnl_matrix<double> rotA(3,3,0);
    vnl_matrix<double> rotB(3,3,0);
    vnl_matrix<double> rotG(3,3,0);

    rotA(0,0) = vcl_cos(a); rotA(0,1) = -vcl_sin(a);
    rotA(1,0) = vcl_sin(a); rotA(1,1) = vcl_cos(a);
    rotA(2,2) = 1;

    rotG(0,0) = vcl_cos(g); rotG(0,1) = -vcl_sin(g);
    rotG(1,0) = vcl_sin(g); rotG(1,1) = vcl_cos(g);
    rotG(2,2) = 1;

    rotB(0,0) = vcl_cos(b); rotB(0,2) = vcl_sin(b);
    rotB(1,1) = 1;
    rotB(2,0) = -vcl_sin(b); rotB(2,2) = vcl_cos(b);

    vnl_matrix<double> res = rotG * rotB * rotA;
    return res;
  
}

int
RotateTest()
{
  
#ifdef USE_WIGNER_ROTATION
  std::cerr << "Using Wigner Rotation" << std::endl;
#else
  std::cerr << "NOT Using Wigner Rotation" << std::endl;
#endif
  
  typedef double           PrecisionType;

  PrecisionType c1[45] = {0.7212520319878115,-0.0003385282051116059,0.0000749605643584339,-0.595677776291134,0.00030204505569102896,
   0.0007960419344463232,0.0007041597783487286,-0.00004763110564954577,0.00023051855355286227,-0.00041468059636469434,
   0.31454381107471285,0.0003240763081598791,-0.0003646591186538331,-0.00013836214560059706,0.0010292282395887864,
   0.0011945036277760113,0.00044623369448974076,-0.0007192376368217943,0.0001093875566357573,-0.00015953304798718704,
   -0.0014569479866522461,-0.12532455607008042,0.001029082609176401,0.00015452420774457638,-0.0002648896339109974,
   -0.0012802266647137153,0.000852198927608263,0.001338962813971084,0.003603085155208449,-0.00008088348021568353,
   -0.0006703905742253685,0.000771066948178821,0.0012535994711646709,-0.0012808114574620819,0.000998683995279085,
   0.0014864901181477208,0.03818977726185059,-0.0015034416680695956,-0.0018179313371678475,-0.0020668893003337463,
   0.0019221161938821688,-0.0010735703142327364,-0.0013657370782731923,-0.00015421727639094215,-0.0003780481257301127
  };
  
  typedef itk::SymRealSphericalHarmonicRep< PrecisionType, 8 >
                                                          PixelType;

  typedef PixelType::GradientDirectionType                GradType;
  
  vnl_matrix<double> rotMat = mkRotMat(0.1,0.12,0.23);

  PixelType pix1(c1);
  pix1 *= 1000;
  PixelType rotPix1;
  
  itk::TimeProbe timer1;
  timer1.Start();
  rotPix1 = pix1.Rotate(rotMat);
  timer1.Stop();
  std::cout << "RotationTest: Init Rotation took " << timer1.GetMeanTime() << " seconds.\n";

  itk::TimeProbe timer2;
  timer2.Start();
  rotPix1 = pix1.Rotate(rotMat);
  timer2.Stop();
  std::cout << "RotationTest: second Rotation took " << timer2.GetMeanTime() << " seconds.\n";

  GradType dir;
  
  double alpha,beta,gamma;
  unsigned int numTests = 10000;
  
  const long seed = 10000L;
  vnl_random rand(seed);
//  vnl_random rand;
  itk::TimeProbe timer;
  timer.Start();

  double maxError = 0; 
  double error = 0; 
  for (unsigned int i = 0;i < numTests;++i)
  {
    
    dir[0] = rand.drand32(-1,1);
    dir[1] = rand.drand32(-1,1);
    dir[2] = rand.drand32(-1,1);
    dir = dir/dir.two_norm();
    
    //Pick these at random!
    alpha = rand.drand32(-3.14159,3.14159);
    beta = rand.drand32(-1.5708,1.5708);
    gamma = rand.drand32(-3.14159,3.14159);
    
    rotMat = mkRotMat(alpha,beta,gamma);
    double val1 = pix1.Evaluate(rotMat.transpose() * dir);
    error = vcl_abs( (val1 - pix1.Rotate(rotMat).Evaluate(dir)) /val1);
    if (error > maxError) maxError = error;
    if ( error > 0.0001 )
    {
      std::cerr << "Rotation Test Failed!" << std::endl;
      std::cerr << "i             :" << i << std::endl;
      std::cerr << "rotMat        :\n" << rotMat << std::endl;
      std::cerr << "dir           : " << dir << std::endl;
      std::cerr << "rotdir        : " << rotMat * dir << std::endl;
      
      std::cerr << "\nalpha           : " << alpha << std::endl;
      std::cerr << "beta           : " << setiosflags(ios::fixed) << setprecision(8) << beta << std::endl;
      std::cerr << "gamma           : " << gamma << std::endl;

      std::cerr << "evalutate     : " << pix1.Evaluate(rotMat * dir) << std::endl;
      std::cerr << "Rot Evaluate  : " << pix1.Rotate(rotMat).Evaluate(dir) << std::endl;
      std::cerr << "error         : " << error << std::endl;
      
      //~ std::cout
       //~ << setiosflags(ios::fixed) << setprecision(8) << PixelType::BasisType::Instance().ComputeRotationMatrix(
        //~ static_cast<itk::Matrix<double, 3u, 3u> >(rotMat)) << std::endl;

      return EXIT_FAILURE;
    }
  }

  timer.Stop();
  std::cout << "RotationTest: Time Elapsed " << timer.GetMeanTime() << " seconds.\n";
  std::cout << "max percent Error : " << maxError << "\n";


  std::cout << "passed RotateTest\n";
  return EXIT_SUCCESS;
}

using namespace itk;
int main( int argc, char * argv[] )
{

  if (RotateTest())
    return EXIT_FAILURE;
  
  return EXIT_SUCCESS;  
}
