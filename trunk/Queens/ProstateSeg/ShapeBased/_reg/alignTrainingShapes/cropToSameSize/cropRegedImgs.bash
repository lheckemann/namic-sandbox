#!/bin/bash


# the x-y dim has size 576*576 and 640*640, z-dim has 44 and 40
# what this file does is to crop the 640's and 44's to 576's and 40's
# while keeping the center still be the center after crop.

# thus after this, all files will have 576x576x40
oriDir=..


# p10-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p10-Endo-seg-reg.nrrd -o p10-Endo-seg-reg-sameSz.nrrd


# p11-Endo-seg-reg.nrrd
# 640 640 44
unu crop -min 32 32 2 -max M-32 M-32 M-2 -i ${oriDir}/p11-Endo-seg-reg.nrrd -o p11-Endo-seg-reg-sameSz.nrrd


# p12-Endo-seg-reg.nrrd
# 640 640 44
unu crop -min 32 32 2 -max M-32 M-32 M-2 -i ${oriDir}/p12-Endo-seg-reg.nrrd -o p12-Endo-seg-reg-sameSz.nrrd


# p13-Endo-seg-reg.nrrd
# 640 640 44
unu crop -min 32 32 2 -max M-32 M-32 M-2 -i ${oriDir}/p13-Endo-seg-reg.nrrd -o p13-Endo-seg-reg-sameSz.nrrd


# p14-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p14-Endo-seg-reg.nrrd -o p14-Endo-seg-reg-sameSz.nrrd


# p15-Endo-seg-reg.nrrd
# 640 640 40
unu crop -min 32 32 0 -max M-32 M-32 M -i ${oriDir}/p15-Endo-seg-reg.nrrd -o p15-Endo-seg-reg-sameSz.nrrd


# p16-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p16-Endo-seg-reg.nrrd -o p16-Endo-seg-reg-sameSz.nrrd


# p17-Endo-seg-reg.nrrd
# 640 640 44
unu crop -min 32 32 2 -max M-32 M-32 M-2 -i ${oriDir}/p17-Endo-seg-reg.nrrd -o p17-Endo-seg-reg-sameSz.nrrd


# p18-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p18-Endo-seg-reg.nrrd -o p18-Endo-seg-reg-sameSz.nrrd


# p20-Endo-seg-reg.nrrd
# 640 640 44
unu crop -min 32 32 2 -max M-32 M-32 M-2 -i ${oriDir}/p20-Endo-seg-reg.nrrd -o p20-Endo-seg-reg-sameSz.nrrd


# p2-Endo-seg-reg.nrrd
# 640 640 44
unu crop -min 32 32 2 -max M-32 M-32 M-2 -i ${oriDir}/p2-Endo-seg-reg.nrrd -o p2-Endo-seg-reg-sameSz.nrrd


# p3-Endo-seg-reg.nrrd
# 640 640 44
unu crop -min 32 32 2 -max M-32 M-32 M-2 -i ${oriDir}/p3-Endo-seg-reg.nrrd -o p3-Endo-seg-reg-sameSz.nrrd


# p4-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p4-Endo-seg-reg.nrrd -o p4-Endo-seg-reg-sameSz.nrrd


# p5-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p5-Endo-seg-reg.nrrd -o p5-Endo-seg-reg-sameSz.nrrd


# p6-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p6-Endo-seg-reg.nrrd -o p6-Endo-seg-reg-sameSz.nrrd


# p7-Endo-seg-reg.nrrd
# 640 640 44
unu crop -min 32 32 2 -max M-32 M-32 M-2 -i ${oriDir}/p7-Endo-seg-reg.nrrd -o p7-Endo-seg-reg-sameSz.nrrd


# p8-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p8-Endo-seg-reg.nrrd -o p8-Endo-seg-reg-sameSz.nrrd


# p9-Endo-seg-reg.nrrd
# 576 576 44
unu crop -min 0 0 2 -max M M M-2 -i ${oriDir}/p9-Endo-seg-reg.nrrd -o p9-Endo-seg-reg-sameSz.nrrd


# p1-Endo-seg-reg.nrrd
# 576 576 40
unu crop -min 0 0 0 -max M M M -i ${oriDir}/p1-Endo-seg-reg.nrrd -o p1-Endo-seg-reg-sameSz.nrrd

# p19-Endo-seg-reg.nrrd
# 576 576 40
unu crop -min 0 0 0 -max M M M -i ${oriDir}/p19-Endo-seg-reg.nrrd -o p19-Endo-seg-reg-sameSz.nrrd
