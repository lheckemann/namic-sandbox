#include "ICP3.h"

#include "ICPCost3.h"


#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_lbfgsb.h>

#include <vnl/vnl_det.h>

#include <vnl/vnl_random.h>

#include <iostream>
#include <csignal>

#include <boost/multi_array.hpp>
#include <boost/random.hpp>


/*==================================================*/
ICP3::ICP3()
{
  //  m_expFactor = -0.0001;

  m_fixedPointSet = 0;
  m_movingPointSet = 0;

  computeScalings();  
  computeInitialParameters();

  m_ICPDone = false;

  m_maxFnEval = 2000;
  m_maxIteration = 10;


  m_kdTreeForFixedPointSet = 0;

  m_costFnType = L2;

  m_lambdaDeterminent = 0;
}

/*========================================*/
ICP3::ICP3( PointSet3DType* fixedPointSet, PointSet3DType* movingPointSet )
{
  this->setFixedPointSet(fixedPointSet);
  this->setMovingPointSet(movingPointSet);

  computeScalings();  
  computeInitialParameters();

  m_ICPDone = false;

  m_maxFnEval = 2000;
  m_maxIteration = 10;


  m_kdTreeForFixedPointSet = 0;

  m_costFnType = L2;

  m_lambdaDeterminent = 0;
}

/*==================================================*/
void ICP3::computeInitialParameters()
{
  m_parameters.set_size(m_numOfParameters);
  m_parameters.fill(0.0);

  // affine mtx is be the identity mtx
  m_parameters[0] = 1.0;
  m_parameters[4] = 1.0;
  m_parameters[8] = 1.0;
}

/*==================================================*/
void ICP3::parameterVectorToAffineMatrixAndTranslationVector(const VnlDoubleVectorType &currentParam, \
                                                             VnlDoubleMatrix3x3Type& affine, VnlDoubleVector3DType& translation) const
{
  affine(0, 0) = currentParam[0];
  affine(0, 1) = currentParam[1];
  affine(0, 2) = currentParam[2];

  affine(1, 0) = currentParam[3];
  affine(1, 1) = currentParam[4];
  affine(1, 2) = currentParam[5];

  affine(2, 0) = currentParam[6];
  affine(2, 1) = currentParam[7];
  affine(2, 2) = currentParam[8];

  translation[0] = currentParam[9];
  translation[1] = currentParam[10];
  translation[2] = currentParam[11];
}


/*==================================================*/
void ICP3::affineMatrixAndTranslationVectorToParameterVector(const VnlDoubleMatrix3x3Type& affine, const VnlDoubleVector3DType& translation, \
                                                         VnlDoubleVectorType& currentParam) const
{
  currentParam[0] = affine(0, 0);
  currentParam[1] = affine(0, 1);
  currentParam[2] = affine(0, 2);

  currentParam[3] = affine(1, 0);
  currentParam[4] = affine(1, 1);
  currentParam[5] = affine(1, 2);

  currentParam[6] = affine(2, 0);
  currentParam[7] = affine(2, 1);
  currentParam[8] = affine(2, 2);

  currentParam[9] = translation[0];
  currentParam[10]= translation[1];
  currentParam[11] = translation[2];
}

/*==================================================*/
void ICP3::buildKDTreeForClosestPtSearch()
{
  if (m_fixedPointSet == 0)
    {
      std::cerr<<"m_fixedPointSet not ready. \n";
      raise(SIGABRT);
    }

  // port the pt set in the fixed pt set to the data structure
  // required for kdtree
  typedef boost::multi_array<float, 2> array2dfloat;

  unsigned long numOfPt = m_fixedPointSet->size();
  unsigned int ptDim = 3; // 3D pt

  array2dfloat data(extents[numOfPt][ptDim]);  

  for (unsigned long i = 0; i < numOfPt; i++) 
    {
      const VnlDoubleVector3DType& thisPt = m_fixedPointSet->at(i);
      data[i][0] = static_cast<float>(thisPt[0]);
      data[i][1] = static_cast<float>(thisPt[1]);
      data[i][2] = static_cast<float>(thisPt[2]);
    }

  // construct the tree using the point set
  m_kdTreeForFixedPointSet = new kdtree2(data, true);  // I don't know what this "true" means.
}


