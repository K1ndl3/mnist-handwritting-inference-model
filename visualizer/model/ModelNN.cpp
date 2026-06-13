#include "ModelNN.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

bool ModelNN::loadVector(std::ifstream& in, std::vector<double>& vec) {
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

bool ModelNN::loadMatrix(std::ifstream& in, std::vector<std::vector<double>>& matrix) {
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

bool ModelNN::loadModel(const std::string& path) {
    std::ifstream params_data(path);
    if (!params_data) {
        std::cerr << "Cannot open model file\n";
        return false;
    }

    if (!loadMatrix(params_data, w1) || !loadVector(params_data, b1) ||
        !loadMatrix(params_data, w2) || !loadVector(params_data, b2) ||
        !loadMatrix(params_data, w3) || !loadVector(params_data, b3)) {
        std::cerr << "Failed to load trained parameters\n";
        return false;
    }
    return true;
}


int ModelNN::argMax(const std::vector<double>& outputLayer) {
    int best = 0;
    for (int i = 1; i < outputLayer.size(); i++) {
        if (outputLayer[i] > outputLayer[best]) {
            best = i;
        }
    }
    return best;
}


int ModelNN::inference() {
    std::vector<double> a1(100,0.0);
    std::vector<double> a2(50, 0.0);
    std::vector<double> a3(10, 0);

    // loop through w1 w2 and w2 with b1 b2 b3 to find the layers
    // w1 has dimensions: 100x784 since input has dimensions: 784x1
    for (int row = 0; row < w1.size(); row++) {
        double w_sum = b1[row];
        for (int col = 0; col < w1[0].size(); col++) {
            w_sum += input_normalized[col] * w1[row][col];
        }
        a1[row] = sigmoid(w_sum);
    }
    // w2 has dimensions: 50x100 since a1 has dimensions: 100x1
    for (int row = 0; row < w2.size(); row++) {
        double w_sum = b2[row];
        for (int col = 0; col < w2[0].size(); col++) {
            w_sum += a1[col] * w2[row][col];
        }
        a2[row] = sigmoid(w_sum);
    }
    // w3 has dimensions: 10x50 since a2 has dimensions: 50x1 (no sigmoid on output layer)
    for (int row = 0; row < w3.size(); row++) {
        double w_sum = b3[row];
        for (int col = 0; col < w3[0].size(); col++) {
            w_sum += a2[col] * w3[row][col];
        }
        a3[row] = w_sum;
    }
    return argMax(a3);
}

double ModelNN::sigmoid(double z) {
    return (1) / (1 + std::exp(-z));
}

void ModelNN::setInput(const std::vector<double>& input) {
    input_normalized = input;
}