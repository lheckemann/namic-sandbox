#include <cassert>
#include <iostream>
#include <vector>
#include "libs/vtk_reader.h"
#include "libs/linalg.h"
#include "libs/utilities.h"


std::vector<double> fa_t1;
std::vector<double> fa_t2;
std::vector<double> fa_t3;
std::vector<double> ra_t1;
std::vector<double> ra_t2;
std::vector<double> ra_t3;
std::vector<double> trace_t1;
std::vector<double> trace_t2;
std::vector<double> trace_t3;
std::vector<double> axial_t1;
std::vector<double> axial_t2;
std::vector<double> axial_t3;
std::vector<double> radial_t1;
std::vector<double> radial_t2;
std::vector<double> radial_t3;
std::vector<double> avg_fa;

void MeanStd(const std::vector<double>& arr, double& mean, double& std);

void PushBack(int count, double l1, double l2, double l3);

int main(int argc, char **argv) {
  // Default parameters.
  const char *fiber_file = NULL;

  // Parse command line arguments.
  std::string help_string =
      std::string("Usage:\n") + argv[0] + "\n  --fiber_file FILE\n";
      
  bool error = false;
  int i = 1;
  if (argc % 2) {
    while (i + 1 < argc) {
      if (!std::string(argv[i]).compare("--fiber_file")) {
        fiber_file = argv[++i];
      } else {
        error = true;
        break;
      }
      ++i;
    }
  } else {
    error = true;
  }

  // Handle erroneous input.
  if (!fiber_file || error || argc != 3) {
    std::cout << "Error!" << std::endl << std::endl;
    std::cout << help_string << std::endl;
    return 1;
  }

  VtkReader reader;
  reader.Read(fiber_file);

  const std::vector<vec_t>& points = reader.points();
  const std::map<std::string, std::vector<std::vector<double> > >& fields =
      reader.fields();
  if (fields.find("state") == fields.end()) {
    std::cout << "The state is needed to calulate statistics for the fibers. "
              << "Aborting!" << std::endl;
    return 1;
  }
  const std::vector<std::vector<double> >& states =
      fields.find("state")->second;

  assert(points.size() == states.size());
  assert(states.size() && states[0].size());
  int state_size = states[0].size();
  assert(state_size == 5 || state_size == 5 || state_size == 10 ||
         state_size == 12 || state_size == 15 || state_size == 18);

  // Iterate through all points and calculate statistics.
  for (size_t i = 0; i < points.size(); ++i) {
    const std::vector<double>& state = states[i];

    if (state_size == 5) {
      PushBack(1, state[3], state[4], state[4]);
    } else if (state_size == 6) {
      PushBack(1, state[3], state[4], state[5]);
    } else if (state_size == 10) {
      PushBack(1, state[3], state[4], state[4]);
      PushBack(2, state[8], state[9], state[9]);
      avg_fa.push_back(0.5 * (fa_t1[i] + fa_t2[i]));
    } else if (state_size == 12) {
      PushBack(1, state[3], state[4], state[5]);
      PushBack(2, state[9], state[10], state[11]);
      avg_fa.push_back(0.5 * (fa_t1[i] + fa_t2[i]));
    } else if (state_size == 15) {
      PushBack(1, state[3], state[4], state[4]);
      PushBack(2, state[8], state[9], state[9]);
      PushBack(3, state[13], state[14], state[14]);
    } else if (state_size == 18) {
      PushBack(1, state[3], state[4], state[5]);
      PushBack(2, state[9], state[10], state[11]);
      PushBack(3, state[15], state[16], state[17]);
      avg_fa.push_back(1.0 / 3.0 * (fa_t1[i] + fa_t2[i] + fa_t3[i]));
    }
  }

  double mean, std;
  std::cout << "MEASURMENTS\t\t\t\tMEAN\t\tSTD" << std::endl;

  if (fa_t1.size()) {
    MeanStd(fa_t1, mean, std);
    std::cout << "FA 1. tensor:\t\t\t\t" << mean << "\t" << std << std::endl;
  }
  if (fa_t2.size()) {
    MeanStd(fa_t2, mean, std);
    std::cout << "FA 2. tensor:\t\t\t\t" << mean << "\t" << std << std::endl;
  }
  if (fa_t3.size()) {
    MeanStd(fa_t3, mean, std);
    std::cout << "FA 3. tensor:\t\t\t\t" << mean << "\t" << std << std::endl;
  }

  if (ra_t1.size()) {
    MeanStd(ra_t1, mean, std);
    std::cout << "RA 1. tensor:\t\t\t\t" << mean << "\t" << std << std::endl;
  }
  if (ra_t2.size()) {
    MeanStd(ra_t2, mean, std);
    std::cout << "RA 2. tensor:\t\t\t\t" << mean << "\t" << std << std::endl;
  }
  if (ra_t3.size()) {
    MeanStd(ra_t3, mean, std);
    std::cout << "RA 3. tensor:\t\t\t\t" << mean << "\t" << std << std::endl;
  }

  if (trace_t1.size()) {
    MeanStd(trace_t1, mean, std);
    std::cout << "Trace 1. tensor:\t\t\t" << mean << "\t\t" << std << std::endl;
  }
  if (trace_t2.size()) {
    MeanStd(trace_t2, mean, std);
    std::cout << "Trace 2. tensor:\t\t\t" << mean << "\t\t" << std << std::endl;
  }
  if (trace_t3.size()) {
    MeanStd(trace_t3, mean, std);
    std::cout << "Trace 3. tensor:\t\t\t" << mean << "\t\t" << std << std::endl;
  }

  if (axial_t1.size()) {
    MeanStd(axial_t1, mean, std);
    std::cout << "Axial diffusity 1. tensor:\t\t" << mean << "\t\t" << std << std::endl;
  }
  if (axial_t2.size()) {
    MeanStd(axial_t2, mean, std);
    std::cout << "Axial diffusity 2. tensor:\t\t" << mean << "\t\t" << std << std::endl;
  }
  if (axial_t3.size()) {
    MeanStd(axial_t3, mean, std);
    std::cout << "Axial diffusity 3. tensor:\t\t" << mean << "\t\t" << std << std::endl;
  }

  if (avg_fa.size()) {
    MeanStd(avg_fa, mean, std);
    std::cout << "Average FA:\t\t\t\t" << mean << "\t" << std << std::endl;
  }

  return 0;
}