/*============================================================*/
void ICP3::optimizeTranslationParameters()
{
  // 1. store the m_scalings
  VnlDoubleVectorType oldScalings = m_scalings;

  // 2. m_scalings only turn on those for translation
  m_scalings.fill(0);
  for (int i = 9; i < 12; ++i)
    {
      m_scalings[i] = 1;
    }


  // 3. opt
  CICPCost3 costFn(this);
  costFn.dim = ICP3::m_numOfParameters;

  vnl_lbfgsb optimizer(costFn);
  optimizer.set_max_function_evals(m_maxFnEval);
//   if (-1 == m_maxFnEval)
//     {
//       m_maxFnEval = 2000;
//       optimizer.set_max_function_evals(m_maxFnEval);
//     }
//   else
//     {
//       optimizer.set_max_function_evals(m_maxFnEval);
//     }

//   optimizer.set_verbose(false);
//   optimizer.set_trace(false);
  optimizer.minimize(m_parameters);

//   CICPCost3_GD costFn(this);
//   douher::CGradientDescent opt(&costFn);
//   opt.setScales(m_scalings);
//   opt.setMaxNumberOfIterations(50);
//   opt.minimize(m_parameters);



  // 4. restore m_scalings
  m_scalings = oldScalings;
}

/*============================================================*/
void ICP3::optimizeAffineParameters()
{
  // 1. store the m_scalings
  VnlDoubleVectorType oldScalings = m_scalings;

  // 2. m_scalings only turn on those for translation
  m_scalings.fill(0);
  for (int i = 0; i < 9; ++i)
    {
      m_scalings[i] = 1;
    }

  //  3. opt
  CICPCost3 costFn(this);
  costFn.dim = ICP3::m_numOfParameters;

  vnl_lbfgsb optimizer(costFn);
  optimizer.set_max_function_evals(m_maxFnEval);
//   optimizer.set_verbose(false);
//   optimizer.set_trace(false);
  optimizer.minimize(m_parameters);



//   CICPCost3_GD costFn(this);
//   douher::CGradientDescent opt(&costFn);
//   opt.setScales(m_scalings);
//   opt.setMaxNumberOfIterations(50);
//   opt.minimize(m_parameters);



  // 4. restore m_scalings
  m_scalings = oldScalings;
}

/*============================================================*/
void ICP3::optimizeParameters()
{
  for (int i = 0; i < m_maxIteration; ++i)
    {
      VnlDoubleVectorType oldParam = m_parameters;

      optimizeTranslationParameters();
      std::cout<<"x = "<<m_parameters<<std::endl;

      optimizeAffineParameters();
      std::cout<<"x = "<<m_parameters<<std::endl;

      if ( (oldParam - m_parameters).two_norm() <= 1e-6 )
        {
          break;
        }
    }
}


/*==================================================*/
void ICP3::gogogo(void)
{
  buildKDTreeForClosestPtSearch();

  //  computeScalings();

  optimizeParameters();

  m_ICPDone = true;


  return;
}

/*==================================================*/
PointSet3DType* ICP3::getNewMovingPointSet(VnlDoubleVectorType param) const
{
  unsigned long nd = m_movingPointSet->size();

  PointSet3DType* newMovingPointSet = new PointSet3DType;

  VnlDoubleVector3DType translation;
  VnlDoubleMatrix3x3Type A;

  parameterVectorToAffineMatrixAndTranslationVector(m_parameters, A, translation);


  for (unsigned long ip = 0; ip < nd; ++ip)
    {
      const VnlDoubleVector3DType& p = m_movingPointSet->at(ip);

      VnlDoubleVector3DType newP = A*p + translation;

      //      std::cout<<newP<<std::endl;
      
      newMovingPointSet->push_back(newP);
    }

  //  std::cout<<newMovingPointSet->size()<<std::endl;

  return newMovingPointSet;
}

/*==================================================*/
PointSet3DType* ICP3::getNewMovingPointSet() const
{
  return getNewMovingPointSet(m_parameters);
}

/*==================================================*/
void ICP3::findClosestPoint(VnlDoubleVector3DType pt, unsigned long& idxOfClosestPtInPtSet, double& distanceSqToTheClosestPt) const
{
  /*
    find the closest point to pt, in m_fixedPointSet.

    return the idx of the closest point in idxOfClosestPtInPtSet
    return the square of closest distance in distanceSqToTheClosestPt
  */

  std::vector<float> ptV(3);
  ptV[0] = pt[0];
  ptV[1] = pt[1];
  ptV[2] = pt[2];

  kdtree2_result_vector res;
  int numOfClosestPtWantToFind = 1;
  //  m_kdTreeForFixedPointSet->n_nearest_brute_force(ptV, numOfClosestPtWantToFind, res); 
  m_kdTreeForFixedPointSet->n_nearest(ptV, numOfClosestPtWantToFind, res); 

  idxOfClosestPtInPtSet = res[0].idx;
  distanceSqToTheClosestPt = res[0].dis;
}

