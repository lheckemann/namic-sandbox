
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>

#include "computeStatPDM.h"

using namespace std;
 
const int debug = 1;
const int writePvalDEBUG = 1;

void computeGroupMeanDiffStat(int numSubjects, int numFeatures,
                  int * groupLabel, double * featureValue,
                  double * statistic, double &avgStat)
// computes the group difference statistic for the given group Labels 
//
// the group difference statistic is computed as the absolute difference of the means for each
// feature
//
// numSubjects = Number of Subjects Group A + Group B
// numFeatures = Number of scalar features per Subject
// groupLabel = int array [numSubj2ects] with group Labels, Group A = 0, Group B = 1
// featureValue = double array [numSubjects*numFeatures] with scalar feature values for
//   all Subjects e.g. n-th feature of m-th subjects is at featureValue[m * numFeatures + n]
// statistic = double array [numFeatures] output with group difference statistics
//
//
{
    // compute mean for group A and group B for each feature
    double * meanA = new double [numFeatures];
    double * meanB = new double [numFeatures];
    for (int feat = 0; feat < numFeatures; feat++) {
     meanA[feat] = 0;
     meanB[feat] = 0;
    }
    
    int numSubjA = 0;
    int numSubjB = 0;

    for (int subj = 0; subj < numSubjects; subj++) {
     int subjIndex = subj * numFeatures;
     if (groupLabel[subj] == GROUP_A_LABEL) {
         numSubjA++;
         for (int feat = 0; feat < numFeatures; feat++) {
          meanA[feat] = meanA[feat] + featureValue[subjIndex + feat];
         }
     } else if (groupLabel[subj] == GROUP_B_LABEL) {
         numSubjB++;
         for (int feat = 0; feat < numFeatures; feat++) {
          meanB[feat] = meanB[feat] + featureValue[subjIndex + feat];         
         }
     } else {
         cerr << " group label " << groupLabel[subj] << " does not exist" << endl;
     }
    }
    for (int feat = 0; feat < numFeatures; feat++) {
      meanA[feat] = meanA[feat] / numSubjA;
      meanB[feat] = meanB[feat] / numSubjB;
    }

    // absolute difference is statistic
    
    for (int feat = 0; feat < numFeatures; feat++) {
      double val = fabs(meanA[feat] - meanB[feat]);
      statistic[feat] = val;
    }
    avgStat = 0;

    for (int feat = 0; feat < numFeatures; feat++) {
      avgStat += statistic[feat];
    }
    avgStat /= numFeatures;


    delete meanA;
    delete meanB;

}

void computeGroupStudentTDiffStat(int numSubjects, int numFeatures,
                 int * groupLabel, double * featureValue,
                 double * statistic, double &avgStat)
