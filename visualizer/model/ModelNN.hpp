#pragma once

#include <vector>
#include <string>
#include <fstream>
class ModelNN {

private:
    std::vector<double> input_normalized;

    std::vector<std::vector<double>> w1;
    std::vector<std::vector<double>> w2;
    std::vector<std::vector<double>> w3;

    std::vector<double> b1;
    std::vector<double> b2;
    std::vector<double> b3;

    std::vector<double> a1;
    std::vector<double> a2;
    std::vector<double> a3;
public:
    ModelNN(std::vector<double> input) : input_normalized(input) {

    }    

    bool loadModel(const std::string& path);

    bool loadVector(std::ifstream& in, std::vector<double>& vec);
    bool loadMatrix(std::ifstream& in, std::vector<std::vector<double>>& matrix);

    // argmax function that will go inside the inference function to find out node with highest value (double)
    // index of node with highest value will be the model's prediction 
    int argMax(const std::vector<double>& outputLayer);
    // will perform dot product and addition from weight matrices and bias vectors
    int inference();
    //activation function
    double sigmoid(double z);


    // flow of use:
        // client use: 1) loadModel()
        //             2) inference()
};