# Ising Model Simulation

This project was completed as part of an assignment for a computational physics course in my Bachelor of Science. As such, some of the code was provided to all students by the teaching staff, specifically, the operator overload functions in vector.hpp, and the matrix class in matrix.hpp.

The aim of this project was to simulate the 2D Ising Model, which represents atomic spins (which can exist in either an up or down state) in a 2D lattice. 
As time progresses, some of these spins will flip, in a way that can be modelled by the Metropolis algorithm. 
The number of flips that occur per timestep depends on the size of the lattice and its initial temperature.
The simulation can thus be used to analyse the effect of lattice size and initial temperature on physical properties such as energy, magnetisation, critical heat, and magnetic susceptibility.

The report.pdf file explains the process used to create the simulations in more detail, as well as analysing the results.

single_lattice_sim.cpp simulates a 16x16 lattice over 1100 timesteps, at 20 different initial temperatures, and outputs the average energy, magnetisation, critical heat, and magnetic susceptibility at the end of the simulation.
four_lattice_sim.cpp performs the same simulation, but with four different lattice sizes: 8x8, 16x16, 32x32, and 64x64, using the same 20 initial temperatures.
critical_region_sim.cpp simulates a 64x64 lattice at 13 different temperatures around the critical temperature at which a phase transition is expected.

The four .hpp files are header files that contain functions required for the simulations.

plots.gnu is a Gnuplot file that plots the physical properties resulting from the simulations.

runfile.sh is a bash file that compiles and runs the three simulations, as well as running plots.gnu. 
Please note, if you wish to run this file, it will create three .txt files (one for each simulation), and 10 .png files (from plots.gnu).
