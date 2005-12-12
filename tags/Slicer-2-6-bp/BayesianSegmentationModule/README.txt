What's here ? 
What's supposed to be here ?
What files matter ?


1. itkBayesianClassifierInitializationImageFilter.h
   itkBayesianClassifierInitializationImageFilter.txx
    
   Takes an input image ( to be classified ) and generates membership images
   that can be passed on as input the the Bayesian classifier :
   itkBayesianClassificationImageFilter.h
  
   Already in ITK trunk

2. itkBayesianClassifierImageFilter.h
   itkBayesianClassifierImageFilter.txx

   Takes the membership image from the itkBayesianClassifierInitializationImageFilter
   and classifies it to generate a label map.

   Already in ITK trunk

3. BayesianClassifierInitializer.cxx
   BayesianClassifier.cxx
   
   Example for 1 and 2.
   
   Already in ITK trunk
   
4. itkBayesianClassificationImageFilter.h
   itkBayesianClassificationImageFilter.txx

   This filter is a wrapper around the itk::BayesianClassifierInitializationImageFilter
   and the itk::BayesianClassifierImageFilter. It provides the minimal interfaces
   necessary to provide Slicer with a blackbox that takes an image (to be
   classified) as input and produces a label map. All public interfaces to this
   filter must be ones that can be accessed via a GUI in slicer

   This will ultimately move into slicer2/Modules/vtkITK/cxx

5. itkBayesianClassificationImageFilterTest.cxx

   Test for itkBayesianClassificationImageFilter

   This may not move anywhere.. Or if the slicer guys care about testing it, they
   will test it.

6. vtkITKImageToImageFilterFUC.h
   
   Wrap the itk::ImageToImageFilter for an input of Image< float, 3 >
   and an output (label map) of Image< unsigned char, 3 >
   This is probably the most often anticipated case for classfication.

   This will ultimately move into slicer2/Modules/vtkITK/cxx

7. vtkITKBayesianClassifierImageFilter.cxx
   vtkITKBayesianClassifierImageFilter.h

   A VTK wrapper for itkBayesianClassificationImageFilter, for slicer.

   This will ultimately move into slicer2/Modules/vtkITK/cxx
    
