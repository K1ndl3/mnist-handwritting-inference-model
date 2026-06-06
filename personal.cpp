#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#define REVERSE_BYTES(x) __builtin_bswap32(x)

constexpr int INPUT_SIZE = 784;
constexpr int NUM_EPOCH = 100;

bool validateFileOpen(const std::ifstream& input);

void randomizeVector(std::vector<double>& input);
void randomizedMatrix(std::vector<std::vector<double>>& input);

double sigmoid(double z);

std::vector<double> gradientCalc(std::vector<double>& a, double label);
std::vector<double> forwardPass(std::vector<double> layer,
                                std::vector<std::vector<double>> weight, std::vector<double> bias,bool final = false);

int main() {
    const std::string train_label = "./dataset/train-labels.idx1-ubyte";
    const std::string train_images = "./dataset/train-images.idx3-ubyte";

    std::ifstream label_data(train_label, std::ios::binary);
    std::ifstream image_data(train_images, std::ios::binary);

    if (!validateFileOpen(image_data) || !validateFileOpen(label_data)) {
        std::cerr << "Cannot open image/label files\n";
        return 1;
    }

    // parse label header
    uint32_t magic;
    uint32_t count;
    label_data.read(reinterpret_cast<char*>(&magic), 4);
    label_data.read(reinterpret_cast<char*>(&count), 4);
    magic = REVERSE_BYTES(magic);
    count = REVERSE_BYTES(count);

    std::vector<uint8_t> label_vector(count, 0);
    for (uint32_t i = 0; i < count; i++) {
        uint8_t label;
        label_data.read(reinterpret_cast<char*>(&label), 1);
        label_vector[i] = label;
    }

    // parse image header
    uint32_t num_images;
    uint32_t num_rows;
    uint32_t num_cols;

    image_data.read(reinterpret_cast<char*>(&magic), 4);
    image_data.read(reinterpret_cast<char*>(&num_images), 4);
    image_data.read(reinterpret_cast<char*>(&num_rows), 4);
    image_data.read(reinterpret_cast<char*>(&num_cols), 4);

    num_images = REVERSE_BYTES(num_images);
    num_rows = REVERSE_BYTES(num_rows);
    num_cols = REVERSE_BYTES(num_cols);

    const uint32_t pixels_per_image = num_rows * num_cols;

    std::vector<std::vector<uint8_t>> image_vector(
        num_images, std::vector<uint8_t>(pixels_per_image, 0));

    uint8_t pixel_value;
    for (uint32_t image = 0; image < num_images; image++) {
        for (uint32_t row = 0; row < num_rows; row++) {
            for (uint32_t col = 0; col < num_cols; col++) {
                image_data.read(reinterpret_cast<char*>(&pixel_value), 1);
                image_vector[image][row * num_cols + col] = pixel_value;
            }
        }
    }

    // x[image][pixel] — normalized inputs for the neural network
    std::vector<std::vector<double>> x(
        num_images, std::vector<double>(pixels_per_image, 0.0));

    for (uint32_t im = 0; im < num_images; im++) {
        for (uint32_t i = 0; i < pixels_per_image; i++) {
            x[im][i] = static_cast<double>(image_vector[im][i]) / 255.0;
        }
    }

    // neural network code goes here
    // x[im] is one flattened 28x28 image (784 values in [0, 1])
    // label_vector[im] is the digit 0-9 for that image


    std::vector<double> a1(100, 0.0);
    std::vector<std::vector<double>> w1(100, std::vector<double>(784, 0.0));
    std::vector<double> b1(100,0.0);

    std::vector<double> a2(50, 0.0);
    std::vector<std::vector<double>> w2(50, std::vector<double>(100, 0.0));
    std::vector<double> b2(50,0.0);

    std::vector<double> a3(10, 0.0);
    std::vector<std::vector<double>> w3(10, std::vector<double>(50, 0.0));
    std::vector<double> b3(10,0.0);
    bool final_layer = true;


    randomizedMatrix(w1);
    randomizeVector(b1);
    
    randomizedMatrix(w2);
    randomizeVector(b2);

    randomizedMatrix(w3);
    randomizeVector(b3);

    for (int epoch = 0; epoch < NUM_EPOCH; epoch++) {
        for (int im = 0; im < 60000; im++) {
            // layer1
            a1 = forwardPass(x[im], w1, b1);
            // layer2
            a2 = forwardPass(a1, w2, b2);
            // output
            a3 = forwardPass(a2, w3, b3, final_layer);

        }
    }

    return 0;
}

bool validateFileOpen(const std::ifstream& input) {
    return input.is_open();
}

void randomizeVector(std::vector<double>& input) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    for (auto& num : input) {
        num = dis(gen);
    }
}

void randomizedMatrix(std::vector<std::vector<double>>& input) {
    for (auto& row : input) {
        randomizeVector(row);
    }
}

std::vector<double> forwardPass(std::vector<double> layer,
    std::vector<std::vector<double>> weight, std::vector<double> bias, bool final) {

        std::vector<double> h1(weight.size(),0.0);
        for(int row = 0; row < weight.size(); row++) {
            double w_sum = 0.0;
            for(int col = 0; col < weight[0].size(); col++) {
                w_sum += weight[row][col] * layer[col];
            }
            double z = w_sum + bias[row];
            if (!final) {
                h1[row] = sigmoid(z);
            } else {
                h1[row] = z;
            }
        }
        return h1;
    }

double sigmoid(double z) {
    return 1 / (1 + std::exp(-z));
}

