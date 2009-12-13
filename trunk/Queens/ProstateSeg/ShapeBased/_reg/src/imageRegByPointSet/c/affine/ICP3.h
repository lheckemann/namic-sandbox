#ifndef ICP3_h_
#define ICP3_h_

#include "../pointSet.h"

#include "../kdtree2.hpp"


class ICP3
{
public:
  ICP3();
  ICP3( PointSet3DType* fixedPointSet, PointSet3DType* movingPointSet );

  enum CostFunctionType { L1, L2, Exp };


  // functions:
  const int getNumberOfParamters() const {return m_numOfParameters;}


  void setFixedPointSet(PointSet3DType* fixedPointSet) {m_fixedPointSet = fixedPointSet;}
  void setMovingPointSet(PointSet3DType* movingPointSet) {m_movingPointSet = movingPointSet;}

  void setParameters(const VnlDoubleVectorType& parameters) {m_parameters = parameters;}
  VnlDoubleVectorType getParameters() const {return m_parameters;}

  void setScalings(const VnlDoubleVectorType& scalings) {m_scalings = scalings;}
  VnlDoubleVectorType getScalings() const {return m_scalings;}

  void setLambda(double l) {m_lambdaDeterminent = l;}
  double getLambda() const {return m_lambdaDeterminent;}

  void computeScalings();

  void computeInitialParameters();

  void setMaxIteration(unsigned long maxIteration) {m_maxIteration = maxIteration;}
  void setMaxFunctionEvaluationInOptimization(int maxFnEval) {m_maxFnEval = maxFnEval;}

  void setCostFnType( CostFunctionType costFnType) {m_costFnType = costFnType;}
  CostFunctionType setCostFnType() {return m_costFnType;}
  
  double getCost() const;
  double getCost( const VnlDoubleVectorType& param) const;
  void getCostAndGradient(const VnlDoubleVectorType& currentParam, double* cost, VnlDoubleVectorType* gradient) const;

  //   double getCostL1() const;
  //   void getCostAndGradientL1(const VnlDoubleVectorType& currentParam, double* cost, VnlDoubleVectorType* gradient) const;

  double getCostL2() const;
  double getCostL2( const VnlDoubleVectorType& param) const;
  void getCostAndGradientL2(const VnlDoubleVectorType& currentParam, double* cost, VnlDoubleVectorType* gradient) const;

  //   double getCostExpL2() const;
  //   void getCostAndGradientExpL2(const VnlDoubleVectorType& currentParam, double* cost, VnlDoubleVectorType* gradient) const;


  void adjustGradient(VnlDoubleVectorType* originalGradient) const;


  void parameterVectorToAffineMatrixAndTranslationVector(const VnlDoubleVectorType &currentParam, \
                                                         VnlDoubleMatrix3x3Type& affine, VnlDoubleVector3DType& translation) const;

  void affineMatrixAndTranslationVectorToParameterVector(const VnlDoubleMatrix3x3Type& affine, \
                                                         const VnlDoubleVector3DType& translation, \
                                                         VnlDoubleVectorType& currentParam) const;

  VnlDoubleMatrix3x3Type getAffineMatrix() const;
  VnlDoubleVector3DType getTranslation() const;

  void gogogo();

  void optimizeParameters();
  void optimizeTranslationParameters();
  void optimizeAffineParameters();

  PointSet3DType* getNewMovingPointSet() const;
  PointSet3DType* getNewMovingPointSet(VnlDoubleVectorType param) const;

  void findClosestPoint(VnlDoubleVector3DType pt, unsigned long& idxOfClosestPtInPtSet, double& distanceSqToTheClosestPt) const;

  void buildKDTreeForClosestPtSearch();


  // variables:
  static const int m_numOfParameters = 12; // Affine matrix, translateX, translateY, translateZ

protected:
  // protected variables:
  PointSet3DType* m_fixedPointSet;
  kdtree2* m_kdTreeForFixedPointSet;

  PointSet3DType* m_movingPointSet;

  bool m_ICPDone;

  //  double m_expFactor;

  double m_lambdaDeterminent;

  CostFunctionType m_costFnType;

  VnlDoubleVectorType m_parameters;

  VnlDoubleVectorType m_scalings;

  unsigned long m_maxFnEval;
  unsigned long m_maxIteration;
};



#endif

