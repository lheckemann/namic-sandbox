#include <csignal>
#include <iostream>
#include <ctime>


#include "ICP3.h"
#include "ICPCost3.h"
#include "pointSetFileIO.h"


int main(int argc, char **argv ) 
{
  if( argc < 5 )
    {
      std::cerr << "Arguments Missing. " << std::endl;
      std::cerr << "Usage:  "<<argv[0]<<" fixedPointsFile  movingPointsFile finalMovingPointsFile lambda\n";
      exit(-1);
    }


  const char* fixedSetFileName = argv[1];
  const char* movingSetFileName = argv[2];
  const char* finalMovingSetFileName = argv[3];
  double lambda = atof(argv[4]);

  PointSet3DType* fixedPointSet  = readPointSetFromTextFile3D(fixedSetFileName);
  savePointSetToTextFile3D(fixedPointSet, "f.txt");

  PointSet3DType* movingPointSet = readPointSetFromTextFile3D(movingSetFileName);
  savePointSetToTextFile3D(movingPointSet, "m.txt");
  //  std::cout<<"--------------------------------------------------"<<std::endl;


  ICP3 icp(fixedPointSet, movingPointSet);

  icp.setMaxFunctionEvaluationInOptimization(1000);

  icp.setLambda(lambda);

  // here we go  

  // begin timing
  double tic, toc;  //timing varaibles
  tic = clock();


  icp.gogogo();

  toc = clock();
  tic = (toc - tic)/CLOCKS_PER_SEC;
  std::cout<<"Ellapsed time for icp = "<<tic<<std::endl;  
  // end timing




  VnlDoubleVectorType finalParameters = icp.getParameters();
  std::cout << "Solution = "<<finalParameters<<std::endl;  

  std::cout<<"finale cost = "<<icp.getCost()<<std::endl;


  PointSet3DType* finalMovingSet = icp.getNewMovingPointSet();
  savePointSetToTextFile3D(finalMovingSet, finalMovingSetFileName);

  delete finalMovingSet;



  return EXIT_SUCCESS;
}

