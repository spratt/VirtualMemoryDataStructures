set ylabel "Time (ns)"
set boxwidth 0.9
set term aqua size 500,300
set size 1,1
set nokey
set style fill solid
plot "results.dat" using 1:3:xtic(2) with boxes
