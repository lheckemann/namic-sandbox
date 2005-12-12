#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>

#include "computeStatPDM.h"
#include "argio.h"

using namespace std;


void
load_simpleStat_file(char * filename, int featSelStart, 
           int featSelLen, int featgroupID,
           int &numSubjects, int &numFeatures,
           int * &groupLabel, double * &featureValue) 
{
  const int MAXLINE  = 10 * 50000;  // maximal 50'000 entries each 10 digits long
  static char line [ MAXLINE ];
  //int subjIDindex = 0;
  //int groupNameindex = 1;
  int groupIDindex = 2;
  int firstDataindex = 3;
  int endDataindex = -1;
  int featureLength = -1;

  if (featgroupID >= 0 ) groupIDindex = featgroupID;
  if (featSelStart >= 0 ) firstDataindex = featSelStart;
  if (featSelLen >= 0 ) featureLength = featSelLen;

  ifstream datafile(filename,ios::in); 
  if (!datafile.is_open()) {
    cerr << "file does not exist" << filename << endl;
    exit(0);
  }

  datafile.getline(line,MAXLINE);
  if (datafile.eof()){
    cerr << "not a single line in file " << filename << endl;
    exit(0);
  }
  
  int cnt = 0;
  numFeatures = 0;
  while (line[cnt] != '\0') {
    // skip delimiters
    while  (line[cnt] != '\0' && 
         ( line[cnt] == ' ' || line[cnt] == ',' || line[cnt] == ';' || 
           line[cnt] == '\t')) {
      cnt++;
    }
    if (line[cnt] != '\0') numFeatures++;
    // skip non-delimiters
    while  (line[cnt] != '\0' && 
         line[cnt] != ' ' && line[cnt] != ',' && line[cnt] != ';' && 
         line[cnt] != '\t') {
      cnt++;
    }
  }
  
  if (featureLength > 0 ) numFeatures = featureLength;
  else numFeatures = numFeatures - firstDataindex;

  endDataindex = firstDataindex + numFeatures - 1;
  cout << "Num Features: " << numFeatures << " from  " << firstDataindex << " to " << endDataindex << endl;

  numSubjects = 0;

  datafile.clear(); 
  datafile.seekg(0, std::ios::beg);
  datafile.getline(line,MAXLINE);
  while (!datafile.eof()){
    numSubjects++;
    datafile.getline(line,MAXLINE);
  }

  cout << "Num Subjects: " << numSubjects << endl;

  groupLabel = new int [ numSubjects ];
  featureValue = new double [ numSubjects * numFeatures ];

  int curLine = 0;
  datafile.clear();
  datafile.seekg(0, std::ios::beg);
  datafile.getline(line,MAXLINE);
  while (!datafile.eof()){
    // scan features per line
    int cnt = 0;
    int curFeature = 0;
    while (line[cnt] != '\0' && curFeature <= endDataindex) {
      // skip delimiters
      while  (line[cnt] != '\0' && 
           ( line[cnt] == ' ' || line[cnt] == ',' || line[cnt] == ';' || 
          line[cnt] == '\t')) {
        cnt++;
      }
      // read in feature
      bool numberRead = false;
      int numChar = 0;
      if (line[cnt] != '\0') {
        if (curFeature == groupIDindex) {
          //scan in groupID
          int label = -1;
          int numRead = sscanf(&(line[cnt]),"%d%n",&label, &numChar);
          if (!numRead || !numChar) { cout << "Error while reading " << endl;}
          groupLabel[curLine] = label;
                        numberRead = true;
        } else if (curFeature >= firstDataindex && curFeature <= endDataindex) {
          //scan in feature
          double feature = -1;
          int numRead = sscanf(&(line[cnt]),"%lf%n",&feature, &numChar);
          if (!numRead || !numChar) { cout << "Error while reading " << endl;}
          int featIndex = curFeature - firstDataindex;
          featureValue[curLine*numFeatures + featIndex] = feature;
                        numberRead = true;
        }
                      cnt = cnt + numChar;
        curFeature++;
      } 
      // skip non-delimiters
      while  (line[cnt] != '\0' && 
           line[cnt] != ' ' && line[cnt] != ',' && line[cnt] != ';' && 
           line[cnt] != '\t') {
                      if (numberRead) { 
                        // we were reading a number and now there are just adjacent to this number other characters -> error
                        // when reading number, then the next character needs to be a delimiter
          cout << "Error while reading number, encountering non-number on line " << curLine 
                             << " feature: " << curFeature << " numChar : " << numChar << endl;
                      }
        cnt++;
      }
    }
    if (curFeature <= endDataindex) { cout << "Not enough features" << endl;}
    // next line
    curLine++;
    datafile.getline(line,MAXLINE);
  }

  datafile.close();

  // change labels to the predefined ones, this will fail if there are more than 2 labels in the file
  int preLabelA = groupLabel[0];
  int preLabelB = groupLabel[0];
  int i;
  for (i = 0; i < numSubjects; i++) {
    if (preLabelA != groupLabel[i]) {
      if (preLabelB != preLabelA && preLabelB  != groupLabel[i]) {
          cout << "Error more than 2 labels in file" << endl;
      } else {
                      preLabelB = groupLabel[i];
      }
    }
  }
  for (i = 0; i < numSubjects; i++) {
    if (preLabelA == groupLabel[i]) {
      groupLabel[i] = GROUP_A_LABEL ;
    } else if (preLabelB == groupLabel[i]) {
      groupLabel[i] = GROUP_B_LABEL ;
    }
  }
  cout << "data has been relabeled: " <<  preLabelA << " --> " << GROUP_A_LABEL
       << " ; " << preLabelB << " --> " << GROUP_B_LABEL << endl;

}


