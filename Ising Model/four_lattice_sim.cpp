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
    // Simulates a 2D Ising model on a square lattice, using the Metropolis algorithm to perform Monte Carlo simulations of the system
    // at different temperatures and for different lattice sizes to check that the magnetisation per spin and magnetic susceptibility per spin obey finite-size scaling laws

    double mu = 1;  // Magnetic moment
    double J = 1;   // Coupling constant
    double B = 0;   // External magnetic field
    double k = 1;   // Boltzmann constant

    // Vector of temperatures to simulate
    std::vector<double> T_vec = {0.1, 0.3, 0.5, 0.7, 1, 1.3, 1.5, 1.7, 2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.3, 3.5, 3.7, 4, 4.3, 4.5, 4.7};
   
    // Vector to store the rescaled temperature for finite size scaling
    std::vector<double> t_vec(T_vec.size());

    double gamma = 7.0/4.0;     // Critical exponent for magnetic susceptibility
    double nu = 1.0;            // Critical exponent for correlation length

    // Vector of lattice sizes to simulate
    std::vector<int> L_vec = {8, 16, 32, 64};

    // Vectors to store the output data for specific heat and magnetic susceptibility
    std::vector<std::vector<double>> C_output(L_vec.size()+1);
    std::vector<std::vector<double>> X_output(L_vec.size()+1);

    // Vectors to store the rescaled specific heat and magnetic susceptibility
    std::vector<std::vector<double>> C_rescaled(L_vec.size()+1);
    std::vector<std::vector<double>> X_rescaled(L_vec.size()+1);

    // Store the temperature vector in the first row of the output vectors
    C_output.at(0) = T_vec;
    X_output.at(0) = T_vec;

    // Loop over grid sizes
    for(int a=0; a<L_vec.size(); a++){

        int L = L_vec.at(a);     // Number of spins in each row/column of the square lattice
        int Ns = L*L;            // Total number of spins in the lattice

        // Rescale the temperature for finite size scaling
        for(int i=0; i<T_vec.size(); i++){
            t_vec.at(i) = pow(L, 1/nu)*(T_vec.at(i) - 2.27)/T_vec.at(i);
        }

        // Store the rescaled temperature vector in the first row of the rescaled output vectors
        C_rescaled.at(0) = t_vec;
        X_rescaled.at(0) = t_vec;

        // Set the burn-in and number of sweeps for the simulation
        int burn_in = 100;
        int n_sweeps = 1000+burn_in;

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

        // Output initial state of lattice to the terminal
        output_lattice(lattice, L);

        // Loop over temperatures
        for(int l=0; l<T_vec.size(); l++){
            double T = T_vec.at(l);

            // Calculate the initial energy
            double E = -mu*B*sum(lattice, L, L);
            // Add the interaction term(s)
            for(int i=0; i<L; i++){
                for(int j=0; j<L; j++){
                    E = E - J*lattice.at(i, j)*(lattice.at(i, (j+1)%L) + lattice.at((i+1)%L, j));
                }
            }

            // Calculate initial magnetisation
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
                    int flip_x = uniform_rn2(mt_generator);     // Row index of the spin to flip
                    int flip_y = uniform_rn3(mt_generator);     // Column index of the spin to flip
                
                    // Flip the spin
                    lattice.at(flip_x, flip_y)=-lattice.at(flip_x, flip_y);

                    // Calculate the change in energy due to the flip
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

            // Magnetic susceptibility per spin
            X.at(l) = M_variance*Ns/(k*T);     // log for plotting

            
            // Output final state of lattice to the terminal
            output_lattice(lattice, L);

        }

        // Store the specific heat and magnetic susceptibility for this lattice size in the output vectors
        C_output.at(a+1) = C;
        X_output.at(a+1) = X;

        // Rescale the specific heat and magnetic susceptibility for finite size scaling
        std::vector<double> C_new(C.size());
        std::vector<double> X_new(X.size());
        for(int b=0; b<C.size(); b++){
            C_new.at(b) = C.at(b)/(log(L));
            X_new.at(b) = X.at(b)/(pow(L, gamma/nu));
        }
        C_rescaled.at(a+1) = C_new;
        X_rescaled.at(a+1) = X_new;
    }

    // Output the specific heat and magnetic susceptibility for each lattice size to text files
    output(C_output, T_vec.size(), C_output.size(), "specific_heat.txt");
    output(X_output, T_vec.size(), X_output.size(), "magnetic_susceptibility.txt");

    // Output the rescaled specific heat and magnetic susceptibility for each lattice size to text files
    output(C_rescaled, T_vec.size(), C_rescaled.size(), "specific_heat_rescaled.txt");
    output(X_rescaled, T_vec.size(), X_rescaled.size(), "magnetic_susceptibility_rescaled.txt");
}