/*==================================================*/
void ICP3::computeScalings()
{
  m_scalings.set_size(m_numOfParameters);
  for (int i = 0; i < 9; ++i)
    {
      //      m_scalings[i] = 1;
      m_scalings[i] = 0;
    }

  for (int i = 9; i < 12; ++i)
    {
      m_scalings[i] = 1;
    }

  //  std::cerr<<m_scalings<<std::endl;
}

/*==================================================*/
double ICP3::getCost( const VnlDoubleVectorType& param) const
{
  return getCostL2(param);
}

/*==================================================*/
double ICP3::getCost() const
{
  return getCostL2();
}


/*==================================================*/
void ICP3::getCostAndGradient(const VnlDoubleVectorType &currentParam, double* cost, VnlDoubleVectorType* gradient) const
{
  getCostAndGradientL2(currentParam, cost, gradient);

  return;
}

/*============================================================*/
double ICP3::getCostL2( const VnlDoubleVectorType& param) const
{
  VnlDoubleVector3DType translation;
  VnlDoubleMatrix3x3Type A;

  parameterVectorToAffineMatrixAndTranslationVector(param, A, translation);

  unsigned long nmp = m_movingPointSet->size();

  double cost = 0;

  for (unsigned long imp = 0; imp < nmp; ++imp)
    {
      const VnlDoubleVector3DType& thisMovingPoint = m_movingPointSet->at(imp);

      VnlDoubleVectorType newP = A*thisMovingPoint + translation;

      // find closest point in fixed point set to this newP
      unsigned long idxOfClosestPtInPtSet = 0;
      double distanceSqToTheClosestPt = 0;
      findClosestPoint(newP, idxOfClosestPtInPtSet, distanceSqToTheClosestPt);

//       std::cerr<<"the closest point is: "<<idxOfClosestPtInPtSet<<std::endl;
//       std::cerr<<"the closest dist: "<<distanceSqToTheClosestPt<<std::endl;

      /* now, the closest point is idx by idxOfClosestPtInFixedPointSet */
      cost += distanceSqToTheClosestPt;
    }

  cost /= static_cast< double >(nmp);


  double det = vnl_det(A);
  //  cost += m_lambdaDeterminent*(1/(det*det) - 1);
  cost += m_lambdaDeterminent*(1/(det*det));

  return cost;
}


/*============================================================*/
double ICP3::getCostL2() const
{
  return getCostL2(m_parameters);
}


