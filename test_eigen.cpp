#include <iostream>
#include <Eigen/Dense>

int main() {
    // Cria uma matriz 2x2
    Eigen::Matrix2d mat;
    mat << 1, 2,
           3, 4;
    
    std::cout << "Matriz original:\n" << mat << std::endl;
    std::cout << "Determinante: " << mat.determinant() << std::endl;
    std::cout << "Transposta:\n" << mat.transpose() << std::endl;
    
    return 0;
}