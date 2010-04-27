

template <typename TRegistration>
class RegistrationInterfaceCommand : public itk::Command 
{
  static const unsigned int Dimension =3; 
public:
  typedef  RegistrationInterfaceCommand   Self;
  typedef  itk::Command                   Superclass;
  typedef  itk::SmartPointer<Self>        Pointer;
  itkNewMacro( Self );

  //Set-Get methods to change between the scales
  itkSetMacro(MultiScaleSamplePercentageIncrease, double);
  itkSetMacro(MultiScaleMaximumIterationIncrease, double);
  itkSetMacro(MultiScaleStepLengthIncrease, double);


public:
  typedef   TRegistration                              RegistrationType;
  typedef   RegistrationType *                         RegistrationPointer;
  
  typedef   itk::SingleValuedNonLinearOptimizer        OptimizerType;
  typedef   OptimizerType *                            OptimizerPointer;
  typedef   itk::GradientDescentOptimizer              GradientOptimizerType;
  typedef   GradientOptimizerType *                    GradientOptimizerPointer;
  typedef   itk::GradientDescentLineSearchOptimizer    LineSearchOptimizerType;
  typedef   LineSearchOptimizerType  *                 LineSearchOptimizerPointer;
  typedef   itk::SPSAOptimizer                         SPSAOptimizerType;
  typedef   SPSAOptimizerType  *                       SPSAOptimizerPointerType;
  
  typedef   itk::Image< float, Dimension >   ImageType;
  typedef   itk::MultiImageMetric< ImageType>    MetricType;
  typedef   MetricType *                                 MetricPointer;

