
//8 Point structure I am associating with the "pixelType" of my ButterflySchemeSphereSource result mesh

#ifndef __itkButterflyStructure_h
#define __itkButterflyStructure_h

#include <algorithm>
using namespace std;

namespace itk{

class ButterflyStructure : public Object
{

public:

  typedef ButterflyStructure   Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(ButterflyStructure, Object);

 //typedef ButterflyStructure       Self;
     //itkNewMacro(Self);

itkSetMacro ( Left,  unsigned long );
itkGetMacro ( Left,  unsigned long );

itkSetMacro ( Right,  unsigned long );
itkGetMacro ( Right,  unsigned long );

itkSetMacro ( Top,  unsigned long );
itkGetMacro ( Top,  unsigned long );

itkSetMacro ( Bottom,  unsigned long );
itkGetMacro ( Bottom,  unsigned long );

itkSetMacro ( Star1,  unsigned long );
itkGetMacro ( Star1,  unsigned long );

itkSetMacro ( Star2,  unsigned long );
itkGetMacro ( Star2,  unsigned long );

itkSetMacro ( Star3,  unsigned long );
itkGetMacro ( Star3,  unsigned long );

itkSetMacro ( Star4,  unsigned long );
itkGetMacro ( Star4,  unsigned long );




void generateButterflyPoints(unsigned long X, unsigned long Y, unsigned long Z, std::vector< vector<unsigned long> >& PointL, std::vector< vector<unsigned long> >& CellL);


 unsigned long  m_Left;
 unsigned long  m_Right;
 unsigned long  m_Top;
 unsigned long  m_Bottom;
 unsigned long  m_Star1;
 unsigned long  m_Star2;
 unsigned long  m_Star3;
 unsigned long  m_Star4;
 
protected:
  ButterflyStructure();
  ~ButterflyStructure() {}
};
}
#endif
