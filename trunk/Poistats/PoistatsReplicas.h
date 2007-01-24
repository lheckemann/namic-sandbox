#ifndef __PoistatsReplicas_h
#define __PoistatsReplicas_h

#include <itkArray.h>

#include <vector>

#include "PoistatsReplica.h"
#include "PoistatsModel.h"

/** \class PoistatsReplica
 * 
 */
class PoistatsReplicas
{
public:

  typedef PoistatsReplica::MatrixType MatrixType;
  typedef PoistatsReplica::MatrixPointer MatrixPointer;
  typedef std::vector< MatrixPointer > MatrixListType;
  
  typedef PoistatsReplica::ArrayType ArrayType;
  typedef PoistatsReplica::ArrayPointer ArrayPointer;

  PoistatsReplicas();
  PoistatsReplicas( PoistatsModel *model, const int nReplicas );
  ~PoistatsReplicas();
  
  int GetNumberOfReplicas();
  void SetNumberOfReplicas( const int nReplicas );
  
  double GetMinimumCurrentEnergy();
  
  void FillCurrentMeanEnergies( const double energy );
  double GetCurrentMeanOfEnergies() const;

  void FillPreviousMeanEnergies( const double energy );
  double GetPreviousMeanOfEnergies() const;
  
  double GetNormalizedMeanCurrentPreviousEnergiesDifference() const;
  
  double GetCurrentMeanEnergy( const int replica );
  void SetCurrentMeanEnergy( const int replica, const double energy );

  double GetPreviousMeanEnergy( const int replica );
  
  void GetRandomSortedFirstSecondReplicas( int &first, int &second );
    
  void CopyCurrentToPreviousEnergies();

  void CopyCurrentToPreviousEnergy( int replica );
      
  static void SortArray( itk::Array< double >* unsorted, 
    itk::Array< int >* sortedIndices );
    
  double GetTemperature( const int replica );
  void SetTemperature( const int replica, const double temperature );

  void CoolTemperatures( const double coolingFactor );

  bool ShouldUpdateEnergy( const int replica );
  
  void ResetCurrentToPreviousEnergy( const int replica );

  double CalculateProbablityExchange( const int replica1, const int replica2 );
  
  void ExchangeTemperatures( const int replica1, const int replica2 );
  
  void SetBasePaths( const MatrixPointer basePath );
  
  MatrixPointer GetBasePath( const int replica );

  MatrixPointer GetPreviousTrialPath( const int replica );
  
  void SetInitialPoints( const MatrixPointer points );
  
  void SetNumberOfControlPoints( const int nPoints );
  int GetNumberOfControlPoints();

  void SetPreviousTrialPaths( const MatrixPointer path );
  
  void SetNumberOfSteps( const int nSteps );
  int GetNumberOfSteps();

  void SetCurrentTrialPaths( const MatrixPointer path );
  MatrixPointer GetCurrentTrialPath( const int replica );

  void SetBestTrialPaths( const MatrixPointer path );
  MatrixPointer GetBestTrialPath( const int replica );

  MatrixListType GetBestTrialPaths();

  void GetBestTrialPathsProbabilities( MatrixPointer probabilities );

  void SetBestTrialPathProbabilities( 
    const int replica, ArrayPointer probabilities );
  
  ArrayPointer GetBestTrialPathProbabilities( const int replica);
  
  void SpaceTemperaturesEvenly( 
    const double floorTemp, const double ceilingTemp );

  void GetPerturbedBasePath( const int replica, MatrixPointer perturbedPath, 
    const double sigma, const MatrixPointer possibleStartPoints, 
    const MatrixPointer possibleEndPoints);
    
  void PerturbCurrentTrialPath( const int replica, MatrixPointer lowTrialPath, 
    const int nSteps );    

  void FoundBestPath( const int replica, const MatrixPointer basePath );
  
  void CopyCurrentToPreviousTrialPath( const int replica );

  MatrixPointer RethreadPath( 
    MatrixPointer originalPath, const int nNewSamples );

  MatrixPointer CubicSplineInterpolation( 
    MatrixPointer originalPath,
    ArrayPointer originalPathGrid,
    const int nNewSamples );

private:

  PoistatsModel *m_PoistatsModel;

  int m_NumberOfReplicas;  
  PoistatsReplica *m_Replicas;

  MatrixPointer m_InitialPoints;

  int m_NumberOfControlPoints;
  
  int m_NumberOfSteps;

  // cubic spline stuff
  typedef itk::Vector< double, 3 >    VectorType;
  typedef itk::Image< VectorType, 1 > OutputImageType;  

  typedef itk::PointSet< VectorType, 1 > PointSetType;
    
  typedef itk::BSplineScatteredDataPointSetToImageFilter
     < PointSetType, OutputImageType >  CubicSplineFilterType;
  typedef CubicSplineFilterType::Pointer CubicSplineFilterPointer;

  CubicSplineFilterPointer m_CubicSplineFilter;    


};

#endif
