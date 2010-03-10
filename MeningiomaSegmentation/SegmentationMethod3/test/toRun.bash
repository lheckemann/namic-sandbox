#!/bin/bash

# Seg image 1, 1000 iteration, smooth factor 0.8, pick a point (68 63 43) in the object, use global statistics 1
echo Running case 1
date
./bin/meningiomaSegmentor ../../Data/roi_01_2x.nrrd r1.nrrd 1000 0.8 68 63 43 1
date


echo Running case 2
date
# Seg image 2, 1500 iteration, smooth factor 0.8, pick a point (59 58 39) in the object, use global statistics 1
./bin/meningiomaSegmentor ../../Data/roi_02_2x.nrrd r2.nrrd 2000 0.8 59 58 39 1
date

# Seg image 4, 500 iteration, smooth factor 0.8, pick a point (35 34 32) in the object, use global statistics 1
echo Running case 4
date
./bin/meningiomaSegmentor ../../Data/roi_04_2x.nrrd r4.nrrd 500 0.8 35 34 32 1
date

# Seg image 8, 500 iteration, smooth factor 1.0, pick a point (36 42 18) in the object, 
# Use global statistics 0 (Thus, using local statistics )
# This is slower than the global method, but this is a robust method to handle the difficult roi8 image.
echo Running case 8
date
./bin/meningiomaSegmentor ../../Data/roi_08_2x.nrrd r8.nrrd 500 1.0 36 42 18 0
date