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

#ifdef _WINDOWS
#include <getoptwin32.h>
#else
#include <getopt.h>
#endif

#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_convolve.h>
#include "itkStatisticalPermutation.h"
#include <vnl/vnl_matlab_read.h>
#include <vcl_string.h>
#include <vcl_fstream.h>

const int cNumPerm = 40000;
const int cMinmax = 0;
const int cNumcontrol = 100;
const int cNumpatients = 90;
const int cMeasureDimension = 500;

void showUsage(const char* programName)
{
  vcl_cerr << "USAGE:  " << programName << " [OPTIONS] " << vcl_endl;
  vcl_cerr << "\t\t-h --help : print this message" << vcl_endl;
  vcl_cerr << "\t\t-n --numperm <numperm>: number of permutations (def = "
      << cNumPerm << ")." << vcl_endl;
  vcl_cerr << "\t\t-m --minmax <minmax> : min or max of the quintile" << vcl_endl;
  vcl_cerr << "\t\t\t(-1 = min, 1 = max, 0 = max(abs), def = "
      << cMinmax << ")." << vcl_endl;
  vcl_cerr << "\t\t-c --numcontrol : number of control (def =  "
      << cNumcontrol << ")." << vcl_endl;
  vcl_cerr << "\t\t-p --numpatients : number of patients (def =  "
      << cNumpatients << ")." << vcl_endl;
  vcl_cerr << "\t\t-d --measuredim : dimension of measurement vector (def =  "
      << cMeasureDimension << ")." << vcl_endl;
  vcl_cerr << "\t\t-C --readControlFile <controlFile> : name of MATLAB file containing control data " << vcl_endl;
  vcl_cerr << "\t\t\t(stored matrix size overrides above user params, def = random generation)" << vcl_endl;
  vcl_cerr << "\t\t-P --readPatientFile <patientFile> : name of MATLAB file containing patient data " << vcl_endl;
  vcl_cerr << "\t\t\t(stored matrix size overrides above user params, def = random generation)" << vcl_endl;
  vcl_cerr << "\t\t-d --measuredim : dimension of measurement vector (def =  "
      << cMeasureDimension << ")." << vcl_endl;

  exit(EXIT_FAILURE);
}

