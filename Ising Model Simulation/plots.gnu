set terminal png

# Plots from single_lattice_sim.cpp

set output "single_lattice_energy.png"
set xlabel "T"
set ylabel "Energy"
unset key
plot "Ising_2D_averages.txt" using 1:2:3 with errorbars

set output "single_lattice_magnetisation.png"
set xlabel "T"
set ylabel "Magnetisation"
set key
plot "Ising_2D_averages.txt" using 1:4:5 with errorbars title "Computational",\
     "Ising_2D_averages.txt" using 1:6 title "Analytic"

set output "single_lattice_specific_heat.png"
set xlabel "T"
set ylabel "Specific Heat"
unset key
plot "Ising_2D_averages.txt" using 1:7

set output "single_lattice_magnetic_susceptibility.png"
set xlabel "T"
set ylabel "Magnetic Susceptibility"
unset key
plot "Ising_2D_averages.txt" using 1:8


# Plots from four_lattice_sim.cpp

set output "four_lattice_specific_heat.png"
set xlabel "T"
set ylabel "Specific Heat"
set key
plot "specific_heat.txt" using 1:2 title "L=8", \
     "specific_heat.txt" using 1:3 title "L=16", \
     "specific_heat.txt" using 1:4 title "L=32", \
     "specific_heat.txt" using 1:5 title "L=64", \

set output "four_lattice_magnetic_susceptibility.png"
set xlabel "T"
set ylabel "Magnetic Susceptibility"
set key
plot "magnetic_susceptibility.txt" using 1:2 title "L=8", \
     "magnetic_susceptibility.txt" using 1:3 title "L=16", \
     "magnetic_susceptibility.txt" using 1:4 title "L=32", \
     "magnetic_susceptibility.txt" using 1:5 title "L=64", \


set output "four_lattice_specific_heat_rescaled.png"
set xlabel "L^(1/nu)t"
set ylabel "Rescaled Specific Heat"
set key
set xrange [-50:50]
plot "specific_heat_rescaled.txt" using 1:2 title "L=8", \
     "specific_heat_rescaled.txt" using 1:3 title "L=16", \
     "specific_heat_rescaled.txt" using 1:4 title "L=32", \
     "specific_heat_rescaled.txt" using 1:5 title "L=64", \

set output "four_lattice_magnetic_susceptibility_rescaled.png"
set xlabel "L^(1/nu)t"
set ylabel "Rescaled Magnetic Susceptibility"
set key
set xrange [-50:50]
plot "magnetic_susceptibility_rescaled.txt" using 1:2 title "L=8", \
     "magnetic_susceptibility_rescaled.txt" using 1:3 title "L=16", \
     "magnetic_susceptibility_rescaled.txt" using 1:4 title "L=32", \
     "magnetic_susceptibility_rescaled.txt" using 1:5 title "L=64", \


# Plots from critical_region_sim.cpp

set output "critical_region_magnetisation.png"
set xlabel "log|T - T_c|"
set ylabel "log(M)"
set autoscale xy
unset key
plot "critical_region.txt" using 1:3

set output "critical_region_magnetic_susceptibility.png"
set xlabel "log|T - T_c|"
set ylabel "log(X)"
unset key
plot "critical_region.txt" using 1:2