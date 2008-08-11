#!/usr/bin/perl

$COMMAND = './PerformanceTestClient';
$SERVER  = 'localhost';
$PORT    = 18944;
#$NDATA   = 100;
$DURATION= 1; # in second
$LOGFILE = 'client.log';
$PTPLOG  = 'ptp-XXX.log';

{
    
    print "***** Test phase 1-2/5 -- TRANSFORM -- Frame rate vs CPU load / delay *****\n";
    $channel = 0.5;
    for ($j = 0; $j < 5; $j ++) {   # from 1 to 16 channels
        $channel = $channel*2;
        $fps = 8;
        for ($i = 0; $i < 8; $i ++) { # from 16 to 2048 fps
            $fps = $fps * 2;
            $file = sprintf("test-t-%04d-%02d", $fps, $channel);
            if ($PTPLOG != '') {
                system("wc -l $PTPLOG >> $LOGFILE");
            }
            $line = "$COMMAND -h $SERVER -p $PORT -t $file -d $DURATION -f $fps -c $channel >> $LOGFILE";
            print($line."\n");
            system($line);
            sleep(2);
        }
    }

#    print "***** Test phase 2 -- TRANSFORM -- Channel vs CPU load / delay @ 100 fps *****\n";
#
#    $fps = 128;
#    for ($i = 0; $i < 10; $i ++) {
#        $channel = $i + 1;
#        $file = sprintf("test-t-%04d-%02d", $fps, $channel);
#        if ($PTPLOG != '') {
#            system("wc -l $PTPLOG >> $LOGFILE");
#        }
#        $line = "$COMMAND -h $SERVER -p $PORT -t $file -d $DURATION -f $fps -c $channel >> $LOGFILE";
#        print($line."\n");
#        system($line);
#        sleep(2);
#    }

    print "***** Test phase 3/5 -- IMAGE -- Frame rate vs CPU load / delay  *****\n";
    $channel = 1;
    $x = 256;
    $y = 256;
    $z = 1;
    $fps = 0.5;
    for ($i = 0; $i < 8; $i ++) {  # from 1 to 256 fps (from 131072 bytes/sec to 33554432 bytes/s)
        $fps = $fps*2;
        $file = sprintf("test-i-%04d-%02d-%d-%d-%d", $fps, $channel, $x, $y, $z);
        if ($PTPLOG != '') {
            system("wc -l $PTPLOG >> $LOGFILE");
        }
        $line = "$COMMAND -h $SERVER -p $PORT -t $file -d $DURATION -F $fps -C $channel -x $x -y $y -z $z >> $LOGFILE";
        print($line."\n");
        system($line);
        sleep(2);
    }

    print "***** Test phase 4/5 -- IMAGE -- image size vs CPU load / delay @ 32 fps *****\n";
    $fps = 32;
    $channel = 1;
    $x = 64;
    $y = 32;
    $z = 0.5;
    for ($i = 0; $i < 8; $i ++) {  # from 131072 bytes/sec to 33554432 bytes/s -- equivalent to phase 3
        $z = $z*2;
        $file = sprintf("test-i-%04d-%02d-%d-%d-%d", $fps, $channel, $x, $y, $z);
        if ($PTPLOG != '') {
            system("wc -l $PTPLOG >> $LOGFILE");
        }
        $line = "$COMMAND -h $SERVER -p $PORT -t $file -d $DURATION -F $fps -C $channel -x $x -y $y -z $z >> $LOGFILE";
        print($line."\n");
        system($line);
        sleep(2);
    }

    #print "***** Test phase 5 -- TRANSFORM/IMAGE -- Frame raate vs CPU load / delay @ 100fps (trak) + 10fps (img)*****\n";
    print "***** Test phase 5/5 -- TRANSFORM/IMAGE -- Frame raate vs CPU load / delay @ 100fps (trak) + 256*256*2*32 bytes/s (img)*****\n";
    
    $trackfps = 100;
    $trackchannel = 1;
    $imgchannel = 1;
    $x = 256;
    $y = 256;
    #$z = 0.5;
    $imgfps = 0.5;
    for ($i = 0; $i < 6; $i ++) {
        $imgfps = $imgfps * 2;
        $z = 32 / $imgfps;
        $file = sprintf("test-t-%04d-%02d-i-%04d-%02d-%d-%d-%d", $trackfps, $trackchannel, $imgfps, $imgchannel, $x, $y, $z);
        if ($PTPLOG != '') {
            system("wc -l $PTPLOG >> $LOGFILE");
        }
        $line = "$COMMAND -h $SERVER -p $PORT -t $file -d $DURATION -f $trackfps -c $trackchannel -F $imgfps -C $imgchannel -x $x -y $y -z $z >> $LOGFILE";
        print($line."\n");
        system($line);
        sleep(2);
    }
        
}



