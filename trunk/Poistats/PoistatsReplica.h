#ifndef __PoistatsReplica_h
#define __PoistatsReplica_h

#include <itkArray2D.h>
#include <itkArray.h>

#include "itkPointSet.h"
#include "itkBSplineScatteredDataPointSetToImageFilter.h"

#include "PoistatsModel.h"

class PoistatsReplica
{
public:

  typedef itk::Array2D< double > MatrixType;
  typedef MatrixType* MatrixPointer;

  typedef itk::Array< double > ArrayType;
  typedef ArrayType* ArrayPointer;
  
  PoistatsReplica();
  PoistatsReplica( PoistatsModel * model );
  ~PoistatsReplica();
  
  void Init();

  void SetModel( PoistatsModel* model );
  
  double GetCurrentMeanEnergy();
  void SetCurrentMeanEnergy( const double energy );
  
  double GetPreviousMeanEnergy();  
  void SetPreviousMeanEnergy( const double temperature );

  double GetTemperature();  
  void SetTemperature( const double temperature );

  void CopyCurrentToPreviousEnergy();
  
  void CoolTemperature( const double coolingFactor );
  
  bool ShouldUpdateEnergy();
  
  void ResetCurrentToPreviousEnergy();
  
  MatrixPointer GetBasePath();
  void SetBasePath( MatrixPointer basePath );

  MatrixPointer GetPreviousTrialPath();
  void SetPreviousTrialPath( MatrixPointer path );
  
    
  static void SpaceEvenly( ArrayPointer outputArray, const double floor, 
    const double ceiling );
    
  static void CalculatePathMagnitude( 
    MatrixPointer path, ArrayPointer magnitude );

  static void CalculateCumulativeSum(
    ArrayPointer inputArray, ArrayPointer cumulativeSum );    
    
  static void CalculatePathVectors( MatrixPointer path, MatrixPointer vectors );

  static void CalculateMagnitude( MatrixPointer path, ArrayPointer magnitude );
  
  MatrixPointer GetCurrentTrialPath();
  
  void SetCurrentTrialPath( MatrixPointer path);

  MatrixPointer GetBestTrialPath();
  
  void SetBestTrialPath( MatrixPointer path);
  
  void GenerateUnitSphereRandom( MatrixPointer randomUnitSphere );  

  void GenerateConstrainedRandomPoint3D(
    vnl_vector< double > currentPoint,
    MatrixPointer possibleNewPoints,
    const double sigma,
    ArrayPointer newRandomPoint );

  void GetPerturbedBasePath( MatrixPointer perturbedPath, 
    const double sigma, const MatrixPointer possibleStartPoints, 
    const MatrixPointer possibleEndPoints);

  static void CopyPath( const MatrixPointer source, MatrixPointer destination );

  void FoundBestPath( const MatrixPointer basePath );
  
  void CopyCurrentToPreviousTrialPath();  
  
  void SetBestTrialPathProbabilities( ArrayPointer probabilities );
  
  ArrayPointer GetBestTrialPathProbabilities();
    
private:

  PoistatsModel *m_PoistatsModel;
  
  double m_CurrentMeanEnergy;
  double m_PreviousMeanEnergy;
  
  double m_Temperature;

  MatrixPointer m_BasePath;
  MatrixPointer m_PreviousTrialPath;
  MatrixPointer m_CurrentTrialPath;
  MatrixPointer m_BestTrialPath;  
  ArrayPointer m_BestTrialPathProbabilities;

  void DeletePathIfNotNull( MatrixPointer path );

  void DeleteArrayIfNotNull( ArrayPointer array );
  
  static bool FindPointsWithinRadius(
    vnl_vector< double >* center,
    MatrixPointer points,
    const double sigma,
    std::vector< int > *indices );

};

#endif
