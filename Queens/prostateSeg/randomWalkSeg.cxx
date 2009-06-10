#include "randomWalkSeg.h"

#include <iostream>
#include <cmath>

#include "cVolume.h"
#include "cVolOp.h"

randomWalkSeg::randomWalkSeg() 
{
  _beta = 0.1;

  _vol = 0;
  _seedVol = 0;
  _init_potential_vol = 0;
  // in seed Im, the pixel value is label:
  // 0: un-marked
  // 1: background
  // 2: object

  _indexMapping = 0;

  _Lu = 0;
  _BT = 0;
  _rhs = 0;
  _potential = 0;

  num_iter_eq_CG_solver = 1000;
  CG_solver_err_tolerance = 1e-6;

  _data_updated = false;
}

randomWalkSeg::~randomWalkSeg() 
{

  if (_Lu)
    {
      delete _Lu;
    }

  if (  _BT)
    {
      delete _BT;
    }

  if (_rhs)
    {
      delete _rhs;
    }
  

  if (_potential)
    {
      delete _potential;
    }
}


void randomWalkSeg::setVol(cVolume *vol)
{
  _vol = vol;
}


void randomWalkSeg::setSeedVol(cVolume *vol)
{
  if (!_vol)
    {
      std::cerr<<"not initiated. exit\n";
      exit(-1);
    }

  if (vol->getSizeX() != _vol->getSizeX() || vol->getSizeY() != _vol->getSizeY() || vol->getSizeZ() != _vol->getSizeZ())
    {
      std::cerr<<"size doesn't match. exit\n";
      exit(-1);
    }
  
  _seedVol = vol;
}

void randomWalkSeg::getIndexMapping()
{
  if (_vol->initiated() == false || _seedVol->initiated() == false)
    {
      std::cerr<<"not initiated. exit\n";
      exit(-1);
    }

  int nx = _vol->getSizeX();
  int ny = _vol->getSizeY();
  int nz = _vol->getSizeZ();
  int n = nx*ny*nz;

  //  std::cout<<"total voxel = "<<n<<std::endl<<std::flush;

  // get number of un-marked voxels
  int numUnmarkedVox = get_num_unseeded_voxels();
  int numMarkedVoxel = n - numUnmarkedVox;


  _indexMapping = new cVolume(nx, ny, nz);
  _indexMapping->setAllVoxels(-1);

  int countUnmarkedVoxel = numMarkedVoxel;
  int countMarkedVoxel = 0;

  for (int iy = 0; iy <= ny-1; ++iy)
    {
      for (int ix = 0; ix <= nx-1; ++ix)
        {
          for (int iz = 0; iz <= nz-1; ++iz)
            {
              double l = _seedVol->getVoxel(ix, iy, iz);

              if (0.00001 <= l || -0.00001 >= l)
                {
                  if (countMarkedVoxel >= n)
                    {
                      std::cerr<<"countMarkedVoxel >= n!!!\n";
                    }
                  _indexMapping->setVoxel(ix, iy, iz, countMarkedVoxel++);
                }
              else
                {
                  // (l == 0.0)
                  if (countUnmarkedVoxel >= n)
                    {
                      std::cerr<<"countUnmarkedVoxel >= n !!!\n";
                    }

                  _indexMapping->setVoxel(ix, iy, iz, countUnmarkedVoxel++);
                }
            }
        }
    }

//   std::cout<<"_indexMapping->getMaxVoxel = "<<_indexMapping->getMaxVoxel()<<std::endl;
//   std::cout<<"_indexMapping->getMinVoxel = "<<_indexMapping->getMinVoxel()<<std::endl;

}

int randomWalkSeg::get_num_unseeded_voxels()
{
  if (!_seedVol)
    {
      std::cerr<<"not initiated. exit\n";
      exit(-1);
    }

  int nx = _seedVol->getSizeX();
  int ny = _seedVol->getSizeY();
  int nz = _seedVol->getSizeZ();

  _numUnmarkedVox = 0;

  for (int ix = 0; ix <= nx-1; ++ix)
    {
      for (int iy = 0; iy <= ny-1; ++iy)
        {
          for (int iz = 0; iz <= nz-1; ++iz)
            {
              double l = _seedVol->getVoxel(ix, iy, iz);

              if (0.00001 >= l && -0.00001 <= l)
                {
                  ++_numUnmarkedVox;
                }
            }
        }
    }

  return _numUnmarkedVox;
}


