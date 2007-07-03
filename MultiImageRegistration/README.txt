This project by Polina Golland, and implemented by

+ Serdar K Balci (serdar at csail.mit.edu)

The goal of the project is to register a collection
of images simultaneously, instead of pair-wise.
Currently, implemented methods are
    Congealing with entropy 
    Congealing with variance
    
Currently supported deformation models
    Affine transform
    Affine followed by cubic BSpline with mesh refinement
    
   
#
#
# DATA
#
#    
    
Obtain test data at
http://public.kitware.com/pub/itk/Data/BrainWeb/BrainPart1.tgz
http://public.kitware.com/pub/itk/Data/BrainWeb/BrainPart2.tgz
http://public.kitware.com/pub/itk/Data/BrainWeb/BrainPart3.tgz
http://public.kitware.com/pub/itk/Data/DTI/Reconstructed/FAImage.tar.gz    
    
Extract files and provide path to cmake for testing.
Type ctest or "make test" to run the tests    
    
#
#
#  DOCUMENTATION
#
#        
    
    For the use of the binary file see SourceGuide.txt
