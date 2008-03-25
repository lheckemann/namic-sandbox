
  plot "AllThreadingMemoryResults.txt" using 5:9 with linespoints lw 3 title 'Threading', "AllDefaultMemoryResults.txt" using 5:9 with linespoints lw 3 title 'NoThreading'
  set xlabel "Number of Samples"
  show xlabel
  set ylabel "Memory Usage (kB)"
  show ylabel
  set title "Memory Usage vs. Samples Size With and Without Threading" 
#  pause -1;
  set out 'ThreadingMemory.ps'
  set terminal postscript color landscape enhanced "Helvetica" 14 
  replot
  set size 1,1

