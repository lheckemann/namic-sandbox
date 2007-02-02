#include "TensorReaderStrategy.h"

TensorReaderStrategy::TensorReaderStrategy(){
  
}


TensorReaderStrategy::~TensorReaderStrategy(){
  
}
  
void 
TensorReaderStrategy::SetObserver( CommandUpdate* observer ){
  this->m_Observer = observer;
}

void
TensorReaderStrategy::SetFileName( std::string fileName ) {
  m_TensorFileName = fileName;
}
