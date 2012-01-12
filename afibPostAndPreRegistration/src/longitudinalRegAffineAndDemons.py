import os.path
import os
import sys
import string

#import datetime

exeFile = "./r/registerPostToPreAffineAndDemons"

dataPath = "/home/gaoyi/usr/work/data/heart/utah/selected"


preEndoFileList = ["PREENDO01a.nrrd",
                   "PREENDO02a.nrrd",
                   "PREENDO03a.nrrd",
                   "PREENDO04a.nrrd",
                   "PREENDO05a.nrrd",
                   "PREENDO06a.nrrd",
                   "PREENDO07a.nrrd",
                   "PREENDO09a.nrrd",
                   "PREENDO10a.nrrd",
                   "PREENDO11a.nrrd",
                   "PREENDO12a.nrrd",
                   "PREENDO15a.nrrd",
                   "PREENDO16a.nrrd"]

preMRIFileList = ["PREMRI01.nrrd",
                  "PREMRI02.nrrd",
                  "PREMRI03.nrrd",
                  "PREMRI04.nrrd",
                  "PREMRI05.nrrd",
                  "PREMRI06.nrrd",
                  "PREMRI07.nrrd",
                  "PREMRI09.nrrd",
                  "PREMRI10.nrrd",
                  "PREMRI11.nrrd",
                  "PREMRI12.nrrd",
                  "PREMRI15.nrrd",
                  "PREMRI16.nrrd"]

postEndoFileList = ["POSTENDO01a.nrrd",
                    "POSTENDO02a.nrrd",
                    "POSTENDO03a.nrrd",
                    "POSTENDO04a.nrrd",
                    "POSTENDO05a.nrrd",
                    "POSTENDO06a.nrrd",
                    "POSTENDO07a.nrrd",
                    "POSTENDO09a.nrrd",
                    "POSTENDO10a.nrrd",
                    "POSTENDO11a.nrrd",
                    "POSTENDO12a.nrrd",
                    "POSTENDO15a.nrrd",
                    "POSTENDO16a.nrrd"]

postMRIFileList = ["POSTMRI01.nrrd",
                   "POSTMRI02.nrrd",
                   "POSTMRI03.nrrd",
                   "POSTMRI04.nrrd",
                   "POSTMRI05.nrrd",
                   "POSTMRI06.nrrd",
                   "POSTMRI07.nrrd",
                   "POSTMRI09.nrrd",
                   "POSTMRI10.nrrd",
                   "POSTMRI11.nrrd",
                   "POSTMRI12.nrrd",
                   "POSTMRI15.nrrd",
                   "POSTMRI16.nrrd"]


if len(preEndoFileList) != len(preMRIFileList):
        print "Error: numbers of files do not match."

if len(preMRIFileList) != len(postMRIFileList):
        print "Error: numbers of files do not match."

if len(postMRIFileList) != len(postEndoFileList):
        print "Error: numbers of files do not match."

n = len(postMRIFileList)

for it in range(n):
        preEndoFile = dataPath + "/" + preEndoFileList[it];
        preMRIFile = dataPath + "/" + preMRIFileList[it];
        postEndoFile = dataPath + "/" + postEndoFileList[it];
        postMRIFile = dataPath + "/" + postMRIFileList[it];

        postToPreEndo = os.path.basename(preEndoFileList[it])
        postToPreMRI = os.path.basename(preMRIFileList[it])

        postToPreEndo = postToPreEndo.replace(".nrrd", "_affineAndDemons_postToPreEndo.nrrd")
        postToPreMRI = postToPreMRI.replace(".nrrd", "_affineAndDemons_postToPreMRI.nrrd")

        cmdThe = " ".join([exeFile, postMRIFile, preMRIFile, postEndoFile, preEndoFile, postToPreMRI, postToPreEndo])
        #print cmdThe
        os.system(cmdThe)
