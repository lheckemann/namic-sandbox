
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
    cout << " reads in a file with a white-space (space, tab, CR, LF) separated list of distance values "
      << " or any other scalar, real valued measuremnt " 
      << " and write a 'comma' separated list of RGB values(each space separated)" << endl
      << " for better readability in case of red/green blindness, the color green is omitted" << endl;
    cout << " Colors assigned:  from blue (min) to red (blue) " << endl;
    cout << "usage:  infile [-o outfile] [-min <min>] [-max <max>] [-iv] [-zeroSym] [-midWhite | -midYellow | -midPink] "<< endl;
    cout << endl;
    cout << "infile      input data set " << endl;
    cout << "-out outfile  output data set, if omitted then 'output_def.colors'" << endl;
    cout << "-iv           add additional pre- and post-ample to include in OpenInventor files" << endl;
    cout << "-zeroSym      min and max will be adjusted to be symmetric 0 -> min = -max " << endl;
    cout << "-midWhite | -midYellow | -midPink  selection of intermediate color" << endl;
    cout << "" << endl;
    cout << endl << endl;
    exit(0);
  }

  char *infile     = strdup(argv[1]);
  char *outfile    = ipGetStringArgument(argv, "-out", "output_def.colors");
  double curPval;
  bool doOIVstyle = ipExistsArgument(argv, "-iv");
  bool doMidWhite = ipExistsArgument(argv, "-midWhite");
  bool doMidPink = ipExistsArgument(argv, "-midPink");
  bool doMidYellow = ipExistsArgument(argv, "-midYellow");
  if (!doMidYellow && !doMidPink && !doMidWhite) doMidPink = true;

  if (!infile) {
    cerr << " no input file specified " << endl;exit(1);
    }
  FILE * infile_file = fopen(infile,"r");
  if (!infile_file) {
    cerr << " Cannot open file " << infile_file << endl;
    exit (-1);
  }
  double min = 10000000, max = -1000000;
  while (fscanf(infile_file," %lf ", &curPval) > 0) {
    if (curPval < min) min = curPval;
    if (curPval > max) max = curPval;
  }

  double RealMin= ipGetDoubleArgument(argv, "-min", min);
  double RealMax = ipGetDoubleArgument(argv, "-max", max);

  cout << "Min: " << RealMin << " , max: " << RealMax << endl;

  bool zeroSymFlag = ipExistsArgument(argv, "-zeroSym");

  if (zeroSymFlag) {
    if (fabs(RealMin) > fabs(RealMax)) { RealMax = - RealMin; }
    else { RealMin = - RealMax;}
  }

  double halfval =  (RealMax - RealMin)/2;
  const int GATHER_ITEMS = 10;
  int numFeatures = 0;
  float colorR, colorG, colorB;
  int first = 1;

  ofstream efile(outfile, ios::out); 

  rewind(infile_file);
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
    if (curPval < RealMin) {
      colorB = 1; 
    } else if (curPval > RealMax){
      colorR = 1;
    } else if (curPval <  halfval + RealMin ) {
      double part = (curPval - RealMin)/halfval;
      if (doMidPink) {
     colorR = part; colorG = 0; colorB = 1;
      } else if (doMidWhite) {
     colorR = part; colorG = part; colorB = 1;
      } else if (doMidYellow) {
     colorR = part; colorG = part; colorB = 1-part;
      }
    } else {
      double part = (curPval - RealMin - halfval)/halfval;
      if (doMidPink) {
     colorR = 1; colorG = 0; colorB = 1-part;
      } else if (doMidWhite) {
     colorR = 1; colorG = 1-part; colorB = 1-part;
      } else if (doMidYellow) {
     colorR = 1; colorG = 1-part; colorB = 0;
      }
    }
    efile << colorR << " " << colorG  << " " << colorB ;
    numFeatures++;
  }
  //cout << "numFeatures: " << numFeatures  << endl;
  fclose(infile_file);
  efile << endl;
  if (doOIVstyle) {
    efile << " ]  }" << endl;
    efile << " MaterialBinding { value PER_VERTEX_INDEXED }" << endl;
  }
  efile.close();
}
