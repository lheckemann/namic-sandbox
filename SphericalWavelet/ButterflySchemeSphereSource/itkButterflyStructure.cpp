#include "itkButterflyStructure.h"
using namespace std;


namespace itk {
 
 ButterflyStructure::ButterflyStructure()
 {};

 void ButterflyStructure::
  generateButterflyPoints(unsigned long X, unsigned long Y, unsigned long Z, std::vector< vector<unsigned long> > & PointL, std::vector< vector<unsigned long> > & CellL)
 {
   
  A1=X;
        A2=Y;
  B1=Z;
  std::vector <unsigned long> _result1,_result2;
  std::vector <unsigned long>::iterator Result1, Result2;
  //B2      
  sort(PointL[X].begin( ), PointL[X].end( ) );
  sort(PointL[Y].begin( ), PointL[Y].end( ) );
        Result1 = set_intersection ( PointL[X].begin ( ) , PointL[X].end ( ) ,PointL[Y].begin ( ) , PointL[Y].end ( ) , _result1.begin ( ) );
  _result1.erase(Result1, _result1.end());// erase the elements that are not in common in _result1
  sort(CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) );
  sort(CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) );
  //intersection of the 2 vectors of points (1vect= 1triangle)
        Result2 = set_intersection ( CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) ,CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) , _result2.begin ( ) );
  _result2.erase(0,Result2-1); //erase the common elements, now we have only B1 and B2 potentially
  if (_result2[0]==Z){ // if B1 is the first ...
   B2=_result2[1];}
  else {B2=_result2[0];}//if B1 est le second !

  //C1
        sort(PointL[X].begin( ), PointL[X].end( ) );
  sort(PointL[Z].begin( ), PointL[Z].end( ) );
  Result1 = set_intersection ( PointL[X].begin ( ) , PointL[X].end ( ) ,PointL[Z].begin ( ) , PointL[Z].end ( ) , _result1.begin ( ) );
  _result1.erase(Result1, _result1.end());// erase the triangles that are not in common in _result1
  sort(CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) );
  sort(CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) );
  Result2 = set_intersection ( CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) ,CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) , _result2.begin ( ) );
        _result2.erase(0,Result2-1); //erase the common elements, now we have only C1 and Y potentially
        if (_result2[0]==Y){ // if Y is the first ...
   C1=_result2[1];}
  else {C1=_result2[0];}//if C1 est le second !

  //C2
  sort(PointL[Y].begin( ), PointL[Y].end( ) );
  sort(PointL[Z].begin( ), PointL[Z].end( ) );
  Result1 = set_intersection ( PointL[Y].begin ( ) , PointL[Y].end ( ) ,PointL[Z].begin ( ) , PointL[Z].end ( ) , _result1.begin ( ) );
  _result1.erase(Result1, _result1.end());// erase the triangles that are not in common in _result1
  sort(CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) );
  sort(CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) );
  Result2 = set_intersection ( CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) ,CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) , _result2.begin ( ) );
        _result2.erase(0,Result2-1); //erase the common elements, now we have only C2 and X potentially
        if (_result2[0]==X){ // if X is the first ...
   C2=_result2[1];}
  else {C2=_result2[0];}//if C2 est le second !


  //C3
  sort(PointL[X].begin( ), PointL[X].end( ) );
  sort(PointL[B2].begin( ), PointL[B2].end( ) );
  Result1 = set_intersection ( PointL[X].begin ( ) , PointL[X].end ( ) ,PointL[B2].begin ( ) , PointL[B2].end ( ) , _result1.begin ( ) );
  _result1.erase(Result1, _result1.end());// erase the triangles that are not in common in _result1
  sort(CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) );
  sort(CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) );
  Result2 = set_intersection ( CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) ,CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) , _result2.begin ( ) );
        _result2.erase(0,Result2-1); //erase the common elements, now we have only C3 and Y potentially
        if (_result2[0]==Y){ // if Y is the first ...
   C3=_result2[1];}
  else {C3=_result2[0];}//if C3 est le second !

  //C4
  sort(PointL[Y].begin( ), PointL[Y].end( ) );
  sort(PointL[B2].begin( ), PointL[B2].end( ) );
  Result1 = set_intersection ( PointL[Y].begin ( ) , PointL[Y].end ( ) ,PointL[B2].begin ( ) , PointL[B2].end ( ) , _result1.begin ( ) );
  _result1.erase(Result1, _result1.end());// erase the triangles that are not in common in _result1
  sort(CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) );
  sort(CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) );
  Result2 = set_intersection ( CellL[_result1[0]].begin( ), CellL[_result1[0]].end( ) ,CellL[_result1[1]].begin( ), CellL[_result1[1]].end( ) , _result2.begin ( ) );
        _result2.erase(0,Result2-1); //erase the common elements, now we have only C4 and X potentially
        if (_result2[0]==X){ // if X is the first ...
   C4=_result2[1];}
  else {C4=_result2[0];}//if C4 est le second !
 
 }
}//end of itk namespace
