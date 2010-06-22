import os
import glob
import sys
import string

import datetime


#segExe = "../src/r/atlasSegMIExe"
segExe = "src/r/atlasSegMIExe_outputPr"

if len(sys.argv) < 2:
    print "args: dirForNamicChallengeData"
    sys.exit(-1)

dataDir = sys.argv[1]


# #HOME = os.getenv("HOME")
# #projDir = os.path.join(os.getcwd(), "..")

# dataDir = os.path.join(projDir, "prostate", "namicProstate")


# generate results dir
now = datetime.datetime.now()
rsltDir = "pstSeg-" + str(now.year) + "-" + str(now.month) + "-" + str(now.day) + "-" + str(now.hour)
if os.path.exists(rsltDir):
    rsltDir += ("-" + str(now.minute))
cmd = "mkdir " + rsltDir
os.system(cmd)
#print cmd



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
for i in range(len(pstTrainingImgAll)):
        pstTrainingImgAll[i] = os.path.join(dataDir, pstTrainingImgAll[i])


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
for i in range(len(pstLabelImgAll)):
        pstLabelImgAll[i] = os.path.join(dataDir, pstLabelImgAll[i])


n = len(pstTrainingImgAll)

for i in range(1, n):
        rawImgName = pstTrainingImgAll[i]
        #print os.path.basename(rawImgName)


        tf = open("trainingImgName.txt", 'w')
        trainImgName = pstTrainingImgAll[0:i] + pstTrainingImgAll[i+1:]
        for ii in trainImgName:
                tf.write(ii + "\n");
        tf.close();


        lf = open("labelImgName.txt", 'w')
        labelImgName = pstLabelImgAll[0:i] + pstLabelImgAll[i+1:]
        for ii in labelImgName:
                lf.write(ii + "\n");
        lf.close();


        outputName = rawImgName.replace(".nrrd", "-atlas.nrrd");
        outputName = os.path.basename(outputName)
        outputName = os.path.join(rsltDir, outputName)

        segCmd = " ".join([segExe, rawImgName, "trainingImgName.txt", "labelImgName.txt", outputName])
        os.system(segCmd)
