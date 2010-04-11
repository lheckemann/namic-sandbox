  plot "AllThreadingMemoryResults.txt" using 9:13 with linespoints lw 3 title 'Sparse, Threaded', "AllDefaultMemoryResults.txt" using 9:13 with linespoints lw 3 title 'Default'
  set xlabel "Number of Grid Nodes"
  show xlabel
  set ylabel "Memory (kB)"
  show ylabel
  set title "Memory Usage vs. BSpline Grid Nodes"
#  pause -1;
  set out 'ThreadingMemory.ps'
  set terminal postscript color landscape enhanced "Helvetica" 14
  replot
  set size 1,1