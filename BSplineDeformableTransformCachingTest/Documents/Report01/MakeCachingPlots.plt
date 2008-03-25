
  plot "AllCachingResults.txt" using 5:9 with linespoints lw 3 title 'Caching', "AllNoCachingResults.txt" using 5:9 with linespoints lw 3 title 'NoCaching'
  set xlabel "Number of Samples"
  show xlabel
  set ylabel "Execution Time (secs)"
  show ylabel
  set title "Execution Time vs. Samples Size With and Without B-Spline Transform Weight Caching" 
#  pause -1;
  set out 'CachingTiming.ps'
  set terminal postscript color landscape enhanced "Helvetica" 14 
  replot
  set size 1,1

