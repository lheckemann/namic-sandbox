#include "ProstateSegCLP.h"

#include "randomWalkSeg.h"
#include "cVolOp.h"

#include <ctime>
#include <string>

// Description:
// This function gives progress feedback to slicer.
//  percentComplete=0 means 0% complete
//  percentComplete=100 means 100% complete
void progress_callback(int percentComplete)
{
  /*
  // TODO: report these information as well to Slicer
        std::cout << "<filter-start>"
                  << std::endl;
        std::cout << "<filter-name>"
                  << (this->Watcher->GetProcess()
                      ? this->Watcher->GetProcess()->GetClassName() : "None")
                  << "</filter-name>"
                  << std::endl;
        std::cout << "<filter-comment>"
                  << " \"" << this->Watcher->GetComment() << "\" "
                  << "</filter-comment>"
                  << std::endl;
        std::cout << "</filter-start>"
                  << std::endl;
        std::cout << std::flush;
  */
  std::cout << "<filter-progress>" << double(percentComplete)/100.0 << "</filter-progress>" << std::endl;
}

int main( int argc, char * argv [] )
{
   PARSE_ARGS; 

  cVolOp op;

  //////
  double t1, t2;
  t1 = clock();

  std::cout<<"reading volume...."<<std::flush;  
  cVolume *vol = op.volreadDouble( originalImageFileName.c_str() );
  std::cout<<"done\n";

  std::cout<<"reading seed...."<<std::flush;
  cVolume *seedVol = op.volreadDouble( seedImageFileName.c_str() );  
  std::cout<<"done\n"<<std::flush;

  randomWalkSeg rw;
  rw.setLabelValues(1.0/*background*/, 0.0 /*unmarked*/, 2.0 /*object*/);
  rw.setProgressCallback(&progress_callback);
  rw.setVol(vol);
  rw.setSeedVol(seedVol);

  int numIter = 1000;
  rw.set_solver_num_iter(numIter);

  double solverErrTol = 1e-5;
  rw.set_solver_err_tol(solverErrTol);

  rw.setBeta(beta);
  // std::cout<<"beta value is "<<rw_beta<<std::endl<<std::flush;

  std::cout<<"computing result...."<<std::flush;
  rw.update();
  std::cout<<"done\n"<<std::flush;

  cVolume *potential_volume = rw.get_output();

  /* ==================================================
     median smooth and threshold   */

  std::cout<<"smoothing result potential volume...."<<std::flush;
  cVolume* potential_volume_smth = op.medianFilter(  potential_volume, 3, 3, 3);
  delete potential_volume; potential_volume = 0;
  std::cout<<"done\n"<<std::flush;

  double thld = 0.5;

  cVolume *bin = op.hardThreshold(potential_volume_smth, thld);
  delete potential_volume_smth; potential_volume_smth = 0;
  /* median smooth and threshold   
     ==================================================  */



//   /*==================================================
//     threshold 
//    */
//   double thld = 0.5;

//   cVolume *bin = op.hardThreshold(potential_volume, thld);
//   delete potential_volume; potential_volume = 0;
//   /* threshold   
//      ==================================================  */


  std::cout<<"writing result volume...."<<std::flush;
  bin->setSpacing(vol->getSpacingX(), vol->getSpacingY(), vol->getSpacingZ() );
  bin->setOrigin(vol->getOriginX(), vol->getOriginY(), vol->getOriginZ() );


  op.volwriteUchar( segmentedImageFileName.c_str(), bin );
  delete bin; bin = 0;
  std::cout<<"done\n"<<std::flush;

  t2 = clock();
  t1 = (t2 - t1)/CLOCKS_PER_SEC;
  std::cout<<"New Elapsed time = "<<t1<<std::endl<<std::flush;
  // end timing
  ////////////////////////////////////////////////////////////

  //delete volSmth; volSmth = 0;
  delete seedVol; seedVol = 0;


  return EXIT_SUCCESS ;
}