// computes the group difference statistic for the given group Labels using the Student t statistic
//
// the group difference statistic is computed as the absolute difference of the means for each
// feature weighted with their variances (Student t statistic)
//
// numSubjects = Number of Subjects Group A + Group B
// numFeatures = Number of scalar features per Subject
// groupLabel = int array [numSubj2ects] with group Labels, Group A = 0, Group B = 1
// featureValue = double array [numSubjects*numFeatures] with scalar feature values for
//   all Subjects e.g. n-th feature of m-th subjects is at featureValue[m * numFeatures + n]
// statistic = double array [numFeatures] output with group difference statistics
//
//
{
    // compute mean for group A and group B for each feature
    double * meanA = new double [numFeatures];
    double * meanB = new double [numFeatures];
    double * varA = new double [numFeatures];
    double * varB = new double [numFeatures];

    for (int feat = 0; feat < numFeatures; feat++) {
      meanA[feat] = 0;
      meanB[feat] = 0;
      varA[feat] = 0;
      varB[feat] = 0;
    }
    
    int numSubjA = 0;
    int numSubjB = 0;
    
    // compute means
    for (int subj = 0; subj < numSubjects; subj++) {
     int subjIndex = subj * numFeatures;
     if (groupLabel[subj] == GROUP_A_LABEL) {
         numSubjA++;
         for (int feat = 0; feat < numFeatures; feat++) {
          meanA[feat] = meanA[feat] + featureValue[subjIndex + feat];
         }
     } else if (groupLabel[subj] == GROUP_B_LABEL) {
         numSubjB++;
         for (int feat = 0; feat < numFeatures; feat++) {
          meanB[feat] = meanB[feat] + featureValue[subjIndex + feat];         
         }
     } else {
         cerr << " group label " << groupLabel[subj] << " does not exist" << endl;
     }
    }
    for (int feat = 0; feat < numFeatures; feat++) {
     meanA[feat] = meanA[feat] / numSubjA;
     meanB[feat] = meanB[feat] / numSubjB;
    }
    // compute variances
    for (int subj = 0; subj < numSubjects; subj++) {
     int subjIndex = subj * numFeatures;
     double diff;
     if (groupLabel[subj] == GROUP_A_LABEL) {
         for (int feat = 0; feat < numFeatures; feat++) {
                         diff = featureValue[subjIndex + feat] - meanA[feat];
                         varA[feat] = varA[feat] + diff * diff;
         }
     } else if (groupLabel[subj] == GROUP_B_LABEL) {
         for (int feat = 0; feat < numFeatures; feat++) {
                         diff = featureValue[subjIndex + feat] - meanB[feat];
                         varB[feat] = varB[feat] + diff*diff;         
         }
     } else {
         cerr << " group label " << groupLabel[subj] << " does not exist" << endl;
     }
    }
    // varA = sigma_a^2 * (n_a - 1)
    // varB = sigma_b^2 * (n_b - 1)

    // absolute difference is statistic

    for (int feat = 0; feat < numFeatures; feat++) {
      double val = fabs(meanA[feat] - meanB[feat]) / sqrt( (varA[feat] + varB[feat]) * ( numSubjA  + numSubjB) 
                        / numSubjA / numSubjB / (numSubjA + numSubjB - 2)) ;
      statistic[feat] = val;
    }
    avgStat = 0;
    for (int feat = 0; feat < numFeatures; feat++) {
      avgStat += statistic[feat];
    }
    avgStat /= numFeatures;

    delete meanA;
    delete meanB;
    delete varA;
    delete varB;

}

int doTesting(int numSubjects, int numFeatures, int numPerms, int tupelSize,
           int * groupLabel, double * featureValue,
           double significanceLevel, int significanceSteps,
           double * pValue, char *outbase)