void MeanStd(const std::vector<double>& arr, double& mean, double& std) {
  mean = 0.0;
  std = 0.0;

  for (size_t i = 0; i < arr.size(); ++i) {
    mean += arr[i];
  }
  mean /= arr.size();
  for (size_t i = 0; i < arr.size(); ++i) {
    std += (arr[i] - mean) * (arr[i] - mean);
  }
  std /= arr.size();
  std = sqrt(std);
}

void PushBack(int count, double l1, double l2, double l3) {
  if (count == 1) {
    fa_t1.push_back(l2fa(l1, l2, l3));
    ra_t1.push_back(l2ra(l1, l2, l3));
    trace_t1.push_back(trace(l1, l2, l3));
    axial_t1.push_back(l1);
    radial_t1.push_back(l2);
  } else if (count == 2) {
    fa_t2.push_back(l2fa(l1, l2, l3));
    ra_t2.push_back(l2ra(l1, l2, l3));
    trace_t2.push_back(trace(l1, l2, l3));
    axial_t2.push_back(l1);
    radial_t2.push_back(l2);
  } else if (count == 2) {
    fa_t3.push_back(l2fa(l1, l2, l3));
    ra_t3.push_back(l2ra(l1, l2, l3));
    trace_t3.push_back(trace(l1, l2, l3));
    axial_t3.push_back(l1);
    radial_t3.push_back(l2);
  }
}
