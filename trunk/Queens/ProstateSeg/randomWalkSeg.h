#include "cVolume.h"

#include "spdEqSolver.h"


class randomWalkSeg
{
public:
  randomWalkSeg();
  ~randomWalkSeg();

  cVolume *_vol;
  cVolume *_seedVol;
  // in seed Vol, the voxel value is label:
  // 0: un-marked
  // 1: background
  // 2: object

  cVolume *_init_potential_vol;


  cVolume *_indexMapping;

  double _beta;

  void setBeta(double beta) {_beta = beta;}

  void setVol(cVolume *vol);
  void setSeedVol(cVolume *vol);

  void set_init_potential_volume(cVolume *vol);

  void set_solver_num_iter(int n);
  void set_solver_err_tol(double tol);

  int _numUnmarkedVox;

  int get_num_unseeded_voxels();

  void getIndexMapping();

  vnl_sparse_matrix<double> *_Lu;
  vnl_sparse_matrix<double> *_BT;
  vnl_vector<double> *_rhs;
  vnl_vector<double> *_potential;

  // indicating the potential data is updated
  bool _data_updated;

  void getLuB();

  void getRhs();
  cVolume* get_potential_volume();

  void getRsltVol();

  cVolume* get_output();

  void update();

  /*========================================
    20090106*/
  void setInput(DoubleImageType::Pointer image);
  void setSeedImage(DoubleImageType::Pointer image);

  DoubleImageType::Pointer getOutputImage();

protected:
  DoubleImageType::Pointer m_inputImage;
  DoubleImageType::Pointer m_seedImage;
  DoubleImageType::Pointer m_outputImage;

  void generate_data();
  void get_potential_vector();
  vnl_vector<double>* set_init_potential_vector();

  int num_iter_eq_CG_solver;
  double CG_solver_err_tolerance;
};