// computes the local pValues for each feature according to non-parametric permutation tests
//
// numSubjects = Number of Subjects Group A + Group B
// numFeatures = Number of Scalar features per Subject
// numPerms    = Number of Permutations used in computation
// tupelSize   = Numbers of features grouped in tupels 
// groupLabel = int array [numSubjects] with group Labels, Group A = 0, Group B = 1
// featureValue = double array [numSubjects*numFeatures] with scalar feature values for
//   all Subjects e.g. n-th feature of m-th subjects is at featureValue[m * numFeatures + n]
// significanceLevel = level of significance (usually 0.05 or 0.01)
// significanceSteps = number of significance steps
// pValue = double array [numFeatures] output with p-values at each feature location
//          the pValue is == 1 for all features with pValue greater than significanceLevel
//          for features with pValue less than significanceLevel, the actual p-values are 
//          computed in step sizes of significanceLevel/significanceSteps; 
//  e.g. significanceValue = 0.05, significanceSteps = 10; so, the possible p-values are:
//    1 (non-significant), 0.05, 0.045, 0.040, 0.035, 0.030, 0.025, 0.020, 0.015, 0.010, 0.005
//
{
    int numSubjA, numSubjB;
    ofstream efile;
    numSubjA = 0; numSubjB = 0;
    for (int subj = 0; subj < numSubjects; subj++) {
      if (groupLabel[subj] == GROUP_A_LABEL) {
        numSubjA++;
      } else if (groupLabel[subj] == GROUP_B_LABEL) {
        numSubjB++;         
      } else {
        cerr << " group label " << groupLabel[subj] << " does not exist" << endl;
      }
    }
    cout << "group sizes: " << numSubjA << "/" << numSubjB << " = " << numSubjects << endl;
    cout << "Features : " << numFeatures << " , real: " << numFeatures / tupelSize << endl;

    double * permStat = new double [numPerms * numFeatures / tupelSize];
    int * newGroupLabel = new int [numSubjects]; 
    double * realStudTStat = new double [numFeatures / tupelSize];
    double * realMeanDiffStat = new double [numFeatures / tupelSize];
    double * avgStat = new double [numPerms];

    double * realStat = permStat; // the first permutation is the real one
    int updateInc = numPerms/40+1;

    // generate the permutations and the statistics of the permutations
    for (int perm = 0; perm < numPerms; perm++) {
      if (perm % updateInc == 0) {
       cout << "."; cout.flush();
      }

      generatePermGroup(groupLabel, numSubjA, numSubjB, newGroupLabel );
      if (perm == 0) {  // the first permutation is the real one , re-asssign 
       for (int i=0; i<numSubjects; i++) {newGroupLabel[i]=groupLabel[i];}
      } 

      if (tupelSize == 1) {
        computeGroupMeanDiffStat(numSubjects, numFeatures,  
                                 newGroupLabel, featureValue, 
                                   &(permStat[perm * numFeatures / tupelSize]), avgStat[perm]);
        if (perm == 0) {
          computeGroupStudentTDiffStat(numSubjects, numFeatures,
                                       newGroupLabel, featureValue,realStudTStat, avgStat[perm]);
        }
      } else  {
        computeGroupVecHotelDiffStat(numSubjects, numFeatures,  tupelSize,
                                     newGroupLabel, featureValue, 
                                     &(permStat[perm * numFeatures / tupelSize]), avgStat[perm]); 
        if (perm == 0) {
          double avgMeanDiff;
          computeGroupVecMeanDiffStat(numSubjects, numFeatures,  tupelSize,
                                      newGroupLabel, featureValue, realMeanDiffStat, avgMeanDiff); 
        }
      }
      //cout << avgStat[perm] << endl;
    }
    cout << endl;

    // compute p-values
    double * permStatPval = new double [numPerms * numFeatures / tupelSize];

    computePermStatPval(numFeatures / tupelSize, numPerms, permStat, permStatPval);

    double avgPval =  computePValue(numPerms, avgStat, avgStat[0]);
    
    cout << "writing avg shape diff pvalue " << avgPval << endl;
    // write uncorrected p-values
    string avgPvalFile(outbase);
    avgPvalFile = avgPvalFile + string("_avgPval.txt");
    efile.open(avgPvalFile.c_str(), ios::out); 
    efile << avgPval << endl;
    efile.close();
    
    cout << "writing raw pvalues " << endl;
    // write uncorrected p-values
    string rawPvalFile(outbase);
    rawPvalFile = rawPvalFile + string("_rawPval.txt");
    efile.open(rawPvalFile.c_str(), ios::out); 
    int feat;
    for (feat = 0; feat < numFeatures / tupelSize; feat++)
      efile << permStatPval[feat] << " ";
    efile << endl;
    efile.close();
    
    cout << "writing statistic (T^2)" << endl;
    // write uncorrected p-values
    string realStatFile(outbase);
    realStatFile = realStatFile + string("_realStat.txt");
    efile.open(realStatFile.c_str(), ios::out); 

    for (feat = 0; feat < numFeatures / tupelSize; feat++)
      efile << realStat[feat] << " ";
    efile << endl;
    efile.close();

    if (tupelSize == 1) {
      cout << "writing t statistic" << endl;
      // write uncorrected p-values
      string realTStatFile(outbase);
      realTStatFile = realTStatFile + string("_tstat.txt");
      efile.open(realTStatFile.c_str(), ios::out); 
      for (feat = 0; feat < numFeatures / tupelSize; feat++)
       efile << realStudTStat[feat] << " ";
      efile << endl;
      efile.close();

      cout << "writing Cohen's d statistic" << endl;
      // write uncorrected p-values
      string realZStatFile(outbase);
      realZStatFile = realZStatFile + string("_Dstat.txt");
      efile.open(realZStatFile.c_str(), ios::out); 
      for (feat = 0; feat < numFeatures / tupelSize; feat++)
       efile << 2 * realStudTStat[feat]/sqrt((double) (numSubjA + numSubjB - 2)) << " ";
      efile << endl;
      efile.close();

      cout << "writing r-square statistic" << endl;
      // write uncorrected p-values
      string realRStatFile(outbase);
      realRStatFile = realRStatFile + string("_r2stat.txt");
      efile.open(realRStatFile.c_str(), ios::out); 
      for (feat = 0; feat < numFeatures / tupelSize; feat++) {
       double factor = realStudTStat[feat] * realStudTStat[feat] /(numSubjA + numSubjB - 2);
       efile << factor / (1 + factor) << " ";
      }
      efile << endl;
      efile.close();

    } else {
      // write T stats = sqrt (T^2)
      cout << "writing T statistic" << endl;
      string realTStatFile(outbase);
      realTStatFile = realTStatFile + string("_Tstat.txt");
      efile.open(realTStatFile.c_str(), ios::out); 
      for (feat = 0; feat < numFeatures / tupelSize; feat++)
       efile << sqrt(realStat[feat]) << " ";
      efile << endl;
      efile.close();

      // write r^2: it is not yet clear whether this is the correct formulation of r^2, as we are using 
      // a non-standard, robust T^2 formulation
      cout << "writing r-square statistic" << endl;
      string realRStatFile(outbase);
      realRStatFile = realRStatFile + string("_r2stat.txt");
      efile.open(realRStatFile.c_str(), ios::out); 
      for (feat = 0; feat < numFeatures / tupelSize; feat++) {
       double factor = realStat[feat]/(numSubjA + numSubjB - 2);
       efile << factor / (1 + factor) << " ";
      }
      efile << endl;
      efile.close();

      cout << "writing Mean Diff statistic" << endl;
      // write uncorrected effect size = difference of means
      string realMeanDiffStatFile(outbase);
      realMeanDiffStatFile = realMeanDiffStatFile + string("_MeanDiff.txt");
      efile.open(realMeanDiffStatFile.c_str(), ios::out); 
      for (feat = 0; feat < numFeatures / tupelSize; feat++)
       efile << realMeanDiffStat[feat] << " ";
      efile << endl;
      efile.close();
    }


    cout << "computing Min distribution" << endl;
    
    // compute Minimum Statistic over all features for each permutation
    double * minStat = new double [numPerms]; 
    for (int perm = 0; perm < numPerms; perm++) {
      double percLevel;
      percLevel = 0.0000001;
      double percValue = computeSignValue(numFeatures / tupelSize, 
                             &(permStatPval[perm * numFeatures  / tupelSize]), 
                             percLevel); 
      minStat[perm] = percValue;
    }

    

    // reset pValue
    for (feat = 0; feat < numFeatures / tupelSize; feat++) pValue[feat] = 1.0;
    
    // loop over all significance levels
    double significanceStepsize = significanceLevel / significanceSteps;
    for (double curSignLevel = significanceLevel; curSignLevel >= 0.00001;
      curSignLevel = curSignLevel - significanceStepsize) {

     // compute significance value in minimum statistic at significance level 
     double curSignValue = computeSignValue(numPerms, minStat, curSignLevel);

     // find statistic treshold for each feature f using 
     // the significance value of the minimum statistic
     for (int feat = 0; feat < numFeatures  / tupelSize; feat++) {

         // if real value is equal or greater then 
         // feature Threshold then significant
         //if (realStat[feat] >= featThresh) {
         if (permStatPval[feat] <= curSignValue) {
          pValue[feat] = curSignLevel;
         }
      }
      if (debug) {
        cout << curSignLevel << " -> " << curSignValue << " done " << endl;
      }

    }
     

    string outfile(outbase);
    outfile = outfile + string("_corrPval.txt");
    efile.open(outfile.c_str(), ios::out); 
    for (feat = 0; feat < numFeatures / tupelSize; feat++)
      efile << pValue[feat] << " ";
    efile << endl;
    efile.close();

    delete permStat;
    delete realStudTStat;
    delete permStatPval;
    delete minStat;
    delete avgStat;
    delete newGroupLabel;
    delete realMeanDiffStat;

    return 1;
}