void randomWalkSeg::getLuB()
{
  if (_vol->initiated() == false || _seedVol->initiated() == false || _indexMapping->initiated() == false)
    {
      std::cerr<<"not initiated. exit\n";
      exit(-1);
    }

  int nx = _vol->getSizeX();
  int ny = _vol->getSizeY();
  int nz = _vol->getSizeZ();
  int n = nx*ny*nz;

  // get number of un-marked voxels
  int numUnmarkedVox = get_num_unseeded_voxels();
  int numMarkedVoxel = n - numUnmarkedVox;

  if (numMarkedVoxel == 0 || numUnmarkedVox == 0)
    {
      std::cerr<<"strange. exit\n";
      exit(-1);
    }

  _Lu = new vnl_sparse_matrix<double>(numUnmarkedVox, numUnmarkedVox);
  _BT = new vnl_sparse_matrix<double>(numUnmarkedVox, numMarkedVoxel); // not square
           

  for (int iz = 0; iz <= nz-1; ++iz)
    {
      for (int iy = 0; iy <= ny-1; ++iy)
        {
          for (int ix = 0; ix <= nx-1; ++ix)
            {
              int idx = _indexMapping->getVoxel(ix, iy, iz);

              if (idx < numMarkedVoxel)
                {
                  // means the current voxel is marked
                  continue;
                }

              double v = _vol->getVoxel(ix, iy, iz);

              for (int izz = iz-1; izz <= iz+1; ++izz)
                {
                  for (int iyy = iy-1; iyy <= iy+1; ++iyy)
                    {
                      for (int ixx = ix-1; ixx <= ix+1; ++ixx)
                        {
                          if (ixx == ix && iyy == iy && izz == iz)
                            {
                              continue;
                            }

                          if ((ixx - ix)*(iyy - iy) != 0 || (ixx - ix)*(izz - iz) != 0 || (iyy - iy)*(izz - iz) != 0)
                            {
                              // get rid of diagnal nbhd
                              continue;
                            }

                          if (ixx >= 0 && ixx <= nx-1 && iyy >= 0 && iyy <= ny-1 && izz >= 0 && izz <= nz-1)
                            {
                              int idx1 = _indexMapping->getVoxel(ixx, iyy, izz);

                              double v1 = _vol->getVoxel(ixx, iyy, izz);
                      
                              //                      double w = (v - v1);
                              double w = exp(-_beta*(v - v1)*(v - v1));

                              if (idx1 < numMarkedVoxel)
                                {
                                  // idx1 is a marked voxel, so w goes into BT
                                  // std::cout<<"I'm setting _BT. \n"<<std::flush;
                                  //printf("setting _BT at (%d, %d) to %f \n", idx - numMarkedVoxel, idx1, -w);
                                  (*_BT)(idx - numMarkedVoxel, idx1) = -w;
                                }
                              else
                                {
                                  // idx1 is an un-marked voxel, so w goes into _LU
                                  (*_Lu)(idx - numMarkedVoxel, idx1 - numMarkedVoxel) = -w;
                                }

                              (*_Lu)(idx - numMarkedVoxel, idx - numMarkedVoxel) += w;
                              // diagnal element of _Lu
                            }
                        }
                    }
                }
            }
        }
    }
}


void randomWalkSeg::getRhs()
{

  int nx = _vol->getSizeX();
  int ny = _vol->getSizeY();
  int nz = _vol->getSizeZ();
  int n = nx*ny*nz;

  int numMarkedVoxel = n - _numUnmarkedVox;

  vnl_vector<double> m(numMarkedVoxel, 0.0);

  for (int iz = 0; iz <= nz-1; ++iz)
    {
      for (int iy = 0; iy <= ny-1; ++iy)
        {
          for (int ix = 0; ix <= nx-1; ++ix)
            {
              double l = _seedVol->getVoxel(ix, iy, iz);

              if (1.999999 <= l && l <= 2.000001) // 2.0 indicates foreground
                {
                  int idx = _indexMapping->getVoxel(ix, iy, iz);
                  // idx should < numMarkedVoxel
                  m(idx) = 1;
                }
            }
        }
    }

  vnl_vector<double> tmp;
  //  std::cout<<"tmp.data_block = "<<tmp.data_block()<<std::endl<<std::flush;
  _BT->mult(m, tmp);

  //  std::cout<<"tmp.data_block = "<<tmp.data_block()<<std::endl<<std::flush;

//   std::cout<<"m is zero??????  "<<m.is_zero()<<std::endl<<std::flush;
//   std::cout<<"tmp is zero??????  "<<tmp.is_zero()<<std::endl<<std::flush;

  //  vcl_cerr<<m<<vcl_endl;

  _rhs = new vnl_vector<double>(-tmp);
}


