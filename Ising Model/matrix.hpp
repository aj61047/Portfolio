#pragma once

#include <vector>

class Matrix{
    private:
        int rows;
        int cols;
        std::vector<double> v;

    public:
        Matrix(int in_rows, int in_cols) : rows(in_rows), cols(in_cols), v(in_rows*in_cols){
        }

        // Matrix(int in_rows, int in_cols){
        //     rows=in_rows;
        //     cols=in_cols;
        //     v.resize(in_rows*in_cols);
        // }

        double* data(){ 
            return v.data(); 
        }

        double& at(int i, int j){ 
            return v.at(i*cols + j); 
        }

        int N_rows(){
            return rows;
        }

        int N_cols(){
            return cols;
        }

        // Function to extract a single row from a matrix
        std::vector<double> row(int i){
            std::vector<double> a(cols);
            for(int j=0; j<cols; j++){
                a.at(j)=v.at(i*cols+j);
            }
            return a;
        }
};

Matrix operator+(Matrix M1, Matrix M2){
    int rows = M1.N_rows(); // should be the same as M2.N
    int cols = M1.N_cols();
    Matrix M_result(rows, cols);
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            M_result.at(i,j)=M1.at(i,j)+M2.at(i,j);
        }
    }
    return M_result;
}

Matrix operator*(Matrix matrixA, Matrix matrixB){
    int rows = matrixA.N_rows();
    int cols = matrixB.N_cols();
    Matrix result(rows, cols);

    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            for(int k=0; k<rows; k++){
                result.at(i,j) += matrixA.at(i,k)*matrixB.at(k,j);
            }
        }
    }
    return result;
}

// Function to perform elementwise multiplication between two matrices
Matrix multiply_elementwise(Matrix A, Matrix B){
    // Need A and B to have the same dimensions
    int rows = A.N_rows();
    int cols = A.N_cols();

    Matrix result(rows, cols);

    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result.at(i,j)=A.at(i,j)*B.at(i,j);
        }
    }

    return result;
}
