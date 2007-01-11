#include "itkVectorContainer.h"
#include "itkVector.h"
#include "vnl/vnl_vector_fixed.h"
#include <iostream>

int main( int argc, char *argv[] ){
  //typedef vnl_vector_fixed< double, 3 > TGradientDirection;
  typedef double TGradientDirection;
  itk::VectorContainer< unsigned int, TGradientDirection >::Pointer gradientDirectionContainer = NULL;
  
  gradientDirectionContainer = itk::VectorContainer< unsigned int, TGradientDirection >::New();
  std::cout << "Size: " << int(gradientDirectionContainer->Size()) << std::endl;
  gradientDirectionContainer->CreateIndex(0);
  
  return 1;
}  
    
