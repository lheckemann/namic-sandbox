#define DEBUGing

#include "itkSWaveletTest.h"

#include <algorithm>
#include <fstream>



int main( int argc, char * argv [] )
  //itkIcosahedronMeshSourceTest(int, char* [] )
{

//   if (argc != 2) {
//     cerr<<"format: regularShpereMesh levelOfSubdivision"<<endl;
//     exit(-1);
//   }

  SphereMeshSourceType::Pointer  mySphereMeshSource = SphereMeshSourceType::New();

  PointType center; 
  center.Fill( 0.0 );

  VectorType scale;
  scale.Fill( 1.0 );
  
  int n = atoi(argv[1]);
  if (n<=5 && n>=0) {
    mySphereMeshSource->SetCenter( center );
    mySphereMeshSource->SetResolution( n );
    mySphereMeshSource->SetScale( scale );

    mySphereMeshSource->Modified();

    try
      {
        mySphereMeshSource->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
        std::cerr << "Error during Update() " << std::endl;
        std::cerr << excp << std::endl;
      }

    //        std::cout << "mySphereMeshSource: " << mySphereMeshSource;

    std::vector< PointType > v = mySphereMeshSource->GetVerts();
    std::vector< PointType >::const_iterator itv = v.begin();
    std::vector< PointType >::const_iterator itvEnd = v.end();

    std::vector< double > f( v.size() );
    std::vector< double >::iterator itf = f.begin();

    for ( ; itv != itvEnd; ++itv, ++itf ) {
      *itf = (*itv)[2];
    }

    mySphereMeshSource->SetScalarFunction( f );

    mySphereMeshSource->SWaveletTransform();

    mySphereMeshSource->inverseSWaveletTransform();

    std::vector< double > reF =  mySphereMeshSource->GetReconstructedScalarFunction();

    MeshType::Pointer myMesh = mySphereMeshSource->GetOutput();


//     PointType  pt;
//     cerr<<"# of vertics = "<<myMesh->GetNumberOfPoints()<<endl;
//     cerr<<"# of faces = "<<myMesh->GetNumberOfCells()<<endl;
//     cerr<<"# of edges = V + F -2 = "<<myMesh->GetNumberOfCells() + myMesh->GetNumberOfPoints() -2 <<endl;

    std::cerr<<std::endl;
    std::cerr<<std::endl;
    std::cerr<<"resulotion is: "<<mySphereMeshSource->GetResolution()<<std::endl;
    std::cerr<<"size of scalar function: "<<f.size()<<std::endl;
    std::cerr<<"size of reconstructed scalar function: "<<reF.size()<<std::endl;


    std::vector< double > dif(f.size());
    std::vector< double >::iterator itDif=dif.begin(), itDifEnd = dif.end(), itReF=reF.begin();
    std::vector< double >::iterator itfEnd = f.end();

    std::ofstream file("error.dat");        
    double d = 0;
    for ( itf = f.begin(); itf != itfEnd; ++itf, ++itReF ) {
      d = d>fabs(*itf-*itReF)?d:fabs(*itf-*itReF);
      file<<*itf<<"\t\t\t"<<*itReF<<"\t\t\t"<<fabs(*itf-*itReF)<<std::endl;
    }

    std::cerr<<std::endl;
    std::cerr<<"The largest abs error is:..... "<<d<<std::endl;
    std::cerr<<std::endl;


//     for (itf = f.begin(); itDif != itDifEnd; ++itDif, ++itReF) {
//       *itDif = fabs(*itf - *itReF);
//       std::cerr<<*itReF<<"\t";
//     }

//     itf = std::max_element(f.begin(), f.end());
//     itReF = std::max_element(reF.begin(), reF.end());
//     itDif = std::max_element(dif.begin(), dif.end());
//     std::cerr<<"max value of ORIGINAL function= "<<*itf<<std::endl;
//     std::cerr<<"max value of RECONSTRUCTED function= "<<*itReF<<std::endl;
//     std::cerr<<"max abs diff= "<<*itDif<<std::endl;
//     std::cerr<<std::endl;
//     std::cerr<<std::endl;


    //    std::cout << "Testing itk::IcosahedronMeshSource "<< std::endl;

//     for(unsigned int i=0; i<myMesh->GetNumberOfPoints(); i++) 
//       {
//         myMesh->GetPoint(i, &pt);
//         std::cout << "Point[" << i << "]: " << pt << std::endl;
//       }

//     std::cout << "Test End "<< std::endl;

//    displayITKmesh(myMesh);

    return EXIT_SUCCESS;
  }
  else {
    std::cout<<"Parameter should be within {1,2,3,4,5}"<<std::endl;
  }

}