  void Execute(itk::Object * object, const itk::EventObject & event)
  {
    if( !(itk::IterationEvent().CheckEvent( &event )) )
    {
      return;
    }
    RegistrationPointer registration =  dynamic_cast<RegistrationPointer>( object );
    OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( 
                       registration->GetOptimizer() );
    MetricPointer       metric = dynamic_cast< MetricPointer>
                                         (registration->GetMetric());

    // Output message about registration
    std::cout << "message: metric type: " << metric->GetNameOfClass() << std::endl;
    std::cout << "message: Transform Type: " << registration->GetTransformArray(0)->GetNameOfClass();
    if( !strcmp(registration->GetTransformArray(0)->GetNameOfClass(), "BSplineDeformableTransform") )
    {
      std::cout << " (" << bsplineGridSize << "x" << bsplineGridSize << "x" << bsplineGridSize << ")";
    }    
    std::cout << std::endl;
    std::cout << "message: Image Resolution: " <<
        registration->GetImagePyramidArray(0)->
        GetOutput(registration->GetCurrentLevel())->GetLargestPossibleRegion().GetSize() << std::endl;
    //registration->GetImagePyramidArray(0)->GetOutput(registration->GetCurrentLevel())->Print(std::cout,6);
    std::cout << "message: Number of Images: " << metric->GetNumberOfImages() << std::endl;
    std::cout << "message: Number of total parameters: " << registration->GetTransformParametersLength() << std::endl;
    std::cout << "message: Optimizertype: " << optimizer->GetNameOfClass() << std::endl;

    if ( registration->GetCurrentLevel() == 0 )
    {
      // Set the number of spatial samples according to the current level
      metric->SetNumberOfSpatialSamples(
                        (unsigned int) (metric->GetNumberOfSpatialSamples() /
                        vcl_pow( vcl_pow(2.0F, Dimension )/m_MultiScaleSamplePercentageIncrease,
                             (double) (registration->GetNumberOfLevels() - 1.0) ) ) );

      
      
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer = dynamic_cast< GradientOptimizerPointer >(
            registration->GetOptimizer() );
        gradientPointer->SetNumberOfIterations(
             (int)(gradientPointer->GetNumberOfIterations()*vcl_pow(m_MultiScaleMaximumIterationIncrease,
                   (double) (registration->GetNumberOfLevels() - 1.0) ) ));
        gradientPointer->SetLearningRate(
            gradientPointer->GetLearningRate()*vcl_pow(m_MultiScaleStepLengthIncrease,
                (double) (registration->GetNumberOfLevels() - 1.0) )  );
        //print messages
        std::cout << "message: Optimizer # of Iter. to go : " << gradientPointer->GetNumberOfIterations() << std::endl;
        std::cout << "message: Learning rate : " << gradientPointer->GetLearningRate() << std::endl;

      }

      else if(!strcmp(optimizer->GetNameOfClass(), "GradientDescentLineSearchOptimizer") )
      {
        LineSearchOptimizerPointer lineSearchOptimizerPointer =
            dynamic_cast< LineSearchOptimizerPointer >(
            registration->GetOptimizer() );
        lineSearchOptimizerPointer->SetMaximumIteration(
            (int)(lineSearchOptimizerPointer->GetMaximumIteration()*vcl_pow(m_MultiScaleMaximumIterationIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) ));
        lineSearchOptimizerPointer->SetStepLength(lineSearchOptimizerPointer->GetStepLength()*
            vcl_pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) );
        //print messages
        std::cout << "message: Optimizer # of Iter. to go : " <<
            lineSearchOptimizerPointer->GetMaximumIteration() <<std::endl;
        std::cout << "message: Optimizer learning rate : " <<
            lineSearchOptimizerPointer->GetStepLength() << std::endl;
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "SPSAOptimizer") )
      {
        SPSAOptimizerPointerType SPSAOptimizerPointer =
            dynamic_cast< SPSAOptimizerPointerType >(
            registration->GetOptimizer() );
        SPSAOptimizerPointer->SetMaximumNumberOfIterations(
            (int)(SPSAOptimizerPointer->GetMaximumNumberOfIterations()*vcl_pow(m_MultiScaleMaximumIterationIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) ));
        SPSAOptimizerPointer->Seta(SPSAOptimizerPointer->Geta()*
            vcl_pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ) );
        SPSAOptimizerPointer->SetA( SPSAOptimizerPointer->GetMaximumNumberOfIterations()/10.0 );
        SPSAOptimizerPointer->Setc( SPSAOptimizerPointer->Getc()*
                                    vcl_pow(m_MultiScaleStepLengthIncrease,(double) (registration->GetNumberOfLevels() - 1.0) ));
        //print messages
        std::cout << "message: Optimizer # of Iter. to go : " <<
            SPSAOptimizerPointer->GetMaximumNumberOfIterations() <<std::endl;
        std::cout << "message: Optimizer a : " <<
            SPSAOptimizerPointer->Geta() << std::endl;
        std::cout << "message: Optimizer c : " <<
            SPSAOptimizerPointer->Getc() << std::endl;
      }

      
    }
    else
    {
      // Set the number of spatial samples according to the current level
      metric->SetNumberOfSpatialSamples(
            (unsigned int) (metric->GetNumberOfSpatialSamples() *
                  vcl_pow(2.0, Dimension )/m_MultiScaleSamplePercentageIncrease ) );

      // Decrease the learning rate at each increasing multiresolution level
      // Increase the number of steps
      if(  !strcmp(optimizer->GetNameOfClass(), "GradientDescentOptimizer" ) )
      {
        GradientOptimizerPointer gradientPointer =
            dynamic_cast< GradientOptimizerPointer >(
                                                                 registration->GetOptimizer() );
        gradientPointer->SetNumberOfIterations(
            (int) (gradientPointer->GetNumberOfIterations()/m_MultiScaleMaximumIterationIncrease ));
        gradientPointer->SetLearningRate(
            gradientPointer->GetLearningRate()/m_MultiScaleStepLengthIncrease  );
        //print messages
        std::cout << "message: Optimizer # of Iter. to go : " << gradientPointer->GetNumberOfIterations() << std::endl;
        std::cout << "message: Learning rate : " << gradientPointer->GetLearningRate() << std::endl;
      }

      else if(!strcmp(optimizer->GetNameOfClass(), "GradientDescentLineSearchOptimizer") )
      {
        LineSearchOptimizerPointer lineSearchOptimizerPointer =
            dynamic_cast< LineSearchOptimizerPointer >(registration->GetOptimizer() );
        lineSearchOptimizerPointer->SetMaximumIteration(
            (int)(lineSearchOptimizerPointer->GetMaximumIteration()/m_MultiScaleMaximumIterationIncrease ));
        lineSearchOptimizerPointer->SetStepLength(
            lineSearchOptimizerPointer->GetStepLength()/m_MultiScaleStepLengthIncrease );
        
        std::cout << "message: Optimizer # of Iter. to go : " <<
            lineSearchOptimizerPointer->GetMaximumIteration() << std::endl;
        std::cout << "message: Optimizer learning rate : " <<
            lineSearchOptimizerPointer->GetStepLength() << std::endl;
      }

      //SPSA
      else if(!strcmp(optimizer->GetNameOfClass(), "SPSAOptimizer") )
      {
        SPSAOptimizerPointerType SPSAOptimizerPointer =
            dynamic_cast< SPSAOptimizerPointerType >(registration->GetOptimizer() );
        SPSAOptimizerPointer->SetMaximumNumberOfIterations(
            (int)(SPSAOptimizerPointer->GetMaximumNumberOfIterations()/m_MultiScaleMaximumIterationIncrease ));
        SPSAOptimizerPointer->Seta(
            SPSAOptimizerPointer->Geta()/m_MultiScaleStepLengthIncrease );
        SPSAOptimizerPointer->SetA( SPSAOptimizerPointer->GetMaximumNumberOfIterations()/10.0 );
        SPSAOptimizerPointer->Setc( SPSAOptimizerPointer->Getc()/m_MultiScaleStepLengthIncrease );

        
        std::cout << "message: Optimizer # of Iter. to go : " <<
            SPSAOptimizerPointer->GetMaximumNumberOfIterations() << std::endl;
        std::cout << "message: Optimizer a : " <<
            SPSAOptimizerPointer->Geta() << std::endl;
        std::cout << "message: Optimizer c : " <<
            SPSAOptimizerPointer->Getc() << std::endl;
      }

    }

    std::cout << "message: Sample Percentage : " << 
        metric->GetNumberOfSpatialSamples() / (double)metric->GetFixedImageRegion().GetNumberOfPixels() << std::endl;




  }
  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }

  void SetFileNames(    std::vector<std::string> fileNames2,
                        std::vector<std::string> itkNotUsed(inputFileNames2),
                        std::vector<std::string> outputFileNames2,
                        std::string              outputFolder2)
  {
    for(unsigned int i=0; i<fileNames.size(); i++ )
    {
      fileNames[i] = fileNames2[i];
      inputFileNames[i] = inputFileNames[i];
      outputFileNames[i] =  outputFileNames2[i];
    }
    outputFolder =  outputFolder2;
    transformArray.resize(fileNames.size());

  }
  int bsplineGridSize;
  
  protected:
    //Constructor initialize the variables
    RegistrationInterfaceCommand()
    {
      m_MultiScaleSamplePercentageIncrease = 1;
      m_MultiScaleMaximumIterationIncrease = 1;
      m_MultiScaleStepLengthIncrease = 1;
    };
  private:
    double m_MultiScaleSamplePercentageIncrease;
    double m_MultiScaleMaximumIterationIncrease;
    double m_MultiScaleStepLengthIncrease;

    std::vector<std::string> fileNames;
    std::vector<std::string> inputFileNames;
    std::vector<std::string> outputFileNames;
    std::string              outputFolder;
    std::vector< itk::Transform< double, Dimension, Dimension >* > transformArray;
};
