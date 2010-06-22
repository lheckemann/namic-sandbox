import os
import sys
import string

import datetime


segExe = "src/r/SFLSRobustStat3DTestProbMap"

if len(sys.argv) < 3:
    print "args: dirForNamicChallengeData probabilityMapDir(atlas dir)"
    sys.exit(-1)

imgDir = sys.argv[1]
probabilityMapDir = sys.argv[2]


# generate results dir
now = datetime.datetime.now()
rsltDir = "pstSeg-" + str(now.year) + "-" + str(now.month) + "-" + str(now.day) + "-" + str(now.hour)
if os.path.exists(rsltDir):
    rsltDir += ("-" + str(now.minute))
cmd = "mkdir " + rsltDir
os.system(cmd)
#print cmd



# prepare image data
#imgDir = '../namicProstate'

imgAll = ["000035.00001.001.nrrd", 
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
for i in range(len(imgAll)):
    imgAll[i] = os.path.join(imgDir, imgAll[i])


# prepare probability maps
#probabilityMapDir = '../pstSeg-2010-6-16-9'
probabilityMapAll = ['000035.00001.001-atlas.nrrd',
                     '000036.00002.002-atlas.nrrd',
                     '000041.00002.001-atlas.nrrd',
                     '000045.00002.004-atlas.nrrd',
                     '000049.00001.001-atlas.nrrd',
                     '000054.00003.001-atlas.nrrd',
                     '000055.00001.001-atlas.nrrd',
                     '000056.00002.001-atlas.nrrd',
                     '000060.00001.002-atlas.nrrd',
                     '000064.00003.001-atlas.nrrd',
                     '000069.00001.001-atlas.nrrd',
                     '000073.00002.001-atlas.nrrd',
                     '000085.00002.001-atlas.nrrd',
                     '000088.00002.001-atlas.nrrd']
for i in range(len(probabilityMapAll)):
    probabilityMapAll[i] = os.path.join(probabilityMapDir, probabilityMapAll[i])


for i in range(len(probabilityMapAll)):
    for numIter in [100, 500, 900]:
        for curvatureWeight in [0.5, 0.7, 0.9]:
            for homogeniety in [0.5, 0.7, 0.9]:

                outputName = imgAll[i].replace(".nrrd", "-finalSeg");
                outputName = outputName + '-' + str(numIter) + '-' + str(curvatureWeight) + '-' + str(homogeniety) + '.nrrd'
                outputName = os.path.basename(outputName)
                outputName = os.path.join(rsltDir, outputName)
    
                segCmd = " ".join([segExe, imgAll[i], probabilityMapAll[i], outputName, str(numIter), str(curvatureWeight), str(homogeniety)])
                os.system(segCmd)
