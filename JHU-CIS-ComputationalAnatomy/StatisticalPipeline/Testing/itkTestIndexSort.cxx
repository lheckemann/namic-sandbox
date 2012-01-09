/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <getopt.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_random.h>
#include <vcl_algorithm.h>
#include "vnl_index_sort.h"
#include <vcl_iostream.h>
#include <libio.h>

const int cNumElRows = 10;
const int cNumElCols = 6;

void showUsage(const char* programName)
{
  vcl_cerr << "USAGE: " << programName << " [OPTIONS]\n";
  vcl_cerr << "\t\t-h --help : print this message\n";
  vcl_cerr << "\t\t-n --numelrows <numelrows>: number of elements to sort (def = " << cNumElRows << ").\n";
  vcl_cerr << "\t\t-m --numelcols <numelcols>: number of elements to sort (def = " << cNumElCols << ").\n";

  exit(1);
}

int main( int argc, char *argv[] )
{

  int numelrows = cNumElRows;
  int numelcols = cNumElCols;

  int c = 0;
  while(c != EOF)
  {
      static struct option long_options[] =
      {
          {"help",             no_argument,       NULL, 'h'},
          {"numelrows",        required_argument, NULL, 'n'},
          {"numelcols",        required_argument, NULL, 'm'},
          {                  0,                 0,    0,   0},
      };

      int option_index = 0;

      switch((c = 
          getopt_long(argc, argv, "hn:m:",
                      long_options, &option_index)))
      {
          case 'h':
              showUsage(argv[0]);
              break;
          case 'n':
              numelrows = atoi(optarg);
              break;
          case 'm':
              numelcols = atoi(optarg);
              break;
          default:
              if (c > 0)
                  showUsage(argv[0]);
              break;
      }
  }

  typedef double             MeasurementValueType;
  typedef vnl_vector<MeasurementValueType> MeasurementVectorType;
  typedef vnl_matrix<MeasurementValueType> MeasurementMatrixType;

  typedef int                RankValType;
  typedef vnl_vector<RankValType> IndexVectorType;
  typedef vnl_matrix<RankValType> IndexMatrixType;

  typedef vnl_index_sort<MeasurementValueType, RankValType> IndexSortType;


  bool caughtError = false;

  MeasurementVectorType randomVals(numelrows);

  vnl_random genRand(9667566);
  for (int cx = 0; cx < numelrows; cx++)
    randomVals(cx) = genRand.lrand32(numelrows * 2);

  IndexSortType indexSort;

  MeasurementVectorType sortedVals;
  IndexVectorType sortIndices;
  indexSort.vector_sort(randomVals, sortedVals, sortIndices);

  MeasurementVectorType vclSortedVals(randomVals);
  vcl_sort(vclSortedVals.begin(), vclSortedVals.end());

  // check against vcl_sort
  bool sortCheckFail = true;

  if ((sortCheckFail = (vclSortedVals != sortedVals)))
    vcl_cerr << "Random Vector Sort FAILED." << vcl_endl;
  else
    vcl_cout << "Random Vector Sort check PASSED." << vcl_endl;

  caughtError = caughtError || sortCheckFail;

  // check indices
  sortCheckFail = false;
  for (unsigned int ix = 0; ix < randomVals.size(); ix++)
    if (vclSortedVals(ix) != randomVals(sortIndices(ix))) sortCheckFail = true;

  if (sortCheckFail)
    vcl_cerr << "Random Vector Index Sort FAILED." << vcl_endl;
  else
    vcl_cout << "Random Vector Index Sort check PASSED." << vcl_endl;

  caughtError = caughtError || sortCheckFail;


  // check in-place sort
  sortedVals = randomVals;
  indexSort.vector_sort_in_place(sortedVals, sortIndices);
  sortCheckFail = false;
  if ((sortCheckFail = (vclSortedVals != sortedVals)))
    vcl_cerr << "In Place Vector Sort FAILED." << vcl_endl;
  else
    vcl_cout << "In Place Vector Sort check PASSED." << vcl_endl;

  caughtError = caughtError || sortCheckFail;

  // check indices
  sortCheckFail = false;
  for (unsigned int ix = 0; ix < randomVals.size(); ix++)
    if (vclSortedVals(ix) != randomVals(sortIndices(ix))) sortCheckFail = true;

  if (sortCheckFail)
    vcl_cerr << "In Place Vector Index Sort FAILED." << vcl_endl;
  else
    vcl_cout << "In Place Vector Index Sort check PASSED." << vcl_endl;

  caughtError = caughtError || sortCheckFail;


  // check matrix sorting now...
  MeasurementMatrixType randomValM(numelrows, numelcols);
  for (unsigned int rx = 0; rx < randomValM.rows(); rx++)
    for (unsigned int cx = 0; cx < randomValM.cols(); cx++)
      randomValM(rx, cx) = genRand.lrand32(numelrows * 2);

  MeasurementMatrixType sortedValM;
  IndexMatrixType sortedIndicesM;

  // do the matrix sort row-wise
  indexSort.matrix_sort(
    IndexSortType::ByRow, randomValM, sortedValM, sortedIndicesM);

  bool sortIndexCheckFail;
  sortCheckFail = sortIndexCheckFail = false;

  for (unsigned int rx = 0;
    rx < randomValM.rows() && !sortIndexCheckFail && !sortCheckFail; rx++)
    {
    MeasurementVectorType unsortedVectCheck = randomValM.get_row(rx);
    MeasurementVectorType vclSortedVectCheck(unsortedVectCheck);
    vcl_sort(vclSortedVectCheck.begin(), vclSortedVectCheck.end());
    MeasurementVectorType sortedVectCheck = sortedValM.get_row(rx);
    IndexVectorType indexVectCheck = sortedIndicesM.get_row(rx);

    // check against vcl_sort
    sortCheckFail = (vclSortedVectCheck != sortedVectCheck);

    // check indices
    sortIndexCheckFail = false;
    for (unsigned int ix = 0; ix < sortedVectCheck.size(); ix++)
      if (sortedVectCheck(ix) != unsortedVectCheck(indexVectCheck(ix))) sortIndexCheckFail = true;

    }
  if (sortCheckFail)
    vcl_cerr << "Row-Wise Matrix Sort check FAILED." << vcl_endl;
  else
    vcl_cout << "Row-Wise Matrix Sort check PASSED." << vcl_endl;

  if (sortIndexCheckFail)
    vcl_cerr << "Row-Wise Matrix Index Sort Check FAILED." << vcl_endl;
  else
    vcl_cout << "Row-Wise Matrix Index Sort check PASSED." << vcl_endl;

  caughtError = caughtError || sortCheckFail || sortIndexCheckFail;

  // do the matrix sort column-wise
  sortCheckFail = sortIndexCheckFail = false;
  indexSort.matrix_sort(
    IndexSortType::ByColumn, randomValM, sortedValM, sortedIndicesM);

  for (unsigned int rx = 0; rx < randomValM.cols() && !sortIndexCheckFail && !sortCheckFail; rx++)
    {
    MeasurementVectorType unsortedVectCheck = randomValM.get_column(rx);
    MeasurementVectorType vclSortedVectCheck(unsortedVectCheck);
    vcl_sort(vclSortedVectCheck.begin(), vclSortedVectCheck.end());
    MeasurementVectorType sortedVectCheck = sortedValM.get_column(rx);
    IndexVectorType indexVectCheck = sortedIndicesM.get_column(rx);

    // check against vcl_sort
    sortCheckFail = (vclSortedVectCheck != sortedVectCheck);

    // check indices
    sortIndexCheckFail = false;
    for (unsigned int ix = 0; ix < sortedVectCheck.size(); ix++)
      if (sortedVectCheck(ix) != unsortedVectCheck(indexVectCheck(ix))) sortIndexCheckFail = true;

    }
  if (sortCheckFail)
    vcl_cerr << "Column-Wise Matrix Sort check FAILED." << vcl_endl;
  else
    vcl_cout << "Column-Wise Matrix Sort check PASSED." << vcl_endl;

  if (sortIndexCheckFail)
    vcl_cerr << "Column-Wise Matrix Index Sort Check FAILED." << vcl_endl;
  else
    vcl_cout << "Column-Wise Matrix Index Sort check PASSED." << vcl_endl;

  caughtError = caughtError || sortCheckFail || sortIndexCheckFail;

  // check In-place matrix sorting now...
  for (unsigned int rx = 0; rx < randomValM.rows(); rx++)
    for (unsigned int cx = 0; cx < randomValM.cols(); cx++)
      randomValM(rx, cx) = genRand.lrand32(numelrows * 2);

  // initialize to random for in-place sort check...
  sortedValM = randomValM;

  // do the matrix sort row-wise
  indexSort.matrix_sort(
    IndexSortType::ByRow, sortedValM, sortedValM, sortedIndicesM);

  sortCheckFail = sortIndexCheckFail = false;
  for (unsigned int rx = 0; rx < randomValM.rows() && !sortIndexCheckFail && !sortCheckFail; rx++)
    {
    MeasurementVectorType unsortedVectCheck = randomValM.get_row(rx);
    MeasurementVectorType vclSortedVectCheck(unsortedVectCheck);
    vcl_sort(vclSortedVectCheck.begin(), vclSortedVectCheck.end());
    MeasurementVectorType sortedVectCheck = sortedValM.get_row(rx);
    IndexVectorType indexVectCheck = sortedIndicesM.get_row(rx);

    // check against vcl_sort
    sortCheckFail = (vclSortedVectCheck != sortedVectCheck);

    // check indices
    for (unsigned int ix = 0; ix < sortedVectCheck.size(); ix++)
      if (sortedVectCheck(ix) != unsortedVectCheck(indexVectCheck(ix))) sortIndexCheckFail = true;

    }
  if (sortCheckFail)
    vcl_cerr << "In-place Row-Wise Matrix Sort check FAILED." << vcl_endl;
  else
    vcl_cout << "In-place Row-Wise Matrix Sort check PASSED." << vcl_endl;

  if (sortIndexCheckFail)
    vcl_cerr << "In-place Row-Wise Matrix Index Sort Check FAILED." << vcl_endl;
  else
    vcl_cout << "In-place Row-Wise Matrix Index Sort check PASSED." << vcl_endl;

  caughtError = caughtError || sortCheckFail || sortIndexCheckFail;


  sortedValM = randomValM;
  // do the in-place matrix sort column-wise
  indexSort.matrix_sort(
    IndexSortType::ByColumn, sortedValM, sortedValM, sortedIndicesM);

  sortCheckFail = sortIndexCheckFail = false;
  for (unsigned int rx = 0; rx < randomValM.cols() && !sortIndexCheckFail && !sortCheckFail; rx++)
    {
    MeasurementVectorType unsortedVectCheck = randomValM.get_column(rx);
    MeasurementVectorType vclSortedVectCheck(unsortedVectCheck);
    vcl_sort(vclSortedVectCheck.begin(), vclSortedVectCheck.end());
    MeasurementVectorType sortedVectCheck = sortedValM.get_column(rx);
    IndexVectorType indexVectCheck = sortedIndicesM.get_column(rx);

    // check against vcl_sort
    // check against vcl_sort
    sortCheckFail = (vclSortedVectCheck != sortedVectCheck);

    // check indices
    sortIndexCheckFail = false;
    for (unsigned int ix = 0; ix < sortedVectCheck.size(); ix++)
      if (sortedVectCheck(ix) != unsortedVectCheck(indexVectCheck(ix))) sortIndexCheckFail = true;

    }
  if (sortCheckFail)
    vcl_cerr << "In-place Column-Wise Matrix Sort check FAILED." << vcl_endl;
  else
    vcl_cout << "In-place Column-Wise Matrix Sort check PASSED." << vcl_endl;

  if (sortIndexCheckFail)
    vcl_cerr << "In-place Column-Wise Matrix Index Sort Check FAILED." << vcl_endl;
  else
    vcl_cout << "In-place Column-Wise Matrix Index Sort check PASSED." << vcl_endl;

  caughtError = caughtError || sortCheckFail || sortIndexCheckFail;

  if (caughtError)
    vcl_cerr << "*******SORT TEST FAILED******" << vcl_endl;

  return (caughtError ? 1 : 0);
}
