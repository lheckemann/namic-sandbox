#include <cassert>
#include <iostream>
#include <vector>
#include "filter_model.h"
#include "gmm_hacks.h"
#include "tractography.h"


int main(int argc, char **argv) {
  // Default parameters.
  // File names.
  const char *dwi_file = NULL;
  const char *seeds_file = NULL;
  const char *mask_file = NULL;
  const char *output_file = "output.vtk";

  // Which information should be recorded for each fiber point?
  bool record_fa = false;
  bool record_state = false;
  bool record_cov = false;

  // Tensor model parameters
  int num_tensor = 2;
  bool is_full_model = true;

  // Terminating conditions for tractography.
  double fa_min = 0.0;
  double ga_min = 0.1;

  // Seeds per voxel.
  int seeds_per_voxel = 1;

  // Labels.
  std::set<int> labels;

  // Noise.
  double Qm = 0.0;
  double Ql = 0.0;
  double Rs = 0.0;

  // Required angle for branching.
  double theta = 0.0;

  bool full_brain = false;

  bool transform_position = true;
  bool store_glyphs = false;
  
  // Parse command line arguments.
  std::string help_string =
      std::string("Usage:\n") + argv[0] + "\n" +
      "  --dwi_file FILE\n  --mask_file FILE\n  [--seeds_file FILE]\n"
      "  [--output_file FILE]\n  [--record_fa (true|false)]\n"
      "  [--record_state (true|false)]\n  [--record_cov (true|false)]\n"
      "  [--model (one-tensor|two-tensor|three-tensor)]\n"
      "  [--tensor_model (full|simple)]\n  [--fa_min DOUBLE]\n"
      "  [--ga_min DOUBLE]\n  [--seeds INT]\n  [--labels INT(,INT)*]\n"
      "  [--Qm DOUBLE]\n  [--Ql DOUBLE]\n  [--Rs DOUBLE]\n"
      "  [--theta DOUBLE]\n  [--transform_position (true|false)]\n"
      "  [--store_glyphs (true|false)]";
      
  bool error = false;
  int i = 1;
  if (argc % 2) {
    while (i + 1 < argc) {
      if (!std::string(argv[i]).compare("--dwi_file")) {
        dwi_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--seeds_file")) {
        seeds_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--mask_file")) {
        mask_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--output_file")) {
        output_file = argv[++i];
      } else if (!std::string(argv[i]).compare("--record_fa")) {
        if (!std::string(argv[++i]).compare("true")) {
          record_fa = true;
        } else if (std::string(argv[i]).compare("false")) {
          error = true;
        }
      } else if (!std::string(argv[i]).compare("--record_state")) {
        if (!std::string(argv[++i]).compare("true")) {
          record_state = true;
        } else if (std::string(argv[i]).compare("false")) {
          error = true;
        }
      } else if (!std::string(argv[i]).compare("--record_cov")) {
        if (!std::string(argv[++i]).compare("true")) {
          record_cov = true;
        } else if (std::string(argv[i]).compare("false")) {
          error = true;
        }
      } else if (!std::string(argv[i]).compare("--model")) {
        if (!std::string(argv[++i]).compare("one-tensor")) {
          num_tensor = 1;
        } else if (!std::string(argv[i]).compare("three-tensor")) {
          num_tensor = 3;
        } else if (std::string(argv[++i]).compare("two-tensor")) {
          error = true;
        }
      } else if (!std::string(argv[i]).compare("--tensor_model")) {
        if (!std::string(argv[++i]).compare("simple")) {
          is_full_model = false;
        } else if (std::string(argv[i]).compare("full")) {
          error = true;
        }
      } else if (!std::string(argv[i]).compare("--fa_min")) {
        fa_min = atof(argv[++i]);
      } else if (!std::string(argv[i]).compare("--ga_min")) {
        ga_min = atof(argv[++i]);
      } else if (!std::string(argv[i]).compare("--seeds")) {
        seeds_per_voxel = atoi(argv[++i]);
      } else if (!std::string(argv[i]).compare("--labels")) {
        std::string str(argv[++i]);
        size_t found = str.find_first_of(",");
        labels.insert(atoi(str.c_str()));
        while (found != std::string::npos) {
          labels.insert(atoi(str.substr(found + 1).c_str()));
          found = str.find_first_of(",", found + 1);
        }
      } else if (!std::string(argv[i]).compare("--Qm")) {
        Qm = atof(argv[++i]);
      } else if (!std::string(argv[i]).compare("--Ql")) {
        Ql = atof(argv[++i]);
      } else if (!std::string(argv[i]).compare("--Rs")) {
        Rs = atof(argv[++i]);
      } else if (!std::string(argv[i]).compare("--theta")) {
        theta = atof(argv[++i]);
      } else if (!std::string(argv[i]).compare("--transform_position")) {
        if (!std::string(argv[++i]).compare("false")) {
          transform_position = false;
        } else if (std::string(argv[i]).compare("true")) {
          error = true;
        }
      } else if (!std::string(argv[i]).compare("--store_glyphs")) {
        if (!std::string(argv[++i]).compare("true")) {
          store_glyphs = true;
        } else if (std::string(argv[i]).compare("false")) {
          error = true;
        }
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
  if (!dwi_file || !mask_file || error) {
    std::cout << "Error!" << std::endl << std::endl;
    std::cout << help_string << std::endl;
    return 1;
  }
  if (!seeds_file) {
    full_brain = true;
    theta = 0.0;
  }

  if (labels.size() == 0) {
    labels.insert(1);
  }

  if (fa_min == 0.0) {
    if (is_full_model) { 
      fa_min = 0.1;
    } else {
      fa_min = 0.15;
    }
  }
  if (Qm == 0.0) {
    if (num_tensor == 1) {
      Qm = 0.0015;
    } else {
      if (is_full_model) {
        Qm = 0.002;
      } else {
        Qm = 0.003;
      }
    }
  }
  if (Ql == 0.0) {
    if (num_tensor == 1) {
      Ql = 25.0;
    } else if (num_tensor == 2) {
      Ql = 100.0;
    } else if (num_tensor == 3) {
      Ql = 150.0;
    }
  }
  if (Rs == 0.0) {
    if (num_tensor == 1) {
      Rs = 0.02;
    } else {
      if (is_full_model) {
        Rs = 0.01;
      } else {
        Rs = 0.015;
      }
    }
  }

  // Debug output.
  std::cout << "Launching UKF tractography with the following parameters:"
            << std::endl << argv[0] << std::endl
            << "--dwi_file " << dwi_file << std::endl
            << "--seeds_file " << (seeds_file ? seeds_file : "") << std::endl
            << "--mask_file " << mask_file << std::endl
            << "--output_file " << output_file << std::endl
            << "--record_fa " << (record_fa ? "true" : "false") << std::endl
            << "--record_state " << (record_state ? "true" : "false")
            << std::endl
            << "--record_cov " << (record_cov ? "true" : "false") << std::endl;
  if (num_tensor == 3) {
    std::cout << "--model three-tensor" << std::endl;
  } else if (num_tensor == 2) {
    std::cout << "--model two-tensor" << std::endl;
  } else {
    std::cout << "--model one-tensor" << std::endl;
  }
  std::cout << "--tensor_model " << (is_full_model ? "full" : "simple")
            << std::endl
            << "--fa_min " << fa_min << std::endl
            << "--ga_min " << ga_min << std::endl
            << "--seeds " << seeds_per_voxel << std::endl;
  std::set<int>::const_iterator cit = labels.begin();
  std::cout << "--labels " << *cit++;
  for ( ; cit != labels.end(); ++cit) {
    std::cout << "," << *cit;
  }
  std::cout << std::endl
            << "--Qm " << Qm << std::endl
            << "--Ql " << Ql << std::endl
            << "--Rs " << Rs << std::endl
            << "--theta " << theta << std::endl
            << "--transform_position "
            << (transform_position ? "true" : "false") << std::endl
            << "--store_glyphs " << (store_glyphs ? "true" : "false")
            << std::endl;

  // Initialize the tractography object.
  FilterModel *filter_model;
  if (is_full_model) {
    if (num_tensor == 3) {
      std::cout << "Using 3-tensor full model." << std::endl;
      filter_model = new Full3T(Qm, Ql, Rs);
    } else if (num_tensor == 2) {
      std::cout << "Using 2-tensor full model." << std::endl;
      filter_model = new Full2T(Qm, Ql, Rs);
    } else {
      std::cout << "Using 1-tensor full model." << std::endl;
      filter_model = new Full1T(Qm, Ql, Rs);
    }
  } else {
    if (num_tensor == 3) {
      std::cout << "Using 3-tensor simple model." << std::endl;
      filter_model = new Simple3T(Qm, Ql, Rs);
    } else if (num_tensor == 2) {
      std::cout << "Using 2-tensor simple model." << std::endl;
      filter_model = new Simple2T(Qm, Ql, Rs);
    } else {
      std::cout << "Using 1-tensor simple model." << std::endl;
      filter_model = new Simple1T(Qm, Ql, Rs);
    }
  }
  Tractography *tract = new Tractography(filter_model, output_file, num_tensor,
                                         is_full_model, fa_min, ga_min,
                                         seeds_per_voxel, theta,
                                         transform_position, store_glyphs);
  if (tract->LoadFiles(dwi_file, seeds_file ? seeds_file : "", mask_file)) {
    delete tract;
    delete filter_model;
    return 1;
  }

  for (cit = labels.begin(); cit != labels.end(); ++cit) {
    tract->AddLabel(*cit);
  }
  if (record_fa) {
    tract->RecordFa();
  }
  if (record_state) {
    tract->RecordState();
  }
  if (record_cov) {
    tract->RecordCov();
  }

  // Run the tractography.
  tract->Run();

  // Clean up.
  delete tract;
  delete filter_model;

  return 0;
}
