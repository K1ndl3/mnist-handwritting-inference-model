#include "Model.hpp"

bool loadVector(std::ifstream& in, std::vector<double>& vec) {
    int n = 0;
    if (!(in >> n)) {
        return false;
    }

    vec.resize(n);
    for (int i = 0; i < n; i++) {
        if (!(in >> vec[i])) {
            return false;
        }
    }
    return true;
}

bool loadMatrix(std::ifstream& in, std::vector<std::vector<double>>& matrix) {
    int rows = 0;
    int cols = 0;
    if (!(in >> rows >> cols)) {
        return false;
    }

    matrix.assign(rows, std::vector<double>(cols));
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (!(in >> matrix[r][c])) {
                return false;
            }
        }
    }
    return true;
}