int doTestingMax(int numSubjects, int numFeatures, int numPerms, int tupelSize,
           int * groupLabel, double * featureValue,
           double significanceLevel, int significanceSteps, int testStatNorm,
           double * pValue, char *outbase)
// computes the local pValues for each feature according to non-parametric permutation tests
//
// numSubjects = Number of Subjects Group A + Group B
// numFeatures = Number of Scalar features per Subject
// numPerms    = Number of Permutations used in computation
// tupelSize   = Numbers of features grouped in tupels 
// groupLabel = int array [numSubjects] with group Labels, Group A = 0, Group B = 1
// featureValue = double array [numSubjects*numFeatures] with scalar feature values for
//   all Subjects e.g. n-th feature of m-th subjects is at featureValue[m * numFeatures + n]
// significanceLevel = level of significance (usually 0.05 or 0.01)
// significanceSteps = number of significance steps
// testStatNorm     = Which Normalization should be used ?
//    == 1 : normalize with standard deviation
//    == 2 : normalize with 68% cutoff
//    == 3 : do not normalize
// pValue = double array [numFeatures] output with p-values at each feature location
//          the pValue is == 1 for all features with pValue greater than significanceLevel
//          for features with pValue less than significanceLevel, the actual p-values are 
//          computed in step sizes of significanceLevel/significanceSteps; 
//  e.g. significanceValue = 0.05, significanceSteps = 10; so, the possible p-values are:
//    1 (non-significant), 0.05, 0.045, 0.040, 0.035, 0.030, 0.025, 0.020, 0.015, 0.010, 0.005
//
{

  cout << "doing max testing" << endl;
  int feat;
  int numSubjA, numSubjB;
  ofstream efile;
  numSubjA = 0; numSubjB = 0;
  for (int subj = 0; subj < numSubjects; subj++) {
    if (groupLabel[subj] == GROUP_A_LABEL) {
      numSubjA++;
    } else if (groupLabel[subj] == GROUP_B_LABEL) {
      numSubjB++;         
    } else {
      cerr << " group label " << groupLabel[subj] << " does not exist" << endl;
    }
  }
  cout << "group sizes: " << numSubjA << "/" << numSubjB << " = " << numSubjects << endl;
  cout << "Features : " << numFeatures << " , real: " << numFeatures / tupelSize << endl;
  
  double * permStat = new double [numPerms * numFeatures / tupelSize];
  int * newGroupLabel = new int [numSubjects]; 
  
  double * realStat = permStat; // the first permutation is the real one
  
  int updateInc = numPerms/40+1;
  // generate the permutations and the statistics of the permutations
  for (int perm = 0; perm < numPerms; perm++) {
    if (perm % updateInc == 0) {
      cout << "."; cout.flush();
    }
    
    generatePermGroup(groupLabel, numSubjA, numSubjB, newGroupLabel );
    if (perm == 0) { // the first permutation is the real one
      for (int i=0; i<numSubjects; i++) {newGroupLabel[i]=groupLabel[i];}
    } 
    
    double avgStat;
    if (tupelSize == 1) {
      computeGroupMeanDiffStat(numSubjects, numFeatures,  
                   newGroupLabel, featureValue, 
                   &(permStat[perm * numFeatures / tupelSize]), avgStat);
    } else {
      computeGroupVecHotelDiffStat(numSubjects, numFeatures,  tupelSize,
                   newGroupLabel, featureValue, 
                   &(permStat[perm * numFeatures / tupelSize]), avgStat); 
    }
    
  }
  cout << endl;
    

  // normalize with standard deviation
  if (testStatNorm == 1) {
    cout << "doing stddev normalization" << endl;
    for (feat = 0; feat < numFeatures  / tupelSize; feat++) {
      double mean = 0;
      for (int perm = 0; perm < numPerms; perm++) {
        double curVal = permStat[perm * numFeatures  / tupelSize + feat];
        mean = mean + curVal;
      }
      mean = mean / numPerms;
      double sigma = 0;
      for (int perm = 0; perm < numPerms; perm++) {
        double curVal = permStat[perm * numFeatures  / tupelSize + feat];
        sigma = sigma + (curVal - mean) * (curVal - mean);
      }
      sigma = sqrt(sigma / (numPerms - 1 ));
      for (int perm = 0; perm < numPerms; perm++) {
        double curVal = permStat[perm * numFeatures  / tupelSize + feat];
        permStat[perm * numFeatures  / tupelSize + feat] = curVal / sigma; 
      }
    }
  }
  
  // normalize with 68% percentile of each feature
  if (testStatNorm == 2) { 
    cout << "doing 68% normalization (1 stddev)" << endl;
    // it would be better to get the limit 
    // from a second permutation computation
    double * permFeatVals= new double [numPerms];
    for (feat = 0; feat < numFeatures  / tupelSize; feat++) {
      for (int perm = 0; perm < numPerms; perm++) {
        permFeatVals[perm] = permStat[perm * numFeatures  / tupelSize + feat];
      }
      double percLevel = 0.68;  
      double percValue = computeSignValue(numPerms , permFeatVals, percLevel); 
      //if (debug)
      //cout << "(" << percValue << "," << permStat[feat] << ") - " ; cout.flush();
      for (int perm = 0; perm < numPerms; perm++) {
        double curVal = permStat[perm * numFeatures  / tupelSize + feat];
        permStat[perm * numFeatures  / tupelSize + feat] = curVal / percValue; 
      }
    }
    //cout << endl;
  }
  
  // no normalization
  if (testStatNorm == 3) {
    cout << "doing no normalization" << endl;
  }
  

  // compute Maximum Statistic over all features for each permutation
  double * maxStat = new double [numPerms]; 
  for (int perm = 0; perm < numPerms; perm++) {
    
    double percLevel;
    percLevel = 0.99999999; 
    double percValue = computeSignValue(numFeatures  / tupelSize , 
                    &(permStat[perm * numFeatures  / tupelSize]), 
                    percLevel); 
    maxStat[perm] = percValue;
  }

  for (feat = 0; feat < numFeatures / tupelSize; feat++) pValue[feat] = 1.0;
  
  // loop over all significance levels
  double significanceStepsize = significanceLevel / significanceSteps;
  for (double curSignLevel = significanceLevel; curSignLevel >= 0.00001;
       curSignLevel = curSignLevel - significanceStepsize) {
    
    // compute significance value in maximum statistic at significance level 
    double curSignValue = computeSignValue(numPerms, maxStat, 1-curSignLevel);
    
    // find statistic treshold for each feature f using 
    // the significance value of the maximum statistic
    for (feat = 0; feat < numFeatures  / tupelSize; feat++) {
      if (realStat[feat] >= curSignValue) {
    pValue[feat] = curSignLevel;
      }
    }
    if (debug) {
      cout << curSignLevel << " -> " << curSignValue << " done " << endl;
    }
    
  }
  string outfile(outbase);
  outfile = outfile + string("_corrPval.txt");
  efile.open(outfile.c_str(), ios::out); 
  for (feat = 0; feat < numFeatures / tupelSize; feat++)
    efile << pValue[feat] << " ";
  efile << endl;
  efile.close();
  
  delete permStat;
  delete maxStat;
  delete newGroupLabel;
  //delete permFeatVals;
  
  return 1;
}

