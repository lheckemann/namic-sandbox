#!/bin/bash

#
# before start, please launch 10 servers and assign port # 18944 - 18953
#

### Settings
SERVERADDR=localhost


### Tracking -- frame rate
#
# program name                        port# fps  filename            N    Type
#----------------------------------------------------------------------------------
echo "Testing from 1 fps to 10 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 1    test-t-f0001-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 2    test-t-f0002-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 3    test-t-f0003-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 4    test-t-f0004-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 5    test-t-f0005-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 6    test-t-f0006-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 7    test-t-f0007-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 8    test-t-f0008-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 9    test-t-f0009-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 10   test-t-f0010-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 11 fps to 20 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 11   test-t-f0011-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 12   test-t-f0012-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 13   test-t-f0013-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 14   test-t-f0014-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 15   test-t-f0015-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 16   test-t-f0016-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 17   test-t-f0017-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 18   test-t-f0018-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 19   test-t-f0019-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 20   test-t-f0020-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 21 fps to 30 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 21   test-t-f0021-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 22   test-t-f0022-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 23   test-t-f0023-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 24   test-t-f0024-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 25   test-t-f0025-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 26   test-t-f0026-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 27   test-t-f0027-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 28   test-t-f0028-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 29   test-t-f0029-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 30   test-t-f0030-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 40 fps to 100 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 40   test-t-f0040-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 50   test-t-f0050-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 60   test-t-f0060-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 70   test-t-f0070-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 80   test-t-f0080-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 90   test-t-f0090-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 100  test-t-f0100-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 110 fps to 200 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 110  test-t-f0110-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 120  test-t-f0120-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 130  test-t-f0130-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 140  test-t-f0140-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 150  test-t-f0150-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 160  test-t-f0160-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 170  test-t-f0170-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 180  test-t-f0180-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 190  test-t-f0190-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 200  test-t-f0200-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 210 fps to 300 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 210  test-t-f0210-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 220  test-t-f0220-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 230  test-t-f0230-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 240  test-t-f0240-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 250  test-t-f0250-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 260  test-t-f0260-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 270  test-t-f0270-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 280  test-t-f0280-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 290  test-t-f0290-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 300  test-t-f0300-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 310 fps to 400 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 310  test-t-f0310-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 320  test-t-f0320-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 330  test-t-f0330-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 340  test-t-f0340-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 350  test-t-f0350-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 360  test-t-f0360-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 370  test-t-f0370-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 380  test-t-f0380-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 390  test-t-f0390-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 400  test-t-f0400-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 410 fps to 500 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 410  test-t-f0410-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 420  test-t-f0420-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 430  test-t-f0430-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 440  test-t-f0440-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 450  test-t-f0450-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 460  test-t-f0460-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 470  test-t-f0470-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 480  test-t-f0480-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 490  test-t-f0490-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 500  test-t-f0500-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 510 fps to 600 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 510  test-t-f0510-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 520  test-t-f0520-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 530  test-t-f0530-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 540  test-t-f0540-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 550  test-t-f0550-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 560  test-t-f0560-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 570  test-t-f0570-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 580  test-t-f0580-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 590  test-t-f0590-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 600  test-t-f0600-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 610 fps to 700 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 610  test-t-f0610-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 620  test-t-f0620-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 630  test-t-f0630-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 640  test-t-f0640-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 650  test-t-f0650-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 660  test-t-f0660-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 670  test-t-f0670-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 680  test-t-f0680-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 690  test-t-f0690-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 700  test-t-f0700-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 710 fps to 800 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 710  test-t-f0710-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 720  test-t-f0720-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 730  test-t-f0730-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 740  test-t-f0740-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 750  test-t-f0750-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 760  test-t-f0760-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 770  test-t-f0770-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 780  test-t-f0780-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 790  test-t-f0790-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 800  test-t-f0800-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 810 fps to 900 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 810  test-t-f0810-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 820  test-t-f0820-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 830  test-t-f0830-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 840  test-t-f0840-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 850  test-t-f0850-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 860  test-t-f0860-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 870  test-t-f0870-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 880  test-t-f0880-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 890  test-t-f0890-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 900  test-t-f0900-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 910 fps to 1000 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 910  test-t-f0910-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 920  test-t-f0920-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 930  test-t-f0930-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 940  test-t-f0940-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 950  test-t-f0950-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 960  test-t-f0960-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 970  test-t-f0970-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 980  test-t-f0980-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 990  test-t-f0990-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1000 test-t-f1000-c1.csv 1000 TRANSFORM; sleep 2

echo "Testing from 1100 fps to 2000 fps..."
./PerformanceTestClient ${SERVERADDR} 18944 1100 test-t-f1100-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1200 test-t-f1200-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1300 test-t-f1300-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1400 test-t-f1400-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1500 test-t-f1500-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1600 test-t-f1600-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1700 test-t-f1700-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1800 test-t-f1800-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 1900 test-t-f1900-c1.csv 1000 TRANSFORM; sleep 2
./PerformanceTestClient ${SERVERADDR} 18944 2000 test-t-f2000-c1.csv 1000 TRANSFORM; sleep 2







