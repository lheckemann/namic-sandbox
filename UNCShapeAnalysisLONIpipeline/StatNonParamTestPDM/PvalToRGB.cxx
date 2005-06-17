
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <cstdio>

#include "argio.h"

using namespace std;

int main (int argc, const char ** argv) 
{
  if (argc <=1 || ipExistsArgument(argv, "-usage")) {
    cout << argv[0] << endl;
    cout << " reads in a file with a white-space (space, tab, CR, LF) separated list of Pvalues and write a 'comma' separated list of RGB values(each space separated)" << endl;
    cout << " Colors assigned: non-significant (> 0.05) -> blue" << endl;
    cout << " Colors assigned: significant -> from red (0.001) to green (0.05)" << endl;
    cout << "usage:  infile [-o outfile] [-signLevel <alpha>] [-minSignLevel <minVal>] [-iv]"<< endl;
    cout << endl;
    cout << "infile      input data set " << endl;
    cout << "-out outfile  output data set, if omitted then 'output_def.colors'" << endl;
    cout << "" << endl;
    cout << endl << endl;
    exit(0);
  }

  char *infile     = strdup(argv[1]);
  char *outfile    = ipGetStringArgument(argv, "-out", "output_def.colors");

  if (!infile) {
    cerr << " no input file specified " << endl;exit(1);
    }
  const double significanceLevel = ipGetDoubleArgument(argv, "-signLevel", 0.05);
  const double significanceMinLevel = ipGetDoubleArgument(argv, "-minSignLevel", 0.001);
  double signDiff =  (significanceLevel - significanceMinLevel)/2;
  bool doOIVstyle = ipExistsArgument(argv, "-iv");

  FILE * infile_file = fopen(infile,"r");
  if (!infile_file) {
    cerr << " Cannot open file " << infile_file << endl;
    exit (-1);
  }
  ofstream efile(outfile, ios::out); 

  const int GATHER_ITEMS = 10;
  int numFeatures = 0;
  double curPval;
  float colorR, colorG, colorB;
  int first = 1;

  if (doOIVstyle) {
    efile << " Material { diffuseColor [ " << endl;
  }
  while (fscanf(infile_file," %lf ", &curPval) > 0) {
    if (first) {
      first = 0;
    } else {
      efile << ",";
      if (numFeatures % GATHER_ITEMS == 0 ) efile << endl;
      else efile << " ";
    }     
    colorR = 0; colorG = 0; colorB = 0;
    if (curPval > significanceLevel) {
      colorR = 0; colorG = 0; colorB = 1;
    } else if (curPval < significanceMinLevel){
      colorR = 1; colorG = 0; colorB = 0;
    } else if (curPval <  significanceMinLevel + signDiff ) {
      double part = (curPval - significanceMinLevel)/signDiff;
      colorR = 1; colorG = part; colorB = 0;
    } else {
      double part = (curPval - significanceMinLevel - signDiff)/signDiff;
      colorR = 1-part; colorG = 1; colorB = 0;
    }
    efile << colorR << " " << colorG  << " " << colorB ;
    numFeatures++;
  }
  cout << "numFeatures: " << numFeatures  << endl;
  fclose(infile_file);
  efile << endl;
  if (doOIVstyle) {
    efile << " ]  }" << endl;
    efile << " MaterialBinding { value PER_VERTEX_INDEXED }" << endl;
  }
  efile.close();
}