double computePValue(int numPerms, double * stat, double statVal)
// computes the p-value at the given statistical Value using the permutations 
// computes single right tail significance level (appropriate for Maximum statistic)
//
// numPerms = Number of Permutations
// stat = double array[numPerms ] contains the test statisticss
{ 
  static int first = 0;
    if (!first) {
     first = 1;
     srand(time(NULL));
    }

    StatElement * sortPermStat= new StatElement[numPerms];

    for (int perm = 0; perm < numPerms; perm++) {
     sortPermStat[perm].statVal = stat[perm];
     sortPermStat[perm].index = perm;
    }
    
    // sort, smallest first
    qsort(sortPermStat, numPerms, sizeof(StatElement), 
       (int (*) (const void *, const void *)) smallerStatElem);

    // find smallest element
    bool found = false;
    int sortIndex = -1;
    for (int pIndex = numPerms - 1 ; pIndex >= 0 && !found ; pIndex--) {
      if (statVal >= sortPermStat[pIndex].statVal ) {
       found = true;
       sortIndex = pIndex;
       // cout << statVal << "," << sortPermStat[pIndex].statVal << "," << pIndex << endl;
      }
    }
    double retval = 1.0 - (double) sortIndex/numPerms;
    
    delete sortPermStat;

    return retval;

}

