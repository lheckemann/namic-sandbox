#!/usr/bin/perl

$COMMAND = './PerformanceTestClient';
$SERVER  = 'localhost';
$PORT    = 18944;
$NDATA   = 100;
$LOGFILE = 'client.log';
$PTPLOG  = 'ptp-XXX.log';

{
    print "***** Test phase 1 -- Frame rate vs CPU load *****\n";
    $fps = 2;
    for ($i = 0; $i < 10; $i ++) {
        $fps = $fps * 2;
        $file = sprintf("test-t-%04d.csv", $fps);
        system("wc -l $PTPLOG >> $LOGFILE");
        $line = "$COMMAND $SERVER $PORT $fps $file $NDATA TRANSFORM >> $LOGFILE";
        print($line."\n");
        system($line);
        sleep(2);
    }
        
}



