#include <string>
#include "sbiaOrientationDistributionFunction.h"

using namespace itk;
int main( int argc, char * argv[] )
{
  typedef OrientationDistributionFunction<double,4>  PixelType;

  /** Test constructors */
  
  /**
  OrientationDistributionFunction();
  OrientationDistributionFunction(const Self& r);
  OrientationDistributionFunction(const Superclass& r);
  OrientationDistributionFunction(const ComponentType& r);
  OrientationDistributionFunction(const ComponentArrayType r);
  */
  PixelType odf1; //OrientationDistributionFunction();
  odf1.SetNthComponent(5,5);
  
  PixelType odf2 = PixelType(odf1); //OrientationDistributionFunction(const Self& r);
  if (odf2[5] != 5)
    return EXIT_FAILURE;
  
  PixelType odf4 = PixelType(5.0); //OrientationDistributionFunction(const ComponentType& r);
  if (odf4[5] != 5.0)
    return EXIT_FAILURE;

  
  PixelType ODF;
    ///Test the first 15 basis Functions against mathematica code...
    const double expResults1[15] = {0.282095, -0.123017, -0.269124, 0.21153, -0.4713, -0.208466,
    -0.0594028, 0.389589, -0.30877, -0.148039, -0.301723, -0.259252, -0.523245, 0.00555991, 0.107564};
    ///Theta \in (0-PI)
  ///Phi \in (0,2 PI)
  double th1 = 2.4132142;
  double ph1 = 5.231231;
  bool passed = true;
    for (int i = 0;i < 15; i++){
    ODF.SetNthComponent(i,1);
//    std::cout << "Testing basis " << i+1 << std::endl;
    double res1 = ODF.Evaluate(th1,ph1);
    if (fabs(res1 - expResults1[i]) >= 0.001){
            printf("Failed - %d : (%f,%f) || %f :: %f \n",i+1,th1,ph1,res1,expResults1[i]);
      passed = false; 
    }
        else
            printf("Passed - %d : (%f,%f) || %f :: %f \n",i+1,th1,ph1,res1,expResults1[i]);

    ODF.SetNthComponent(i,0);
  }

    if (!passed){
      std::cout << "[FAILED]" << std::endl;
      return EXIT_FAILURE;
}  ///Test assignment operators...
      
  /** assignment operator for the vnl_array class. */
//  Self& operator= (const vnl_vector<ComponentType> r);

      return EXIT_SUCCESS;
  
}
