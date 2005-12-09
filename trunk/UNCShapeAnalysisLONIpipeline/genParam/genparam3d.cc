


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <malloc.h>

#include <iostream>
#include <fstream>
#include <string>

#include "par++/iterator.hh"

#include "argio.h"

char * progname = "genParamFromNet";

int main(int argc, const char *argv[])
{
  int i, j, dim;
  double *xvec, *a;
  Net net;
  Parameter par;
  FILE *file;
 
  // read parameter

  if (argc <2 || ipExistsArgument(argv, "-usage")) {
    std::cout << "genParamFromNet 1.5 version " << std::endl;
    std::cout << "usage: genParamFromNet <innet_file> <outpar_file> [options] "<< std::endl;
    std::cout << "  innet_file   is a surface net file that was extracted using surfnetExtract" << std::endl;
    std::cout << "  outpar_file  stores the computed parametrization" << std::endl;
    std::cout << "  following options are possible (defauls in brackets)" << std::endl;
    std::cout << "    -iterations [500]  maximal number of iterations" << std::endl;
    std::cout << "    -error [1e-8]      bailout-limit for error" << std::endl;
    std::cout << "    -v                 verbose mode (-print_itn # print step at each # iterations)" << std::endl;
    std::cout << "    -inpar inpar_file  use param in inpar_file to initialize the parametrization" << std::endl;
    std::cout << "    there are indeed other parameters but they should not be changed" << std::endl;

    exit(0);
  }
  char *filename = strdup(argv[1]);
  char *out_file = strdup(argv[2]);
  char *inpar_file = ipGetStringArgument(argv,"-inpar",NULL);
  
  int iterations = ipGetIntArgument(argv, "-iterations",500);
  double err     = ipGetDoubleArgument(argv, "-error",1e-8);
  bool verbose   = ipExistsArgument(argv, "-v");

  par.max_active = ipGetIntArgument(argv, "-max_active",1000);
  par.print_itn  = ipGetIntArgument(argv, "-print_itn",-2);
  par.delta      = ipGetDoubleArgument(argv, "-delta",3e-7);
  par.constr_tol = ipGetDoubleArgument(argv, "-constr_tol",1e-3);
  par.line_tol   = ipGetDoubleArgument(argv, "-line_tol",1e-5);
  par.ineq_low   = ipGetDoubleArgument(argv, "-ineq_low",1e-7);
  par.ineq_init  = ipGetDoubleArgument(argv, "-ineq_init",1e-2);
  par.ineq_final = ipGetDoubleArgument(argv, "-ineq_final",1e-6);
  par.ineq_slack = ipGetDoubleArgument(argv, "-ineq_slack",2.0);
  par.newton_tol = ipGetDoubleArgument(argv, "-newton_tol",1e-4);
  par.rho_init   = ipGetDoubleArgument(argv, "-rho_init",1);
  par.c0rho      = ipGetDoubleArgument(argv, "-c0rho",1);
  par.c1rho      = ipGetDoubleArgument(argv, "-c1rho",0.25);
  par.c2rho      = ipGetDoubleArgument(argv, "-c2rho",0.012);
  par.rho_limit  = ipGetDoubleArgument(argv, "-rho_limit",3e-2);
  par.step_small = ipGetDoubleArgument(argv, "-step_small",0.5);
  par.step_large = ipGetDoubleArgument(argv, "-step_large",1.0);


  // read net
  int  status;
  FILE  *stream;

  if (!(stream= fopen(filename, "r"))) {
    std::cout << "cannot read input net file " << filename << std::endl; 
    exit (-1); 
  }
  if (status= read_net(stream, &net)) {
    fprintf(stderr, "read_net exited with %d at position %d in file '%s'\n",
      status, ftell(stream), filename);
    exit(status);
  }
  fclose(stream);

  int euler_num = net.nvert-net.nface;
  
  if (euler_num != 2) {
    std::cout << "Euler equation not satisfied. Euler Number" << euler_num << std::endl;
    return -1;   
  }

  double error = 1000000.0;
  int cnt = 0, percent, fd;
  char state[100];
  if (!verbose) {
    /* set stderr to /dev/null */
    close(STDERR_FILENO);
    fd = open("/dev/null",O_RDWR);
  } else {
    std::cout << "verbose mode" << std::endl;
  }

  NewtonIterator * optim = NULL;
  dim= net.nvert;
  xvec= new double[3*dim];

  if (inpar_file) {
    // scan file for existing parameterization
    if (verbose) std::cout << "Reading initial parametrization" << std::endl;
    file = fopen(inpar_file , "r");
    if (!file) {
      std::cout << "Error: open of file failed." << inpar_file  << std::endl;
      return (0);
    }
    char line[1024];
    char * state = fgets(line, 1023, file);
    while (state && !strstr(line,"vertList=")) {
      state = fgets(line, 1023, file);
    }
    state = fgets(line, 1023, file); //AppendTo[vertList, {\n
    int cnt = 0;
    for (i= 0; i < dim; i++) {
      state = fgets(line, 1023, file);
      if (state) {
  int numread = sscanf(line," { %lf , %lf , %lf ",&(xvec[3*i]), &(xvec[3*i + 1]), &(xvec[3*i + 2]));
  cnt = cnt + numread;
      }
    }
    fclose(file); 
    if (verbose) std::cout << dim << "," << cnt/3 << "," << cnt<< std::endl;
    // initialize optimization with this parametrization
    optim = new NewtonIterator (net, par, xvec);
  } else {
    optim = new NewtonIterator (net, par);
  }
  

  while (cnt < iterations && error > err) {
    cnt++;
    error = optim->iterate();
    if (cnt % 100 == 0 || cnt == 1) {
       optim->get_solution(xvec);
       std::cout << "writing intermediate result at " << cnt << " Iterations" << std::endl;
       file = fopen(out_file , "w");
       if (!file) {
   std::cout << "Error: open of file failed." << out_file << std::endl;
       return (0);
       } 
       fprintf(file,"faces = 1 + { \n");  
       for (i = 0; i < net.nface; i++) {               
       /* debug output for checking */
       fprintf(file,"{");
       for (int j= 0; j < 3; j++)
         fprintf(file,"%d, ", net.face[4*i+j]);
       fprintf(file,"%d}" , net.face[4*i+3]);
       if (i < net.nface -1) fprintf(file,",\n");
       else fprintf(file,"};\n");
       }
       fprintf(file,"vertList= {};\nAppendTo[vertList, {\n");
       for (i= 0; i < dim; i++) {
       fprintf(file,"{%lf, %lf, %lf}",xvec[3*i], xvec[3*i + 1],xvec[3*i + 2]);
       if (i < dim-1) fprintf(file,",\n");
       else fprintf(file,"}];\n");
       }
       fclose(file);
       
       char outfile_pts[500];
       sprintf(outfile_pts,"%s.pts",out_file);
       
       // pts
       file = fopen(outfile_pts , "w");
       if (!file) {
   std::cout << "Error: open of file failed." << outfile_pts << std::endl;
       return (0);
       }
       fprintf(file,"%d \n", dim);
       for (i= 0; i < dim; i++) {
       //vert
       fprintf(file,"%lf %lf %lf \n", xvec[3*i], xvec[3*i + 1],xvec[3*i + 2]);
       }
       fclose(file);  
    }


  }
  if (error <= err)
    sprintf (state,"Opt.o.k. it=%d", cnt);
  else
    sprintf (state,"Opt.err. %.2e",error);

  std::cout << "State: " << state << std::endl;

  optim->get_solution(xvec);
  // pro vertex 3 original and 3 projected coordinates x, y, z
  if (verbose)
    std::cout << "writing output"  << std::endl;

  file = fopen(out_file , "w");
  if (!file) {
    std::cout << "Error: open of file failed." << out_file << std::endl;
    return (0);
  }

  fprintf(file,"faces = 1 + { \n");  
  for (i = 0; i < net.nface; i++) {    
    /* debug output for checking */
    fprintf(file,"{");
    for (int j= 0; j < 3; j++)
      fprintf(file,"%d, ", net.face[4*i+j]);
    fprintf(file,"%d}" , net.face[4*i+3]);
    if (i < net.nface -1) fprintf(file,",\n");
    else fprintf(file,"};\n");
  }
  fprintf(file,"vertList= {};\nAppendTo[vertList, {\n");
  for (i= 0; i < dim; i++) {
    fprintf(file,"{%lf, %lf, %lf}",xvec[3*i], xvec[3*i + 1],xvec[3*i + 2]);
    if (i < dim-1) fprintf(file,",\n");
    else fprintf(file,"}];\n");
  }
  fclose(file);

  char outfile_pts[500];
  sprintf(outfile_pts,"%s.pts",out_file);

  // pts
  file = fopen(outfile_pts , "w");
  if (!file) {
    std::cout << "Error: open of file failed." << outfile_pts << std::endl;
    return (0);
  }
  fprintf(file,"%d \n", dim);
  for (i= 0; i < dim; i++) {
    //vert
    fprintf(file,"%lf %lf %lf \n", xvec[3*i], xvec[3*i + 1],xvec[3*i + 2]);
  }
  fclose(file);
  if (verbose)
    std::cout << "writing finished"  << std::endl;

  char outfile_iv[500];
  sprintf(outfile_iv,"%s.iv",out_file);
  FILE *dfile = fopen(outfile_iv, "w");
  if (!dfile) {
    std::cout << "Could not open input file " << outfile_iv << std::endl;
     return -1;
  }
  // Prepare the destination file for iv format
  fprintf(dfile,"#Inventor V2.1 ascii \n \n");
  // the coordinates
  fprintf(dfile,"Separator { \n ");
  fprintf(dfile," Material { diffuseColor [ 0 0 1 ] } \n");
  fprintf(dfile,"        Coordinate3 { \n");
  fprintf(dfile,"               point [ \n");
  for (i= 0; i < dim; i++) {
    //vert
    fprintf(file,"%lf %lf %lf \n", xvec[3*i], xvec[3*i + 1],xvec[3*i + 2]);
  }
  fprintf(dfile,"               ] \n");// close of point
  fprintf(dfile,"             } \n"); // close of Coordinate3

  // the triangles
  fprintf(dfile,"        IndexedTriangleStripSet { \n");
  fprintf(dfile,"               coordIndex[\n");
  for (i = 0; i < net.nface; i++) {
    //first triangle
    fprintf(dfile,"%d, ", net.face[4*i+0]);
    fprintf(dfile,"%d, ", net.face[4*i+1]);
    fprintf(dfile,"%d, -1,\n", net.face[4*i+2]);
    // second triangle
    fprintf(dfile,"%d, ", net.face[4*i+2]);
    fprintf(dfile,"%d, " , net.face[4*i+3]);
    fprintf(dfile,"%d, -1,\n", net.face[4*i+0]);
  }
  fprintf(dfile,"              ]\n"); // coordIndex  
  fprintf(dfile,"        }\n "); // IndexedTriangleStripSet
  fprintf(dfile,"       }\n"); // Separator
  fprintf(dfile,"\n"); // empty line



  //delete [] net.vert;
  //delete [] net.face;
  //delete [] xvec;

  return 0;
}
