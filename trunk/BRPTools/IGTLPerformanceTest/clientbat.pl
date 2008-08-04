#!/usr/bin/perl

$COMMAND = './PerformanceTestClient';
$SERVER  = 'localhost';
$PORT    = 18944;
#$NDATA   = 100;
$DURATION= 10; # in second
$LOGFILE = 'client.log';
$PTPLOG  = 'ptp-XXX.log';

{
    print "***** Test phase 1 -- Frame rate vs CPU load / delay *****\n";
    $fps = 2;
    for ($i = 0; $i < 10; $i ++) {
        $fps = $fps * 2;
        $file = sprintf("test-t-%04d", $fps);
        system("wc -l $PTPLOG >> $LOGFILE");
        $line = "$COMMAND -h $SERVER -p $PORT -t $file -d $DURATION -f $fps -c 1 >> $LOGFILE";
        print($line."\n");
        system($line);
        sleep(2);
    }

    print "***** Test phase 2 -- Channel vs CPU load / delay @ 128 fps *****\n";

        
}



