#ifndef shapeBasedGAC_h_
#define shapeBasedGAC_h_

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"

// douher
#include "lib/cArray3D.h"
#include "lib/SFLSSegmentor3D.h"

// std
#include <vector>



class CShapeBasedGAC : public douher::CSFLSSegmentor3D< double >
{
public:
  typedef CShapeBasedGAC Self;
  typedef boost::shared_ptr< Self > Pointer;

  typedef douher::CSFLSSegmentor3D< double > SuperClassType;


  typedef vnl_vector< double > VectorType;
  typedef boost::shared_ptr< VectorType > VectorPointerType;

  typedef vnl_matrix< double > MatrixType;
  typedef boost::shared_ptr< MatrixType > MatrixPointerType;


  /* ctor */
  CShapeBasedGAC() : douher::CSFLSSegmentor3D< double >()
  {
    basicInit();
  }

  void basicInit();


  /* New */
  static Pointer New() { return Pointer(new Self); }



  void setCurvatureWeight(double a) {
    m_curvatureWeight = a;
  }


  void setInflation(double a)  {
    m_inflation = a;
  }



  void setFeatureImage(douher::cArray3D< double >::Pointer featureImg);
  void computeFeatureImage();


  /* Function Section */
  void checkIfReadyToGo();
  void doShapeBasedGACSegmenation();
    

  /* ============================================================
     computeForce    */
  void computeForce();
  double computeForceAt(long ix, long iy, long iz);


  void setMeanShape(douher::cArray3D< double >::Pointer meanShape);
  //  void setEigenShapes(std::vector< douher::cArray3D< double >::Pointer > eigenShapes );
  //  long m_numEigenShapes;


  /* Data Section */
  douher::cArray3D< double >::Pointer m_featureImage;

  double m_curvatureWeight;
  double m_inflation;


  douher::cArray3D< double >::Pointer moveCentroidToThePoint(douher::cArray3D< double >::Pointer phi, \
                                                             vnl_vector<double> c, \
                                                             double fillValue);
//   douher::cArray3D< double >::Pointer isoScaleImage(douher::cArray3D< double >::Pointer img, double r, double fillValue);


  VectorType m_startingCenter;
  void setStartingCenter(VectorType startingCenter);
  void setStartingCenter(double x, double y, double z);

  double m_startingScale;
  void setStartingScale(double startingScale);

  void setLeftAndRightPointsInTheMiddleSlice(VectorType l, VectorType r);
  void setLeftAndRightPointsInTheMiddleSlice(long lx, long ly, long lz, long rx, long ry, long rz);




  /* mean shape, represented by level set function. inside neg, outside pos */
  douher::cArray3D< double >::Pointer m_meanShape;

  /* eigen shapes, represented by level set function, inside neg,
   outside pos. All eigen shapes have already been multiplied by their
   corresponding sqrt(eigen values).*/
  //  std::vector< douher::cArray3D< double >::Pointer > m_eigenShapes;


  /* Given imput img, this function makes it the same size as the
     mp_img. */
  douher::cArray3D< double >::Pointer correctSize(douher::cArray3D< double >::Pointer img, double fillValue);


  /* Isoscale image using the scale factor r. Note if r > 1, the
     "object" in the img will LOOKS LARGER after this operation */
  douher::cArray3D< double >::Pointer isoScaleImage(douher::cArray3D< double >::Pointer img, double r, double fillValue);

  /* The mean shape is a level set fn, this gives the binary fn for
     its inside. */
  void getMaskFromMeanShape();


  double m_volumneOfMeanShape;

  double m_LRDistanceInMiddleSliceOfMeanShape;


  double m_volume;
  double m_volumeOld;
  VectorType m_centroid;

  // Input a level set fn, compute its inside volume
  void computeVolume();
  double computeVolume(douher::cArray3D< double >::Pointer phi);

  void updateVolume();

  void computeCentroid();
  vnl_vector<double> computeCentroid(douher::cArray3D< double >::Pointer phi);

  void updateCentroid();



};




#endif
