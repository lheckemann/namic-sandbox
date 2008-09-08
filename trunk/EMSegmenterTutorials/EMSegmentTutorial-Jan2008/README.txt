Slicer3 EMSegmenter Tutorial
===============================

This Tutorial demonstrates how to use the EMSegmenter command line
interface to segment new data using a pre-defined parameter set.  

The tutorial contains (a) an EMSegmenter parameter set; (b) data for
the tutorial: an atlas and input images; (c) input data for two new
patients; and (d) some scripts that demonstrate command-line usage of
the EMSegmenter

If you want to do the registration from within Slicer3, you can load
the tutorials MRML scene, change the parameters or input images, and
run the segmentation.  See tutorial slides.

How to get started
======================

  1) Build a recent version of Slicer3 (svn version from 7 January
     2007 or later).

  2) Download this tutorial.  

  3) IMPORTANT!!!: edit the scripts in this directory to reflect the
     location of your Slicer3 bin directory

  4) try running the first script:
       source Run-BrainTutorial-Defaults.sh

  Running out of memory?  Try runing the "_small" scripts.  They use
  low resolution versions of the same images.

How to run the segmenter with the tutorial data
===============================================

  1) The script "Run-BrainTutorial-Defaults.sh" demonstrates how to
     run the EMSegmenter on the tutorial data.  The resulting labelmap
     is written into the VolumeData_Output directory.  

     Try adding the "--intermediateResultsDirectory myDirectoryName"
     flag to write out intermediate data.  Intermediate data is saved
     in the "myDirectoryName" directory: you can load this scene to
     visualize the intermediate data (e.g. intensity normalized and
     registered images) and segmentation results.

How to run the segmenter with new data
===============================================

  1) The other scripts demonstrate how to run the EMSegmenter on new
     data.  Because the tutorial EMSegment parameters are designed to
     accept one T1 and one T2 input image, each script supplies one
     new T1 and one T2 to be segmented (NB: order is important).  One
     script shows how to specify atlas images on the command line.



