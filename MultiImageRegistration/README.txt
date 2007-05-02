This project by Polina Golland, and implemented by

+ Serdar Balci
+ Mert Sabuncu

The goal of the project is to register a collection
of images simultaneously, instead of pair-wise. Currently, 
implemented methods are
    Congealing with entropy 
    Congealing with variance
    Registering to the mean using pairwise mutual information
    Registering to mean and variance images using mutual information
    Minimizing joint entropy
    
Currently supported deformation models
    Affine transform
    Affine followed by cubic BSpline with mesh refinement
    
    
The code is in the svn repositories. If you dont have access to ITK's
svn servers, you can get read access to the repository by simply
downloading it according to the instructions in the following Wiki page:

http://wiki.na-mic.org/Wiki/index.php/Engineering:SandBox
    


