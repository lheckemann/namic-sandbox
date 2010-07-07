#!/usr/bin/python

import os
import glob
import sys
import string

import datetime


segExe = "src/r/atlasSegMIExe_outputPr"

if len(sys.argv) < 3:
    print "args: targetT2Image dirForNamicChallengeData"
    sys.exit(-1)

targetT2Image = sys.argv[1]
dataDir = sys.argv[2]


pstTrainingImgAll = ["000029.00003.002.nrrd", 
                     "000035.00001.001.nrrd", 
                     "000036.00002.002.nrrd", 
                     "000041.00002.001.nrrd", 
                     "000045.00002.004.nrrd", 
                     "000049.00001.001.nrrd", 
                     "000054.00003.001.nrrd", 
                     "000055.00001.001.nrrd", 
                     "000056.00002.001.nrrd", 
                     "000060.00001.002.nrrd", 
                     "000064.00003.001.nrrd", 
                     "000069.00001.001.nrrd", 
                     "000073.00002.001.nrrd", 
                     "000085.00002.001.nrrd", 
                     "000088.00002.001.nrrd"]

pstLabelImgAll = ["000029.00003.004.nrrd",
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

if len(pstTrainingImgAll) != len(pstLabelImgAll):
    print "Error: number of training images NOT match number of label images"
    sys.exit(-1)
    

for i in range(len(pstTrainingImgAll)):
        pstTrainingImgAll[i] = os.path.join(dataDir, pstTrainingImgAll[i])
        pstLabelImgAll[i] = os.path.join(dataDir, pstLabelImgAll[i])



tf = open("trainingImgName.txt", 'w')
for ii in pstTrainingImgAll:
    tf.write(ii + "\n");
tf.close();

lf = open("labelImgName.txt", 'w')
for ii in pstLabelImgAll:
    lf.write(ii + "\n");
lf.close();


outputName = targetT2Image.replace(".nrrd", "-atlas.nrrd");
outputName = os.path.basename(outputName)
outputName = os.path.join(outputName)

segCmd = " ".join([segExe, targetT2Image, "trainingImgName.txt", "labelImgName.txt", outputName])
os.system(segCmd)


# remove intermediate files
os.system("rm trainingImgName.txt")
os.system("rm labelImgName.txt")

# move some tem files to a separate dir
os.system("mkdir atlasSegTmp")
os.system("mv regTrainImg*.nrrd atlasSegTmp")
os.system("mv regLabelImg*.nrrd atlasSegTmp")



## now the contour evolution

segExe = "src/r/probMapShapeBasedSegExe"

targetT2Image = sys.argv[1]
atlasImage = outputName


meanShape = "meanAndEigenShapes/meanImage.nrrd"
cmd = "ls meanAndEigenShapes/ei*.nrrd > eig.txt"
os.system(cmd);


# generate results dir
now = datetime.datetime.now()
rsltDirPrefix = os.path.basename(targetT2Image)
rsltDirPrefix = rsltDirPrefix.replace(".nrrd", "");
rsltDir = rsltDirPrefix + "-" + str(now.year) + "-" + str(now.month) + "-" + str(now.day) + "-" + str(now.hour)
if os.path.exists(rsltDir):
    rsltDir += ("-" + str(now.minute))
cmd = "mkdir " + rsltDir
os.system(cmd)
#print cmd


for numIter in [2500]:
    for curvatureWeight in [0.6]:
        for homogeniety in [0.8]:

            outputName = targetT2Image.replace(".nrrd", "-finalSeg");
            outputName = outputName + '-' + str(numIter) + '-' + str(homogeniety) + '-' + str(curvatureWeight) + '.nrrd'
            outputName = os.path.basename(outputName)
            outputName = os.path.join(rsltDir, outputName)
    
            segCmd = " ".join([segExe, targetT2Image, outputName, atlasImage, meanShape, "eig.txt", str(numIter), str(homogeniety), str(curvatureWeight)])
            os.system(segCmd)


cmd = "rm eig.txt"
os.system(cmd);

