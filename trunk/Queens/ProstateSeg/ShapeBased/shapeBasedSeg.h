#ifndef shapeBasedSeg_h_
#define shapeBasedSeg_h_

#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkImageRegionIteratorWithIndex.h"

#include <vector>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

class CShapeBasedSeg
{
public:
  // ITK typedef's
  typedef itk::Image< double, 3 > DoubleImageType;
  typedef itk::Image< unsigned char, 3 > UcharImageType;


  // vnl typedef's
  typedef vnl_vector< double > VnlDoubleVectorType;
  typedef vnl_matrix< double > VnlDoubleMatrixType;

  typedef vnl_vector_fixed< double, 3> VnlDoubleVector3DType;
  typedef vnl_matrix_fixed< double, 3, 3> VnlDoubleMatrix3x3Type;


  typedef std::vector< DoubleImageType::Pointer > TVectorOfDoubleImagePointerType;

  CShapeBasedSeg();
  //  CShapeBasedSeg( TFixedImagePointerType fixedImg, TMovingImagePointerType movingImg );


  // functions:
  int getNumberOfParamters() const {return m_numOfParameters;}
  void setNumberOfParamters(int np) {m_numOfParameters = np;}


  void setInputImage(DoubleImageType::Pointer image);

  DoubleImageType::Pointer getSegmentation() const;

  void setMeanShape(DoubleImageType::Pointer meanShape);
  void addEigenShape(DoubleImageType::Pointer eigenShape);

  void setParameters(const VnlDoubleVectorType& parameters) {m_parameters = parameters;}
  VnlDoubleVectorType getParameters() const {return m_parameters;}

//   void setScalings(const VnlDoubleVectorType& scalings) {m_scalings = scalings;}
//   VnlDoubleVectorType getScalings() const {return m_scalings;}

  void basicInitialization();

  void generateInitialParameters();
  void generateInitialScalings();

  void setMaxIteration(unsigned long maxIteration) {m_maxIteration = maxIteration;}
  void setMaxFunctionEvaluationInOptimization(unsigned long maxFnEval) {m_maxFnEval = maxFnEval;}

  /*============================================================
   * Yezzi eng */
  double getCost() const;
  double getCost( const VnlDoubleVectorType& param) const;

  void getCostAndGradient(const VnlDoubleVectorType& currentParam, double* cost, VnlDoubleVectorType* gradient) const;
  /* Yezzi eng 
   *============================================================ */


  void adjustGradient(VnlDoubleVectorType* originalGradient) const;


  void parameterVectorToEigenShapeParameters(const VnlDoubleVectorType& currentParam, \
                                             VnlDoubleVectorType& eigenParam) const;

  void eigenShapeParametersToParameterVector(const VnlDoubleVectorType& eigenParam, \
                                             VnlDoubleVectorType& currentParam) const;

  void parameterVectorToAffineMatrixAndTranslationVector(const VnlDoubleVectorType& currentParam, \
                                                         VnlDoubleMatrix3x3Type& affine, VnlDoubleVector3DType& translation) const;

  void affineMatrixAndTranslationVectorToParameterVector(const VnlDoubleMatrix3x3Type& affine, \
                                                         const VnlDoubleVector3DType& translation, \
                                                         VnlDoubleVectorType& currentParam) const;

  void areYouReady();
//   VnlDoubleMatrix3x3Type getAffineMatrix() const;
//   VnlDoubleVector3DType getTranslation() const;

  void gogogo();

  void optimizeParameters();
  void optimizeTranslationParameters();
  void optimizeAffineParameters();
  void optimizeEigenParameters();

//   PointSet3DType* getNewMovingPointSet() const;
//   PointSet3DType* getNewMovingPointSet(VnlDoubleVectorType param) const;


protected:
//   /*============================================================
//    * friend */
//   friend class CShapeBasedSegCost;


  /*============================================================
   * functions */
  DoubleImageType::Pointer generateShapeFromParameter() const;
  DoubleImageType::Pointer generateShapeFromParameter(const VnlDoubleVectorType& param) const;


  DoubleImageType::Pointer transformImageUsingParameter(DoubleImageType::Pointer img) const;  
  DoubleImageType::Pointer transformImageUsingParameter(DoubleImageType::Pointer img, const VnlDoubleVectorType& param) const;  

//   inline double bandHeaviside(double t, double epsilon = 0.01) const;
//   inline double bandDelta(double t, double epsilon = 0.01) const;

  inline double heaviside(double t, double epsilon = 0.01) const;
  inline double delta(double t, double epsilon = 0.01) const;



  /*============================================================
   * variables */
  /*
   * 0~8: Affine matrix
   * 9~11: translateX, translateY, translateZ
   * 12~: eigen parameters
   */ 
  int m_numOfParameters; 

  long m_shapeSize[3];
  double m_spacing[3];

  DoubleImageType::Pointer m_inputImage;
  bool m_inputImageSet;


  DoubleImageType::Pointer m_meanShape;
  bool m_meanShapeSet;

  TVectorOfDoubleImagePointerType m_listOfEigenShapes;

  DoubleImageType::Pointer m_currentShape;

  bool m_segDone;

  double m_epsilon;
  double m_sigma;

  double m_affineMatrixCoef;


  VnlDoubleVectorType m_parameters;
  VnlDoubleVectorType m_scalings;

  unsigned long m_maxFnEval;
  unsigned long m_maxIteration;
};



//#include "shapeBasedSegCost.h"


#endif


