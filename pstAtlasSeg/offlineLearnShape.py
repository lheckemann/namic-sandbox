#!/usr/bin/python

import os
import sys
import string

import datetime

## dir for namic challenge data, on u4 they are at:
##   work/10/atlasSeg/prostate/namicProstate
if len(sys.argv) < 2:
    print "args: dirForNamicChallengeData"
    sys.exit(-1)

dataDir = sys.argv[1]

regExe = "src/r/reg_3d_similarity_mse"
if len(sys.argv) >= 3:
    regExe = sys.argv[2]


## create result dir to store registered training shapes
rsltDir = "registeredTrainingShapes"
if os.path.exists(rsltDir):
    print "Error: registered trainging shape dir already existed"
    sys.exit(-1)

cmd = "mkdir " + rsltDir
os.system(cmd)


## list of all training shapes
pstTrainingBinaryImage = ["000029.00003.004.nrrd",
                          "000035.00001.004.nrrd",
                          "000036.00002.004.nrrd",
                          "000041.00002.004.nrrd",
                          "000045.00002.005.nrrd",
                          "000049.00001.004.nrrd",
                          "000054.00003.004.nrrd",
                          "000055.00001.004.nrrd",
                          "000056.00002.003.nrrd",
                          "000060.00001.004.nrrd",
                          "000064.00003.004.nrrd",
                          "000069.00001.004.nrrd",
                          "000073.00002.004.nrrd",
                          "000085.00002.004.nrrd",
                          "000088.00002.002.nrrd"]
    
for i in range(len(pstTrainingBinaryImage)):
    pstTrainingBinaryImage[i] = os.path.join(dataDir, pstTrainingBinaryImage[i])


#n = len(pstTrainingImgAll)

fixedShape = pstTrainingBinaryImage[0]

for traingShape in pstTrainingBinaryImage:
    registeredTrainingShapeName = traingShape.replace(".nrrd", "-reg.nrrd")
    registeredTrainingShapeName = os.path.basename(registeredTrainingShapeName)
    registeredTrainingShapeName = os.path.join(rsltDir, registeredTrainingShapeName)

    #regCmd = " ".join([regExe, fixedShape, traingShape, registeredTrainingShapeName, "&"])
    regCmd = " ".join([regExe, fixedShape, traingShape, registeredTrainingShapeName])
    os.system(regCmd)



## learn the shapes
dataDir = "registeredTrainingShapesb"

exeFile="src/r/imagePCAAnalysisTest2"
if len(sys.argv) >= 3:
    exeFile = sys.argv[2]



## create result dir to store mean shape and eigen shapes
rsltDir = "meanAndEigenShapes"
if os.path.exists(rsltDir):
    print "Error: meanAndEigenShapes dir already existed"
    sys.exit(-1)

cmd = "mkdir " + rsltDir
os.system(cmd)


## list all the registered shapes
cmd = "ls registeredTrainingShapes/* > fileList.txt"
os.system(cmd)



for i in range(10):
    print i
    cmd = " ".join([exeFile, "fileList.txt", str(i)])
    os.system(cmd)

    ## move the mean shape to the result dir
    mvCmd = " ".join(["mv", "meanImage.nrrd", rsltDir])
    os.system(mvCmd)


    ## move the eigen shapes to the result dir
    eigenName = "eigenImg_" + str(i) + ".nrrd"
    mvCmd = " ".join(["mv", eigenName, rsltDir])
    os.system(mvCmd)


## remove list of all the registered shapes
cmd = "rm fileList.txt"
os.system(cmd)

