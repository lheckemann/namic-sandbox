/* wall clock */
/* see "Software Optimization for High Performance Computing" p. 135 */

#include <sys/time.h>

double walltime( double *t0 )
{
  double mic, time;
  double mega = 0.000001;
  struct timeval tp;
  struct timezone tzp;
  static long base_sec = 0;
  static long base_usec = 0;

  (void) gettimeofday(&tp,&tzp);
  if (base_sec == 0)
    {
      base_sec = tp.tv_sec;
      base_usec = tp.tv_usec;
    }

  time = (double) (tp.tv_sec - base_sec);
  mic = (double) (tp.tv_usec - base_usec);
  time = (time + mic * mega) - *t0;

  return(time);
}

/* Here give an example for usage:

int main(int argc, char** argv)
{
  double timeZero = 0.0;
  double startTime = 0.0;
  double ellapseTime = 0.0;

  startTime = walltime(&timeZero); // this is necessary
  
  // do the job want to timing

  ellapseTime = walltime(&startTime); 

  std::cout<<"wall time = "<<ellapseTime<<std::endl;

  return 0;
}
*/