/*==================================================*/
void ICP3::getCostAndGradientL2(const VnlDoubleVectorType &currentParam, double *cost, VnlDoubleVectorType *gradient) const
{
  /*
    compute function value (*f) and its gradient vector (*g), gvn
    current state x
  */
  VnlDoubleVector3DType translation;
  VnlDoubleMatrix3x3Type A;

  parameterVectorToAffineMatrixAndTranslationVector(m_parameters, A, translation);

  unsigned long nmp = m_movingPointSet->size();


  double theCost = 0;

  double dCostDA00 = 0;
  double dCostDA01 = 0;
  double dCostDA02 = 0;

  double dCostDA10 = 0;
  double dCostDA11 = 0;
  double dCostDA12 = 0;

  double dCostDA20 = 0;
  double dCostDA21 = 0;
  double dCostDA22 = 0;

  double dCostDTx = 0;
  double dCostDTy = 0;
  double dCostDTz = 0;


  for (unsigned long imp = 0; imp < nmp; ++imp)
    {
      const VnlDoubleVector3DType& thisMovingPoint = m_movingPointSet->at(imp);

      VnlDoubleVectorType newP = A*thisMovingPoint + translation;


      // find closest point in fixed point set to this newP
      unsigned long idxOfClosestPtInFixedPointSet = 0;
      double distanceSqToTheClosestPt = 0;
      findClosestPoint(newP, idxOfClosestPtInFixedPointSet, distanceSqToTheClosestPt);

      //       std::cerr<<"the closest point is: "<<idxOfClosestPtInFixedPointSet<<std::endl;
      //       std::cerr<<"the closest dist: "<<minDistSq<<std::endl;

      const VnlDoubleVector3DType& closestFixedPoint = m_fixedPointSet->at(idxOfClosestPtInFixedPointSet);

      VnlDoubleVector3DType newPMinusClosestFixedPoint = newP - closestFixedPoint;

      /*
        Used tmp vars to reduce compuation. when proof reading, expend
        the whole expressions, which may be easier to follow and
        check.
       */
      double tmp0 = newPMinusClosestFixedPoint[0];
      double tmp1 = newPMinusClosestFixedPoint[1];
      double tmp2 = newPMinusClosestFixedPoint[2];

      dCostDTx += tmp0;
      dCostDTy += tmp1;
      dCostDTz += tmp2;

      dCostDA00 += tmp0*thisMovingPoint[0];
      dCostDA01 += tmp0*thisMovingPoint[1];
      dCostDA02 += tmp0*thisMovingPoint[2];

      dCostDA10 += tmp1*thisMovingPoint[0];
      dCostDA11 += tmp1*thisMovingPoint[1];
      dCostDA12 += tmp1*thisMovingPoint[2];

      dCostDA20 += tmp2*thisMovingPoint[0];
      dCostDA21 += tmp2*thisMovingPoint[1];

      dCostDA22 += tmp2*thisMovingPoint[2];


      theCost += distanceSqToTheClosestPt;
    }

  double det = vnl_det(A);
  double detSq = det*det;

  double aa = A(0, 0);
  double ab = A(0, 1);
  double ac = A(0, 2);

  double ad = A(1, 0);
  double ae = A(1, 1);
  double af = A(1, 2);

  double ag = A(2, 0);
  double ah = A(2, 1);
  double ai = A(2, 2);

  double dHdD = -2*m_lambdaDeterminent/(detSq*det);
  double dHdA00 = dHdD*(ae*ai - af*ah);
  double dHdA01 = dHdD*(af*ag - ad*ai);
  double dHdA02 = dHdD*(ad*ah - ae*ag);

  double dHdA10 = dHdD*(ac*ah - ab*ai);
  double dHdA11 = dHdD*(aa*ai - ac*ag);
  double dHdA12 = dHdD*(ab*ag - aa*ah);

  double dHdA20 = dHdD*(ab*af - ac*ae);
  double dHdA21 = dHdD*(ac*ad - aa*af);
  double dHdA22 = dHdD*(aa*ae - ab*ad);

  gradient->set_size(m_numOfParameters);

  gradient->put(0, dCostDA00 + nmp*dHdA00 ); 
  gradient->put(1, dCostDA01 + nmp*dHdA01 ); 
  gradient->put(2, dCostDA02 + nmp*dHdA02 ); 

  gradient->put(3, dCostDA10 + nmp*dHdA10 ); 
  gradient->put(4, dCostDA11 + nmp*dHdA11 ); 
  gradient->put(5, dCostDA12 + nmp*dHdA12 ); 

  gradient->put(6, dCostDA20 + nmp*dHdA20 ); 
  gradient->put(7, dCostDA21 + nmp*dHdA21 ); 
  gradient->put(8, dCostDA22 + nmp*dHdA22 ); 


  gradient->put(9, dCostDTx);
  gradient->put(10, dCostDTy);
  gradient->put(11, dCostDTz);


  //  (*cost) = theCost;
  (*cost) = theCost/static_cast< double >(nmp);
  //  (*cost) += m_lambdaDeterminent*(1/detSq - 1);
  (*cost) += m_lambdaDeterminent*(1/detSq);

  (*gradient) /= static_cast< double >(nmp);

  //  std::cout<<"gradient before scaling = "<<(*gradient)<<std::endl;

  this->adjustGradient(gradient);

//   std::cout<<"param = "<<m_parameters<<std::endl;
//   std::cout<<"cost = "<<(*cost)<<std::endl;
//   std::cout<<"gradient after scaling = "<<(*gradient)<<std::endl;

  return;
}



/*==================================================*/
void ICP3::adjustGradient(VnlDoubleVectorType* originalGradient) const
{
  unsigned long n = originalGradient->size();
 
  for (unsigned long i = 0; i < n; ++i)
    {
      (*originalGradient)[i] = m_scalings[i]*(*originalGradient)[i];
    }


  return;
}

/*============================================================*/
VnlDoubleMatrix3x3Type ICP3::getAffineMatrix() const
{
  VnlDoubleMatrix3x3Type A;
  VnlDoubleVector3DType t;

  parameterVectorToAffineMatrixAndTranslationVector(m_parameters, A, t);
  
  return A;
}


/*============================================================*/ 

VnlDoubleVector3DType ICP3::getTranslation() const
{
  VnlDoubleMatrix3x3Type A;
  VnlDoubleVector3DType t;

  parameterVectorToAffineMatrixAndTranslationVector(m_parameters, A, t);
  
  return t;
}
