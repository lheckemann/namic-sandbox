Overview
--------  

This package currently contains a command-line utility for clustering
fibers using the Normalized Cut algorithm.

Data
--------
There is a sample fiber file of the fornix included in the directory
called fornix.fib

Usage
--------
Usage of the program after compilation is 
ncutfib <infilename> <outfilename> <# clusters> <scale>

infilename: The input fiber file
outfilename: The name of the fiber file to write the output to.  The
output is a colorized version of the input file based on clustering
# clusters: The number of bundles to form.
scale: A parameter [0,1] that controls the scale of the distance
measurements.  .2 is typically a good value to use.

Sample
----
ncutfib fornix.fib clustered.fib 2 .2


Contact
--------
Casey Goodlett
gcasey@cs.unc.edu
