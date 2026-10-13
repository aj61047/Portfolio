#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include "vector.hpp"
#include "matrix.hpp"
#include "output.hpp"
#include "variance.hpp"
#include "Ising_2D.hpp"
#include <cmath>

int main(){   
    // Check that magnetisation per spin and magnetic susceptibility per spin scale as expected in the critical region, between T=2 and T=T_critical=2.27

    double mu = 1;  // Magnetic moment
    double J = 1;   // Coupling constant
    double B = 0;   // External magnetic field
    double k = 1;   // Boltzmann constant
    

    // Vector of temperatures to simulate in the critical region
    std::vector<double> T_vec = {2, 2.03, 2.05, 2.07, 2.1, 2.13, 2.15, 2.17, 2, 2.2, 2.23, 2.25, 2.27};

    int L = 64;     // Number of spins in each row/column of the square lattice
    int Ns = L*L;   // Total number of spins in the lattice

    int burn_in = 100;              // Number of sweeps to discard for burn-in
    int n_sweeps = 10000+burn_in;   // Total number of sweeps to perform

    // Set up random number generator
    std::random_device rd;

    // Initialise the lattice
    Matrix lattice(L, L);
    lattice = initialise_lattice(lattice, L);   // Only initialise the lattice for the first (lowest) temperature
                                // For higher temps, we take the initial lattice to be the final lattice from the previous temp

    std::vector<double> E_av(T_vec.size());     // Average energy per spin
    std::vector<double> E_error(T_vec.size());  // Error in average energy per spin
    std::vector<double> M_av(T_vec.size());     // Average magnetisation per spin
    std::vector<double> M_error(T_vec.size());  // Error in average magnetisation per spin
    std::vector<double> M_theory(T_vec.size()); // Theoretical magnetisation per spin
    std::vector<double> C(T_vec.size());        // Specific heat per spin
    std::vector<double> X(T_vec.size());        // Magnetic susceptibility per spin
    std::vector<double> T_out(T_vec.size());    // Vector of temperatures to output to a text file, for plotting

    // Output initial state of lattice to the terminal
    output_lattice(lattice, L);

    // Loop over temperatures
    for(int l=0; l<T_vec.size(); l++){
        double T = T_vec.at(l);

        // Calculate the initial energy and magnetisation
        double E = -mu*B*sum(lattice, L, L);
        // Add the interaction term(s)
        for(int i=0; i<L; i++){
            for(int j=0; j<L; j++){
                E = E - J*lattice.at(i, j)*(lattice.at(i, (j+1)%L) + lattice.at((i+1)%L, j));
            }
        }

        //  Calculate initial magnetisation
        double M = sum(lattice, L, L);

        // Initialise vectors to store normalised energy and magnetisation at each sweep
        std::vector<double> E_vec(n_sweeps+1);     
        std::vector<double> M_vec(n_sweeps+1);

        // Initialise vector to store sweep number at each sweep
        std::vector<double> sweep_number(n_sweeps+1);

        // Store normalised energy and magnetisation at sweep 0
        E_vec.at(0) = E/(double)Ns;     
        M_vec.at(0) = M/(double)Ns;

        // Loop over sweeps
        for(int j=0; j<n_sweeps; j++){
            for(int i=0; i<Ns; i++){
                // Set up random number generators
                int seed = rd();
                std::mt19937 mt_generator(seed);
                std::uniform_real_distribution<double> uniform_rn1(0, 1);
                std::uniform_real_distribution<double> uniform_rn2(0, L-1);
                std::uniform_real_distribution<double> uniform_rn3(0, L-1);

                // Choose a spin to flip
                int flip_x = uniform_rn2(mt_generator);
                int flip_y = uniform_rn3(mt_generator);
            
                // Flip the spin, and calculate the change in energy
                lattice.at(flip_x, flip_y)=-lattice.at(flip_x, flip_y);
                double delta_E = delta_Energy(lattice.at(flip_x, flip_y), lattice.at((flip_x-1+L)%L, flip_y), \
                    lattice.at((flip_x+1)%L, flip_y), lattice.at(flip_x, (flip_y+1)%L), lattice.at(flip_x, (flip_y-1+L)%L),\
                    mu, J, B);

                
                // Either accept or reject the proposed flip, based on the Metropolis algorithm
                // If the change in energy is greater than zero, calculate the acceptance probability and compare it to a random number
                if(delta_E > 0){
                    // Generate uniform random number 0<=r<1
                    double r = uniform_rn1(mt_generator);

                    // Calculate the acceptance probability
                    double a = exp(-delta_E/(k*T));

                    // If the random number is greater than the acceptance probability, reject the flip and return the spin to its original state
                    if(r > a){
                        lattice.at(flip_x, flip_y)=-lattice.at(flip_x, flip_y);
                    }
                    // If the random number is less than or equal to the acceptance probability, accept the flip and update the energy and magnetisation
                    else{
                        E = E + delta_E;
                        double delta_M = delta_Magnetisation(lattice.at(flip_x, flip_y));
                        M = M + delta_M;
                    }
                }
                // If the change in energy is less than or equal to zero, accept the flip and update the energy and magnetisation
                else{
                    E = E + delta_E;
                    double delta_M = delta_Magnetisation(lattice.at(flip_x, flip_y));
                    M = M + delta_M;
                }
            }

            // Store the normalised energy and magnetisation at this sweep
            E_vec.at(j+1) = E/(double)Ns;
            M_vec.at(j+1) = std::abs(M)/(double)Ns;

            // Store the sweep number
            sweep_number.at(j+1) = j+1;
        }

        // Burn-in (discard the first burn_in sweeps)
        E_vec.erase(E_vec.begin(), E_vec.begin()+burn_in);
        M_vec.erase(M_vec.begin(), M_vec.begin()+burn_in);


        // Calculate averages

        // Average energy
        E_av.at(l) = vector_sum(E_vec)/((double)E_vec.size());

        // Energy error
        double E_variance = variance(E_vec, E_av.at(l));
        E_error.at(l) = sqrt(E_variance/((double)Ns));

        // Average magnetisation
        M_av.at(l) = vector_sum(M_vec)/((double)M_vec.size());

        // Magnetisation error
        double M_variance = variance(M_vec, M_av.at(l));
        M_error.at(l) = sqrt(M_variance/((double)Ns));

        // Theoretical/analytic magnetisation
        if(k*T/J < 2/log(1+sqrt(2))){
            M_theory.at(l) = pow(1-pow(sinh(2*J/(k*T)), -4), 1/8);
        }
        else{
            M_theory.at(l) = 0;
        }

        // Specific heat per spin
        C.at(l) = E_variance*Ns/(k*pow(T, 2));

        // Magnetic susceptibility per spin (log for plotting)
        X.at(l) = log(M_variance*Ns/(k*T));     

        // Rescale the magnetisation per spin for finite size scaling (log for plotting)
        M_av.at(l) = log(M_av.at(l));           

        // Rescale the temperature for finite size scaling (log for plotting)
        T_out.at(l) = log(std::abs(T-2.27));

        
        // Output final state of lattice to the terminal
        output_lattice(lattice, L);

    }

    // Output the rescaled magnetisation per spin and magnetic susceptibility per spin to a text file for plotting
    std::vector<std::vector<double>> Q3_output = {T_out, X, M_av};
    output(Q3_output, T_out.size(), Q3_output.size(), "a2_part2_2_Q3.txt");
}