/* wall clock */
/* see "Software Optimization for High Performance Computing" p. 135 */

double walltime( double *t0 );


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
