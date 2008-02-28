
  plot "AllCachingMemoryResults.txt" using 5:9 with linespoints title 'Caching', "AllNoCachingMemoryResults.txt" using 5:9 with linespoints  title 'NoCaching'
  set xlabel "Number of Samples"
  show xlabel
  set ylabel "Memory Usage (kB)"
  show ylabel
  set title "Memory Usage vs. Samples Size With and Without Caching" 
#  pause -1;
  set out 'CachingMemory.ps'
  set terminal postscript color landscape enhanced "Helvetica" 14 
  replot
  set size 1,1

