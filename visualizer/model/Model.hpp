#pragma once
#include <vector>
#include <string>
#include <fstream>
class Model {

private:
    std::vector<std::vector<double>> w1;
    std::vector<std::vector<double>> w2;
    std::vector<std::vector<double>> w3;

    std::vector<double> b1;
    std::vector<double> b2;
    std::vector<double> b3;
public:
    bool loadModel(const std::string& path);

    bool loadVector(std::ifstream& in, std::vector<double>& vec);
    bool loadMatrix(std::ifstream& in, std::vector<std::vector<double>>& matrix);

    int inference();
};