#ifndef COMPUTE_PERM_P_VAL_H__MST
#define COMPUTE_PERM_P_VAL_H__MST
 
#include "createPerm.h"

typedef struct
{
    double statVal;
    int index;
} StatElement;


int doTesting(int numSubjects, int numFeatures, int numPerms, int tupelSize,
           int * groupLabel, double * featureValue,
           double significanceLevel, int significanceSteps, int testStatistic,
                            double * pValue, char *outbase);
// computes the local pValues for each feature according to non-parametric permutation tests, considering p-values for equalizing sensitivity

int doTestingMax(int numSubjects, int numFeatures, int numPerms, int tupelSize,
           int * groupLabel, double * featureValue,
           double significanceLevel, int significanceSteps,
           int testStatisticType, int testStatNorm,
           double * pValue, char *outbase);
// computes the local pValues for each feature according to non-parametric permutation tests, only considering Tmax statistics
     
void computeGroupVecHotelDiffStat(int numSubjects, int numFeatures, int tupelSize, 
                      int * groupLabel, double * featureValue,
                      double * statistic);
// computes the group difference statistic for the given group Labels 
// computes Hotelling metric using 3D coordinates

void computeGroupVecMeanDiffStat(int numSubjects, int numFeatures, int tupelSize,
                      int * groupLabel, double * featureValue,
                      double * statistic);
// computes the group difference statistic for the given group Labels 
// computes Absolute Mean distance metric using 3D coordinates

     
void computeGroupDiffStat(int numSubjects, int numFeatures,
                 int * groupLabel, double * featureValue,
                 double * statistic);
// computes the group difference statistic for the given group Labels 
// computes absolute mean difference

double computeSignValue(int numPerms, double * stat, double significanceLevel);
// computes the value at the significanceLevel 

void computePermStatPval(int numFeatures, int numPerms, 
                double * permStat, double * permStatPval);
// computes the Pval for all permutation statistics

int smallerStatElem(StatElement * elem1, StatElement * elem2);
// comparison function for sorting



#endif
