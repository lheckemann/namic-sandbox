#! /usr/bin/perl

$flist = `ls test_*.mg*`;
@files = split  /\n/, $flist;

$dir = `pwd`;
chomp $dir;
print " crt dir = $dir \n";

foreach $file (@files)
  {
    chomp $file;
    print "processing file $file\n";

    foreach $type ("mgh","mgz")
      {
  $cmdline = "../mgh_test $file $dir/test.$type";
  print "\t trying to exec -> $cmdline\n";
  system($cmdline)==0 or die $!;

  system("mri_diff $file $dir/test.$type")==0 or die $!;

  system("rm test.$type")==0 or die $!;
      }
  }
