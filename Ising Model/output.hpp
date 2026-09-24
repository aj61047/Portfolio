#pragma once

#include <vector>

// Function to output data to a text file
// output_data is a vector of vectors. It is assumed that the first vector of data will form the first column of the 
// text file, etc
// rows = length of each vector in output_data (ie the number of rows required in the output text file)
// cols = the number of vectors in output_data (ie the number of columns required in the output text file)
void output(std::vector<std::vector<double>> output_data, double rows, double cols, std::string output_name){
    std::ofstream output;
    output.open(output_name);

    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            output << output_data.at(j).at(i) << " ";
        }
        output << std::endl;
    }
    
    output.close();
}