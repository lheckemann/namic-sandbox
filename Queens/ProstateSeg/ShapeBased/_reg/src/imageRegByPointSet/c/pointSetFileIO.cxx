#include "pointSetFileIO.h"

#include <fstream>
#include <iostream>
#include <csignal>



/*============================================================*/
PointSet2DType* readPointSetFromTextFile2D(const char* fileName)
{
  PointSet2DType* pointSet = new PointSet2DType;

  std::ifstream theFile( fileName );
  if( theFile.fail() )
    {
      std::cerr << "Error opening points file with name : " << std::endl;
      std::cerr << fileName << std::endl;
      raise(SIGABRT);
    }


  VnlDoubleVector2DType onePoint;
  theFile >> onePoint;
  while( !theFile.eof() )
    {
      //      std::cout<<onePoint<<std::endl;
      pointSet->push_back(onePoint);
      theFile >> onePoint;
    }

  theFile.close();

  //  std::cout << "Number of Points = " << pointSet->GetNumberOfPoints() << std::endl;

  return pointSet;
}


/*============================================================*/
void savePointSetToTextFile2D(PointSet2DType* ptSet, const char* fileName)
{
  std::ofstream theFile( fileName );
  if( theFile.fail() )
    {
      std::cerr << "Error opening points file with name : " << std::endl;
      std::cerr << fileName << std::endl;
      raise(SIGABRT);
    }

  unsigned long numPt = ptSet->size();

  for (unsigned long pointId = 0; pointId < numPt; ++pointId)
    {
      VnlDoubleVector2DType onePoint = ptSet->at(pointId);
      unsigned int dim = onePoint.size(); // should be 2;

      for (unsigned int id = 0; id < dim; ++id)
        {
          theFile << onePoint[id] <<"  ";
        }
      theFile<<std::endl;          

    }

  theFile.close();
}


/*============================================================*/
PointSet3DType* readPointSetFromTextFile3D(const char* fileName)
{
  PointSet3DType* pointSet = new PointSet3DType;

  std::ifstream theFile( fileName );
  if( theFile.fail() )
    {
      std::cerr << "Error opening points file with name : " << std::endl;
      std::cerr << fileName << std::endl;
      raise(SIGABRT);
    }


  VnlDoubleVector3DType onePoint;
  theFile >> onePoint;
  while( !theFile.eof() )
    {
      //      std::cout<<onePoint<<std::endl;
      pointSet->push_back(onePoint);
      theFile >> onePoint;
    }

  theFile.close();

  //  std::cout << "Number of Points = " << pointSet->GetNumberOfPoints() << std::endl;

  return pointSet;
}

/*============================================================*/
void savePointSetToTextFile3D(PointSet3DType* ptSet, const char* fileName)
{
  std::ofstream theFile( fileName );
  if( theFile.fail() )
    {
      std::cerr << "Error opening points file with name : " << std::endl;
      std::cerr << fileName << std::endl;
      raise(SIGABRT);
    }

  unsigned long numPt = ptSet->size();

  for (unsigned long pointId = 0; pointId < numPt; ++pointId)
    {
      VnlDoubleVector3DType onePoint = ptSet->at(pointId);
      unsigned int dim = onePoint.size(); // should be 3;

      for (unsigned int id = 0; id < dim; ++id)
        {
          theFile << onePoint[id] <<"  ";
        }
      theFile<<std::endl;          

    }

  theFile.close();
}



/*============================================================
  shared_ptr version */
PointSet2DPointerType readPointSetFromTextFile2D1(const char* fileName)
{
  PointSet2DPointerType pointSet( new PointSet2DType );

  std::ifstream theFile( fileName );
  if( theFile.fail() )
    {
      std::cerr << "Error opening points file with name : " << std::endl;
      std::cerr << fileName << std::endl;
      raise(SIGABRT);
    }


  VnlDoubleVector2DType onePoint;
  theFile >> onePoint;
  while( !theFile.eof() )
    {
      //      std::cout<<onePoint<<std::endl;
      pointSet->push_back(onePoint);
      theFile >> onePoint;
    }

  theFile.close();

  //  std::cout << "Number of Points = " << pointSet->GetNumberOfPoints() << std::endl;

  return pointSet;
}


/*============================================================
  shared_ptr version */
void savePointSetToTextFile2D(PointSet2DPointerType ptSet, const char* fileName)
{
  savePointSetToTextFile2D(ptSet.get(), fileName);
}


/*============================================================
  shared_ptr version */
PointSet3DPointerType readPointSetFromTextFile3D1(const char* fileName)
{
  PointSet3DPointerType pointSet( new PointSet3DType );

  std::ifstream theFile( fileName );
  if( theFile.fail() )
    {
      std::cerr << "Error opening points file with name : " << std::endl;
      std::cerr << fileName << std::endl;
      raise(SIGABRT);
    }


  VnlDoubleVector3DType onePoint;
  theFile >> onePoint;
  while( !theFile.eof() )
    {
      //      std::cout<<onePoint<<std::endl;
      pointSet->push_back(onePoint);
      theFile >> onePoint;
    }

  theFile.close();

  //  std::cout << "Number of Points = " << pointSet->GetNumberOfPoints() << std::endl;

  return pointSet;  
}


/*============================================================
  shared_ptr version */
void savePointSetToTextFile3D(PointSet3DPointerType ptSet, const char* fileName)
{
  savePointSetToTextFile3D(ptSet.get(), fileName);
}
