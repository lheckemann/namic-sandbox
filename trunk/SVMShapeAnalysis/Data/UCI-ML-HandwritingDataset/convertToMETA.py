#!/usr/bin/python
import array

numImagesEachDigit = 200

# read the ascii data from file
f = open('mfeat-pix','r');
allLines = f.readlines();
f.close()

thisDigit = 0
numThisDigit = 0
for line in allLines:
    # convert line containing ascii numbers into
    # an array of floats
    a = array.array('f');
    a.fromlist(map(float,line.split()));
    
    # construct the new filename
    dataFileName = '%d_%03d.raw' % (thisDigit, numThisDigit)

    # write the file
    f = open(dataFileName,'wb')
    a.tofile(f)
    f.close()
        
    # write the meta header
    headerFileName = '%d_%03d.mvh' % (thisDigit, numThisDigit)
    f = open(headerFileName,'w')
    f.write('Length = 240\n')
    f.write('ElementType = MET_FLOAT\n')
    f.write('BinaryData = True\n')    
    f.write('ElementDataFile = %s\n' % dataFileName)
    f.close()
    
    # increment filename data
    numThisDigit += 1
    if numThisDigit == numImagesEachDigit:
        numThisDigit = 0
        thisDigit += 1    