double computeSignValue(int numPerms, double * stat, double significanceLevel)
// computes the value at the significanceLevel 
// This value is the treshold for the provided statistic at the given significanceLevel
// computes single left tail significance level (appropriate for Minimum statistic)
//
// numPerms = Number of Permutations
// stat = double array[numPerms ] contains the test statisticss
{ 
  static int first = 0;
    if (!first) {
     first = 1;
     srand(time(NULL));
    }

    StatElement * sortPermStat= new StatElement[numPerms];

    for (int perm = 0; perm < numPerms; perm++) {
     sortPermStat[perm].statVal = stat[perm];
     sortPermStat[perm].index = perm;
    }
    
    // sort, smallest first
    qsort(sortPermStat, numPerms, sizeof(StatElement), 
       (int (*) (const void *, const void *)) smallerStatElem);

    // index at treshold level
    double signIndex = (double) numPerms * significanceLevel;
    if (significanceLevel == 1.0 ) signIndex = numPerms;
    
    double retval = stat[sortPermStat[(int) signIndex].index];
    
    delete sortPermStat;

    return retval;

}

void computePermStatPval(int numFeatures, int numPerms, 
                double * permStat, double * permStatPval)
// computes the Pval for all permutation statistics
// the p-val is computed as the percentual ordered rank over all permutations
//
// numFeatures = Number of scalar features per Subject
// numPerms = Number of Permutations
// permStat = double array[numPerms * numFeatures] contains the test statistics
// permStatPval = double array [numPerms * numFeatures] returns the p-val of the statistics
{
    static int first = 0;
    if (!first) {
     first = 1;
     srand(time(NULL));
    }

    int feat;
    int perm;

    StatElement * sortPermStat= new StatElement[numPerms];

    for (feat = 0; feat < numFeatures; feat++) {

     for (perm = 0; perm < numPerms; perm++) {
         sortPermStat[perm].statVal = permStat[perm * numFeatures + feat];
         sortPermStat[perm].index = perm;
     }
     
     // sort, smallest first
     qsort(sortPermStat, numPerms, sizeof(StatElement), 
           (int (*) (const void *, const void *)) smallerStatElem);
     
     double prevPval = 0;
     double curPval = 0;
     for (perm = 0 ; perm < numPerms; perm++) {

       // percentual rank 0..1 -> cumulative probability -> p-val
       double nextPval = 1.0 - (double) (perm+1) / (double) numPerms; 
       
       int curIndex = sortPermStat[perm].index;
       
       if ((perm == 0) || (sortPermStat[perm].statVal != sortPermStat[perm-1].statVal)) {
         // current value is different from previous value (or first value), 
         // thus step up p-value
         prevPval = curPval;
         curPval = nextPval;
       }

       permStatPval[curIndex * numFeatures + feat] = curPval;

     }
    }
    delete sortPermStat;
}

