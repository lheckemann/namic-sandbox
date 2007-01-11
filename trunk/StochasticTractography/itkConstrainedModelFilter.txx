#include "itkConstrainedModelFilter.h"

namespace itk{

template< class TTensorParamImage,
        class TConstrainedParamImage >
ConstrainedModelFilter< TTensorParamImage,
                TConstrainedParamImage >
::ConstrainedModelFilter(){
  /** Instantiate the filters **/
  m_tensormodelfilter = TensorModelFilterType::New();
  m_tensormodeltoconstrainedmodelfilter = TensorModelToConstrainedModelFilterType::New();
  
  /**Setup the pipeline**/
  m_tensormodeltoconstrainedmodelfilter->SetInput(
    m_tensormodelfilter->GetOutput());
    
  this->Setgradients(NULL);
  this->Setbvalues(NULL);
}


template< class TTensorParamImage,
        class TConstrainedParamImage >
void
ConstrainedModelFilter< TTensorParamImage,
                TConstrainedParamImage >
::GenerateData( void ){
  m_tensormodelfilter->SetInput(this->GetInput());
  
  m_tensormodelfilter->Setgradients(this->Getgradients());
  m_tensormodelfilter->Setbvalues(this->Getbvalues());
  
  m_tensormodeltoconstrainedmodelfilter->GraftOutput( this->GetOutput() );
  
  m_tensormodeltoconstrainedmodelfilter->Update();
  
  this->GraftOutput(m_tensormodeltoconstrainedmodelfilter->GetOutput() );
}

}