void randomWalkSeg::get_potential_vector()
{
  if (!_Lu)
    {
      std::cerr<<"get left hand side of equation first. exit\n";
      exit(-1);
    }

  if (!_rhs)
    {
      std::cerr<<"get right hand side of equation first. exit\n";
      exit(-1);
    }

  _potential = set_init_potential_vector();

  //  std::cout<<"_rhs is zero? "<<_rhs->is_zero()<<std::endl<<std::flush;

  int it_num = solveSpdEq(_Lu, _rhs, _potential, num_iter_eq_CG_solver, CG_solver_err_tolerance);
  std::cout<<"\t\t\t ran "<<it_num<<" iterations\n"<<std::flush;
}


cVolume* randomWalkSeg::get_potential_volume()
{
  if (!_data_updated)
    {
      std::cerr<<"result not updated. exit...\n";
      exit(-1);
    }

  cVolume* potential_vol = new cVolume(_vol);
  potential_vol->setAllVoxels(0.0);

  int nx = _vol->getSizeX();
  int ny = _vol->getSizeY();
  int nz = _vol->getSizeZ();
  int n = nx*ny*nz;

  int numMarkedVoxel = n - _numUnmarkedVox;

  for (int iz = 0; iz <= nz-1; ++iz)
    {
      for (int iy = 0; iy <= ny-1; ++iy)
        {
          for (int ix = 0; ix <= nx-1; ++ix)
            {
              double l = _seedVol->getVoxel(ix, iy, iz);

              if (0.9999999 <= l && 1.00001 >= l)
                {
                  potential_vol->setVoxel(ix, iy, iz, 0.0);
                }
          
              if (1.9999999 <= l && 2.00001 >= l)
                {
                  potential_vol->setVoxel(ix, iy, iz, 1.0);
                }
          
              if (0.00001 >= l && -0.00001 <= l)
                {
                  int idx = _indexMapping->getVoxel(ix, iy, iz);
                  // idx should >= numMarkedVoxel

                  double p = (*_potential)(idx - numMarkedVoxel);
                  potential_vol->setVoxel(ix, iy, iz, p);
                }
            }
        }
    }
  return potential_vol;
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void randomWalkSeg::generate_data()
{
  if (_vol && _seedVol)
    {
      get_num_unseeded_voxels();

      //std::cout<<"get index mapping....";
      getIndexMapping();
      //std::cout<<"done\n";

      //std::cout<<"get Lu and B....";
      getLuB();
      //std::cout<<"done\n";

      //std::cout<<"get right hand side....";  
      getRhs();
      //std::cout<<"done\n";

      //std::cout<<"CG for result....";  
      get_potential_vector();
      //std::cout<<"done\n";
    }

  _data_updated = true;
}


void randomWalkSeg::setInput(DoubleImageType::Pointer image)
{
  m_inputImage = image;

  cVolOp op;
  setVol( op.itkVolume2cVolume(m_inputImage) );
}



void randomWalkSeg::setSeedImage(DoubleImageType::Pointer image)
{
  m_seedImage = image;


  typedef itk::ImageRegionIterator< DoubleImageType > DoubleImageIteratorType;
  DoubleImageIteratorType seedImageIt( m_seedImage, m_seedImage->GetLargestPossibleRegion() );

  for (seedImageIt.GoToBegin(); !seedImageIt.IsAtEnd(); ++seedImageIt)
    {
      double v = seedImageIt.Get();

      if ( 0.9999 <= v && v <= 1.00001 )
        {
          seedImageIt.Set(0);
        }

      if ( -0.0001 <= v && v <= 0.00001 )
        {
          seedImageIt.Set(1);
        }
    }


  cVolOp op;
  setSeedVol( op.itkVolume2cVolume(m_seedImage) );
}


DoubleImageType::Pointer randomWalkSeg::getOutputImage()
{
  if (!_data_updated)
    {
      std::cerr<<"result not updated. exit...\n";
      exit(-1);
    }

  cVolume *tem = get_potential_volume();
  cVolOp op;
  DoubleImageType::Pointer img = op.cVolume2itkVolume( tem );
  delete tem;
  tem = 0;

  /*post smoothing image*/
  typedef itk::MedianImageFilter< DoubleImageType, DoubleImageType >  FilterType;
  FilterType::Pointer filter = FilterType::New();

  itkInternalVolumeType::SizeType indexRadius;
  
  indexRadius[0] = 3; // radius along x
  indexRadius[1] = 3; // radius along y
  indexRadius[2] = 3; // radius along z

  filter->SetRadius( indexRadius );
  filter->SetInput( img );
  filter->Update();
  
  m_outputImage = filter->GetOutput();
  typedef itk::ImageRegionIterator< DoubleImageType > DoubleImageIteratorType;
  DoubleImageIteratorType outputIt( m_outputImage, m_outputImage->GetLargestPossibleRegion() );

  double thld = 0.5;
  for (outputIt.GoToBegin(); !outputIt.IsAtEnd(); ++outputIt)
    {
      double v = outputIt.Get();
      outputIt.Set(v > thld?1:0);
    }

  m_outputImage->SetSpacing( m_inputImage->GetSpacing() );

  return m_outputImage;
}

cVolume* randomWalkSeg::get_output()
{
  if (!_data_updated)
    {
      std::cerr<<"result not updated. exit...\n";
      exit(-1);
    }

  return get_potential_volume();
}

void randomWalkSeg::update()
{
  if (!_data_updated)
    {
      generate_data();
    }
}


void randomWalkSeg::set_init_potential_volume(cVolume *vol)
{
  if (!_vol)
    {
      std::cerr<<"not initiated, set that first. exit\n";
      exit(-1);
    }

  if (!_seedVol)
    {
      std::cerr<<"Seed not initiated, set that first. exit\n";
      exit(-1);
    }


  if (vol->getSizeX() != _vol->getSizeX() || vol->getSizeY() != _vol->getSizeY() || vol->getSizeZ() != _vol->getSizeZ())
    {
      std::cerr<<"size doesn't match. exit\n";
      exit(-1);
    }
  
  _init_potential_vol = vol;
  // just use the pointer
}


vnl_vector<double>* randomWalkSeg::set_init_potential_vector()
{
  if (!_rhs)
    {
      std::cerr<<"get right hand side of equation first. exit\n";
      exit(-1);
    }
  unsigned int n = _rhs->size();  // n is the number of un-seeded pixels.


  if (!_init_potential_vol)
    {
      vnl_vector<double>* a = new vnl_vector<double>(n, 0.0);
      return a;
    }
  else
    {
      cVolOp op;
      cVolume *normalized_potential_vol = op.normalizeVol(_init_potential_vol, 0, 1.0);

      if (!_indexMapping)
        {
          getIndexMapping();          
        }

      //////////////////////////////////////////////
      // Now we have the initial potential image, and the index
      // mapping ready, next convert the potential image to potential
      // vector.
      vnl_vector<double>* a = new vnl_vector<double>(n);

      int nx = normalized_potential_vol->getSizeX();
      int ny = normalized_potential_vol->getSizeY();
      int nz = normalized_potential_vol->getSizeZ();

      int numMarkedPixel = nx*ny*nz - _numUnmarkedVox;

      for (int iz = 0; iz <= nz-1; ++iz)
        {
          for (int iy = 0; iy <= ny-1; ++iy)
            {
              for (int ix = 0; ix <= nx-1; ++ix)
                {
                  double l = _seedVol->getVoxel(ix, iy, iz);

                  if (l <= 0.000001 && l >= -0.00001)
                    {
                      int idx = _indexMapping->getVoxel(ix, iy, iz);
                      // idx should >= numMarkedPixel

                      double p = normalized_potential_vol->getVoxel(ix, iy, iz);

                      (*a)(idx - numMarkedPixel) = p;
                    }
                }
            }
        }

      delete normalized_potential_vol;
      return a;
    }
}

void randomWalkSeg::set_solver_num_iter(int n)
{
  num_iter_eq_CG_solver = n;
}

void randomWalkSeg::set_solver_err_tol(double tol)
{
  CG_solver_err_tolerance = tol; 
}
