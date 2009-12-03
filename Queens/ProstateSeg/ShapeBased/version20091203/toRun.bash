#!/bin/bash


# For p1, the left and right point in the middle slice is (90 126 13), (165 124 13)
# After this
./r/wholeSeg ./data/p1-s1-701_T1W.nrrd a1.nrrd 90 126 13 165 124 13
unu 2op lte a1.nrrd 2 | unu convert -t uchar -o b1.nrrd # 2 instead of 0 to inclose more


# For p2, the left and right point in the middle slice is (100, 138, 17), (158, 133, 17)
# After this
./r/wholeSeg ./data/p2-s1-35.nrrd a2.nrrd 100 138 17 158 133 17
unu 2op lte a2.nrrd 2 | unu convert -t uchar -o b2.nrrd # 2 instead of 0 to inclose more


# For p7, the left and right point in the middle slice is 81 134 12, 143 128 12
# After this
./r/wholeSeg ./data/p7-s1-032_gre_te15_20cmfov.nrrd a7.nrrd 81 134 12 143 128 12
unu 2op lte a7.nrrd 1 | unu convert -t uchar -o b7.nrrd # 2 instead of 0 to inclose more


