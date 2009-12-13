#ifndef imgRegByPtSet3D_h_
#define imgRegByPtSet3D_h_

#include <vnl/vnl_matrix_fixed.h>

#include "../pointSet.h"

class CImgRegByPtSet3D
{
public:
  CImgRegByPtSet3D();
  CImgRegByPtSet3D(DoubleImage3DPointerType fixedImg, DoubleImage3DPointerType movingImg);

  void computeInitialParameters();

  void imageToPointSet3D( DoubleImage3DPointerType image, unsigned long numOfPt, PointSet3DType& ptSet);

  DoubleImage3DPointerType getFeatureMap(DoubleImage3DPointerType image, double alpha = 1.0, double beta = 40);

  void setMaxIteration(unsigned long maxIteration) {m_maxIteration = maxIteration;}
  void setMaxFunctionEvaluationInOptimization(unsigned long numEval) {m_maxNumberOfFunctionEvaluation = numEval;}

  void setParameters(const VnlDoubleVectorType& parameters) {m_parameters = parameters;}
  VnlDoubleVectorType getParameters() const {return m_parameters;}

  void setNumberOfPoint(unsigned long numPt)
  {
    m_numerOfPointForFixedImage = numPt;
    m_numerOfPointForMovingImage = numPt;
  }

  void setLambda(double la) {m_lambda = la;}

  DoubleImage3DPointerType getRegisteredMovingImage();

  void gogogo();

  void pointSetRegistration(VnlDoubleVectorType initParam);

  void savePointSetToTextFile3D(const PointSet3DType& ptSet, const char* fileName);


  // vars
  bool m_computationDone;
  
  unsigned long m_numerOfPointForFixedImage;
  unsigned long m_numerOfPointForMovingImage;

  unsigned long m_maxIteration;
  unsigned long m_maxNumberOfFunctionEvaluation;

  DoubleImage3DPointerType m_fixedImage;
  DoubleImage3DPointerType m_movingImage;

  PointSet3DType m_fixedPointSet;
  PointSet3DType m_movingPointSet;

  VnlDoubleMatrix3x3Type m_finalAffineMatrix;
  VnlDoubleVector3DType m_finalTranslationVector;

  VnlDoubleVectorType m_parameters;

  double m_lambda;

  double m_minimumCost;
};

#endif