void ALTcomputePermStatPval(int numFeatures, int numPerms, 
                double * permStat, double * permStatPval)
// computes the Pval for all permutation statistics
// the p-val is computed as the percentual ordered rank over all permutations
//
// numFeatures = Number of scalar features per Subject
// numPerms = Number of Permutations
// permStat = double array[numPerms * numFeatures] contains the test statistics
// permStatPval = double array [numPerms * numFeatures] returns the p-val of the statistics
{
    static int first = 0;
    if (!first) {
     first = 1;
     srand(time(NULL));
    }

    StatElement * sortPermStat= new StatElement[numPerms];
    int feat;
    int perm;

    for (feat = 0; feat < numFeatures; feat++) {

     for (perm = 0; perm < numPerms; perm++) {
         sortPermStat[perm].statVal = permStat[perm * numFeatures + feat];
         sortPermStat[perm].index = perm;
     }
     
     // sort, smallest first
     qsort(sortPermStat, numPerms, sizeof(StatElement), 
           (int (*) (const void *, const void *)) smallerStatElem);
     
     double CurPval = 1;
     double stepSize = fabs(sortPermStat[(int) ((double) 0.96 * numPerms)].statVal -
                      sortPermStat[(int) ((double) 0.95 * numPerms)].statVal);
     if (stepSize == 0) 
       stepSize = fabs(sortPermStat[numPerms - 1].statVal - sortPermStat[0].statVal);
     double CurBinMin = sortPermStat[0].statVal;

     for (perm = 0 ; perm < numPerms; perm++) {
       if (sortPermStat[perm].statVal > CurBinMin + stepSize) {
         CurBinMin = sortPermStat[perm].statVal;
         CurPval = 1.0 - (double) (perm+1) / (double) numPerms;
       }
       
       int curIndex = sortPermStat[perm].index;
       
       permStatPval[curIndex * numFeatures + feat] = CurPval;
     }
    }

    delete sortPermStat;

}


int smallerStatElem(StatElement * elem1, StatElement * elem2)
// comparison function for sorting
{
    if (elem1->statVal > elem2->statVal) return 1;
    else if (elem1->statVal < elem2->statVal) return -1;
    else return 0;
}
