# set term pngcairo background "#171111" size 1200,900 font 'Input Mono'
set term png background "#171111" size 1200,900 font 'Input Mono'

########################################
# line styles for ColorBrewer Set3
# author: Anna Schneider
set linetype 1 lc rgb '#8DD3C7' pt 0 # teal
set linetype 2 lc rgb '#FFFFB3' pt 5 # banana
set linetype 3 lc rgb '#BEBADA' pt 7 # lilac
set linetype 4 lc rgb '#FB8072' pt 9 # red
set linetype 5 lc rgb '#80B1D3' pt 11 # steel blue
set linetype 6 lc rgb '#FDB462' pt 13 # adobe orange
set linetype 7 lc rgb '#B3DE69' pt 15 # lime green
set linetype 8 lc rgb '#FCCDE5' pt 3 # mauve
########################################

set style line 11 lt 1 lw 1 lc rgbcolor "#AA9999"
set style line 12 lt 1 lw 1 lc rgbcolor "#443333"

set key top left textcolor linestyle 11
set grid ls 12

set border linewidth 0 ls 12
set ytics textcolor linestyle 11
set xtics textcolor linestyle 11 rotate
set xlabel tc ls 11 offset 0,-3
unset xzeroaxis

set style data linespoints
set datafile separator ","
set key autotitle columnhead

set xrange[0:1024]
set yrange [0:]
plot filename using 1