int main (int argc, const char ** argv) 
{
  if (argc <=1 || ipExistsArgument(argv, "-usage")) {
    cout << argv[0] << endl;
    cout << " computing  local statistics corrected for multiple comparisons via non-parametric permutations tests" << endl;
    cout << "  the metric for univariate data is mean difference and for multivariate data Hotelling T^2" << endl;
    cout << "usage:  infile [-out outfile] [-numPerms <i>] [-signLevel <f>] [-signSteps <i>] [-featSelStart <startIndex>] [-featSelLen <length>] [-featgroupID <groupIDIndex>] [-log] [-3DvectorData [-Hotelling] [-meanDiff]] [-maxStat [-norm68] [-noNorm] [-normSTDDEV]] "<< endl;
    cout << endl;
    cout << "infile      input data set " << endl;
    cout << "-out outfile  output base name, if omitted then infile is used" << endl << endl;
    cout << " indexes for featSelStart and -featgroupID begn at 0" << endl << endl;
    cout << "-log        perform log(1+p) transform necessary for magnitude data" << endl;
    cout << "-maxStat    don't do p-value conversion, but compute T-maximum statistics" <<endl;
    cout << "-normSTDDEV do normalization with standard deviation for maxStat [DEFAULT]" << endl;
    cout << "-norm68     do normalization with 68% cutoff for maxStat" <<endl;
    cout << "-noNorm     do not normalize the data for uniform sensitivity" <<endl;
    cout << "-numPerms <i>" << endl;
    cout << "-signLevel <f>" << endl;
    cout << "-signSteps <i>" << endl;
    cout << "-3DvectorData [-Hotelling] [-meanDiff]  3D statistics with Hotelling T^2 or Mean distance metric"<< endl;
    cout << "-2DvectorData [-Hotelling] [-meanDiff]  2D statistics with Hotelling T^2 or Mean distance metric"<< endl;
    cout << "" << endl;
    cout << "" << endl;

    cout << " the uncorrrected p-values will be written out as well if p-val correction is used" << endl;
    cout << " for univariate and multivariate Hotelling, the t/T score as well as Cohen's d is written " << endl;
    cout << endl << endl;
    exit(0);
  }

  char *infile     = strdup(argv[1]);
  char *outbase    = ipGetStringArgument(argv, "-out", NULL);
  if (!infile) {
    cout << " no input file specified " << endl;exit(1);
    }
  if (!outbase) {
    outbase = strdup(infile);
  } 

  const int numPerms = ipGetIntArgument(argv, "-numPerms", 10000);
  const double significanceLevel = ipGetDoubleArgument(argv, "-signLevel", 0.05);
  const int significanceSteps = ipGetIntArgument(argv, "-signSteps", 100);

  const int featSelStart  = ipGetIntArgument(argv, "-featSelStart", -1);
  const int featSelLen  = ipGetIntArgument(argv, "-featSelLen", -1);
  const int featgroupID  = ipGetIntArgument(argv, "-featgroupID", -1);

  const int maxStatOn = ipExistsArgument(argv, "-maxStat");
  const int logOn = ipExistsArgument(argv, "-log");

  int testStatNorm = 1; // stddes normalization
  if (ipExistsArgument(argv, "-norm68")) {
    testStatNorm = 2;
  }

  if (ipExistsArgument(argv, "-noNorm")) {
    testStatNorm = 3;
  }

  if (ipExistsArgument(argv, "-normSTDDEV")) {
    testStatNorm = 1;
  }
  int numSubjects, numFeatures;
  int * groupLabel = NULL;
  double * featureValue = NULL; 

  load_simpleStat_file(infile,featSelStart,featSelLen,featgroupID,numSubjects, 
             numFeatures,groupLabel,featureValue);

  if (logOn) {
    // log transform the Feature data
    for (int subj = 0; subj < numSubjects; subj++) {
      for (int feat = 0; feat < numFeatures; feat++) {
   int subjIndex = subj * numFeatures;
   if (featureValue[subjIndex + feat] < 0) {
     cerr << "Features are negative, and thus do not represent magnitude data" << endl;
     cerr << "Cowardly refusing to log transform data... " << endl;
     exit(-1);
   }
   featureValue[subjIndex + feat] = log (1 + featureValue[subjIndex + feat]);
      }
    }
  }

  int testStatistic = 1;
  int tupel_size = 1;
  if ( ipExistsArgument(argv, "-3DvectorData") ) {
    tupel_size = 3;
    
    if (ipExistsArgument(argv, "-Hotelling")) {
      testStatistic = 2; // Hotelling statistic over 3-tupels (x,y,z coordinates)
    } else if (ipExistsArgument(argv, "-meanDiff")) {
      testStatistic = 1; // absolute value of mean Difference
    }
  } else  if ( ipExistsArgument(argv, "-2DvectorData") ) {
    tupel_size = 2;
    
    if (ipExistsArgument(argv, "-Hotelling")) {
      testStatistic = 2; // Hotelling statistic over 3-tupels (x,y,z coordinates)
    } else if (ipExistsArgument(argv, "-meanDiff")) {
      testStatistic = 1; // absolute value of mean Difference
    }
  } else {
    tupel_size = 1;
    testStatistic = 1; // absolute value of mean Difference
  }

  double * pValue = new double [numFeatures];
  
  static long timeStart = time(NULL);
  
  if (maxStatOn) {
    doTestingMax(numSubjects, numFeatures, numPerms, tupel_size, groupLabel, featureValue,
                                             significanceLevel, significanceSteps, testStatistic, testStatNorm, pValue, outbase);
  } else {
    doTesting(numSubjects, numFeatures, numPerms, tupel_size, groupLabel, featureValue,
                            significanceLevel, significanceSteps, testStatistic, pValue, outbase);
  }
  static long timeEnd = time(NULL);

  cout << "Computation time: " << timeEnd - timeStart << endl;
  return 0; 
}
