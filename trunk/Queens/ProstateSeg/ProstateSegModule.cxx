#include "ProstateSegCLP.h"

#include "randomWalkSeg.h"
#include "cVolOp.h"

#include <ctime>
#include <string>


void getSeedVolume(cVolume *vol);


int main( int argc, char * argv [] )
{
   PARSE_ARGS; 

  cVolOp op;

  //////
  double t1, t2;
  t1 = clock();

  std::cout<<"reading volume...."<<std::flush;  
  cVolume *vol = op.volreadDouble( originalImageFileName.c_str() );
  //cVolume *volSmth = op.gheSmooth( vol, 10);
  // op.volwriteDouble( "smth.nrrd", vol );
  //delete vol; vol = 0;
  std::cout<<"done\n";

  std::cout<<"reading seed...."<<std::flush;
  cVolume *seedVol = op.volreadDouble( seedImageFileName.c_str() );
  //  getSeedVolume(seedVol);
  std::cout<<"done\n"<<std::flush;

  randomWalkSeg rw;

  //  rw.setVol(volSmth);
  rw.setVol(vol);
  rw.setSeedVol(seedVol);

  int numIter = 1000;
  rw.set_solver_num_iter(numIter);

  double solverErrTol = 1e-8;
  rw.set_solver_err_tol(solverErrTol);

  rw.setBeta(beta);
  // std::cout<<"beta value is "<<rw_beta<<std::endl<<std::flush;

  std::cout<<"computing result...."<<std::flush;
  rw.update();
  std::cout<<"done\n"<<std::flush;

  std::cout<<"smoothing result potential volume...."<<std::flush;
  cVolume *potential_volume = rw.get_output();
  cVolume* potential_volume_smth = op.medianFilter(  potential_volume, 3, 3, 3);
  delete potential_volume; potential_volume = 0;
  std::cout<<"done\n"<<std::flush;

  std::cout<<"writing result volume...."<<std::flush;
//   int finaln = (potential_volume_smth->getSizeX())*(potential_volume_smth->getSizeY())*(potential_volume_smth->getSizeZ());
//   for (int i = 0; i <= finaln-1; ++i)
//     {
//       potential_volume_smth->setVoxel(i, 200*potential_volume_smth->getVoxel(i));
//     }

  double thld = 0.5;
  cVolume *bin = op.hardThreshold(potential_volume_smth, thld);
  delete potential_volume_smth; potential_volume_smth = 0;


  bin->setSpacing(vol->getSpacingX(), vol->getSpacingY(), vol->getSpacingZ() );

  op.volwriteDouble( segmentedImageFileName.c_str(), bin );
  delete bin; bin = 0;
  std::cout<<"done\n"<<std::flush;

  t2 = clock();
  t1 = (t2 - t1)/CLOCKS_PER_SEC;
  std::cout<<"New Ellapsed time = "<<t1<<std::endl<<std::flush;
  // end timing
  ////////////////////////////////////////////////////////////

  //delete volSmth; volSmth = 0;
  delete seedVol; seedVol = 0;


  return EXIT_SUCCESS ;
}


void getSeedVolume(cVolume *vol)
{
  /*
    0 --> 1
    1 --> 0
    2 --> 2
   */

  size_t nx = vol->getSizeX();
  size_t ny = vol->getSizeY();
  size_t nz = vol->getSizeZ();

  size_t n = nx*ny*nz;

  for (size_t i = 0; i < n; ++i)
    {
      double v = (*vol)[i];

      if ( 0.9999 <= v && v <= 1.00001 )
        {
          (*vol)[i] = 0;
        }

      if ( -0.0001 <= v && v <= 0.00001 )
        {
          (*vol)[i] = 1;
        }
    }
}
