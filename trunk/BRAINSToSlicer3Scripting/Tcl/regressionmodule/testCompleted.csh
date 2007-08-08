#!/bin/tcsh
if ( $#argv >= 1 ) set p=$1
if ( $#argv == 0 ) set p=gcc3/DEBUG

if ( $#argv >= 2 ) set cri=$2
if ( $#argv <= 1 ) set cri=Completed

echo "    Testing $p for ${cri}:"
echo

foreach f (`ls -1p $p/*_*_*` )
    if ( -f $f ) then
  if ( $f:r == $f ) then
      tail -1 $f | grep -v $cri > /dev/null
      if ($status == 0) echo "not ${cri}:  " $f 
  endif
    endif
end