int main( int argc, char *argv[] )
{

    int numPerm = cNumPerm;
    int minmax = cMinmax;
    int numcontrol = cNumcontrol;
    int numpatients = cNumpatients;
    int measureDimension = cMeasureDimension;

    vcl_string patientMATLABFile;
    vcl_string controlMATLABFile;

    int c = 0;
    while(c != EOF)
    {
        static struct option long_options[] =
        {
            {"help",             no_argument,       NULL, 'h'},
            {"numperm",          required_argument, NULL, 'n'},
            {"minmax",           required_argument, NULL, 'm'},
            {"numcontrol",       required_argument, NULL, 'c'},
            {"numpatients",      required_argument, NULL, 'p'},
            {"measuredim ",      required_argument, NULL, 'd'},
            {"readControlFile",  required_argument, NULL, 'C'},
            {"readPatientFile",  required_argument, NULL, 'P'},
            {                  0,                 0,    0,   0},
        };

        int option_index = 0;

        switch((c = 
            getopt_long(argc, argv, "hn:m:c:p:d:C:P:",
                        long_options, &option_index)))
        {
            case 'h':
                showUsage(argv[0]);
                break;
            case 'n':
                numPerm = atoi(optarg);
                break;
            case 'm':
                minmax = atoi(optarg);
                break;
            case 'c':
                numcontrol = atoi(optarg);
                break;
            case 'p':
                numpatients = atoi(optarg);
                break;
            case 'd':
                measureDimension = atoi(optarg);
                break;
            case 'C':
                controlMATLABFile = optarg;
                break;
            case 'P':
                patientMATLABFile = optarg;
                break;
            default:
                if (c > 0)
                    showUsage(argv[0]);
                break;
        }
    }

// Required arguments

#define ARG_COUNT 1
    if((argc - optind + 1) < ARG_COUNT)
    {
      showUsage(argv[0]);
      exit(1);
    }

    typedef double             MeasurementValueType;
    typedef vnl_matrix<MeasurementValueType> MeasurementType;
    typedef int                RankValType;

    // Declare the type of the input and output data
    typedef itk::StatPerm<MeasurementType, RankValType, itk::VectorRankSum<MeasurementType, RankValType> > StatPermType;
    typedef StatPermType::MeasurementSetType MeasurementSetType;
    MeasurementSetType::Pointer controlData = MeasurementSetType::New();
    MeasurementSetType::Pointer patientData = MeasurementSetType::New();

    StatPermType::MaxMinEnumType minMaxParameter = StatPermType::MaxAbsoluteValues;

    switch (minmax)
      {
      case 1:
        minMaxParameter = StatPermType::Maximums;
        break;
      case -1:
        minMaxParameter = StatPermType::Minimums;
        break;
      case 0:
        minMaxParameter = StatPermType::MaxAbsoluteValues;
        break;
      default:
        std::cerr << "Unexpected maxmin value:  "  << minmax << std::endl;
        showUsage(argv[0]);
        break;
      }

    vnl_vector<MeasurementValueType> b(20);
    b *= 1.0 / b.size();

    vnl_random genRand;

    if (controlMATLABFile.length() > 0)
    {
      // read in control data from a MATLAB file
      vnl_matrix<MeasurementValueType> controls;
      vcl_ifstream cntrlData(controlMATLABFile.c_str(), vcl_ifstream::binary);
      if (!cntrlData)
      {
        vcl_cerr << "Could not open control file " << controlMATLABFile << vcl_endl;
        exit (-1);
      }
      if (!vnl_matlab_read_or_die(cntrlData, controls, "controlMatrix"))
      {
        vcl_cerr << "Could not read variable \"controlMatrix\" from control file " << controlMATLABFile << vcl_endl;
        exit (-1);
      }
      measureDimension  = controls.cols();
      numcontrol = controls.rows();
      controlData->Reserve(numcontrol);

      for (int cx = 0; cx < numcontrol; cx++)
      {
        vnl_matrix<MeasurementValueType> oneInSet(measureDimension, 1);
        oneInSet.set_column(0, controls.get_row(cx));
        controlData->SetElement(cx, oneInSet);
      }
    }
    else
    {
      controlData->Reserve(numcontrol);
      patientData->Reserve(numpatients);

      for (int cx = 0; cx < numcontrol; cx++)
        {
        MeasurementType oneControl(measureDimension, 1);
        for (int mx = 0; mx < measureDimension; mx++)
          oneControl(mx, 0) = genRand.normal64();

        vnl_vector<MeasurementValueType> smoothControl =
          vnl_convolve(oneControl.get_column(0), b);
        oneControl.set_column(0,
            smoothControl.extract(oneControl.rows(), b.size() - 1));
        controlData->SetElement(cx, oneControl);
      }
    }

    if (patientMATLABFile.length() > 0)
    {
      // read in patient data from a MATLAB file
      vnl_matrix<MeasurementValueType> patients;
      vcl_ifstream ptData(patientMATLABFile.c_str(), vcl_ifstream::binary);
      if (!ptData)
      {
        vcl_cerr << "Could not open patient file " << patientMATLABFile << vcl_endl;
        exit (-1);
      }
      if (!vnl_matlab_read_or_die(ptData, patients, "patientMatrix"))
      {
        vcl_cerr << "Could not read variable \"patientMatrix\" from patient file " << patientMATLABFile << vcl_endl;
        exit (-1);
      }
      measureDimension  = patients.cols();
      numpatients = patients.rows();
      patientData->Reserve(numpatients);

      for (int cx = 0; cx < numpatients; cx++)
      {
        vnl_matrix<MeasurementValueType> oneInSet(measureDimension, 1);
        oneInSet.set_column(0, patients.get_row(cx));
        patientData->SetElement(cx, oneInSet);
      }
    }
    else
    {
      for (int px = 0; px < numpatients; px++)
        {
        MeasurementType onePatient(measureDimension, 1);
        for (int mx = 0; mx < measureDimension; mx++)
          onePatient(px, 0) = genRand.normal64();

        vnl_vector<MeasurementValueType> smoothPatient =
          vnl_convolve(onePatient.get_column(0), b);
        onePatient.set_column(0,
            smoothPatient.extract(onePatient.rows(), b.size() - 1));
        // mimic behavior as Laurent's test code
        for (int sx = 0;
            sx < (10 < measureDimension ? 10 : measureDimension); sx++)
          onePatient(sx, 0) += 0.1;

        patientData->SetElement(px, onePatient);
        }
    }

    if (numcontrol < 1)
    {
      vcl_cerr << "No control data." << vcl_endl;
      exit (-1);
    }

    if (numpatients < 1)
    {
      vcl_cerr << "No patient data." << vcl_endl;
      exit (-1);
    }

    if (controlData->GetElement(0).rows() != patientData->GetElement(0).rows())
    {
      vcl_cerr << "Sizes of patient(" << patientData->GetElement(0).rows() <<
        ") and control " << controlData->GetElement(0).rows() <<
        ") data do not match." << vcl_endl;
      exit (-1);
    }

    StatPermType::Pointer myStatPerm = StatPermType::New();

    myStatPerm->SetMinMax(minMaxParameter);
    myStatPerm->SetNumPerm(numPerm);

    myStatPerm->SetInput(controlData, patientData);

    // compute the stats

    try
      {
      myStatPerm->Compute();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Stat Perm Failed:  " << std::endl;
      std::cerr << excp << std::endl;
      }

    std::cerr << "PValue:  " << myStatPerm->GetPValue() << std::endl;
    std::cerr << "ObtainedQuantile:  " << myStatPerm->GetObtainedQuantile() << std::endl;
    std::cerr << "StatVector:  " << myStatPerm->GetStatVector() << std::endl;
    std::cerr << "QuantileByIteration:  " << myStatPerm->GetQuantileByIteration() << std::endl;

  return EXIT_SUCCESS;
}
