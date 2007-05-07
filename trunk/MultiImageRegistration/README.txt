This project by Polina Golland, and implemented by

+ Serdar Balci
+ Mert Sabuncu

The goal of the project is to register a collection
of images simultaneously, instead of pair-wise. See 
"GroupWiseRegistration.pdf" for details. Currently, 
implemented methods are
    Congealing with entropy 
    Congealing with variance
    Registering to the mean using pairwise mutual information
    Registering to mean and variance images using mutual information
    Minimizing joint entropy
    
Currently supported deformation models
    Affine transform
    Affine followed by cubic BSpline with mesh refinement
    
#
#
#  DOCUMENTATION
#
#        
    
For the use of the binary file see SourceGuide.txt
    
For the computation of objective functions see GroupWiseRegistration.pdf    

