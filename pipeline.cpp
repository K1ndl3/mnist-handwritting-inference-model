#include <cstdint>
#include <iostream> 
#include <string>
#include <sys/types.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <random>
#define REVERSE_BYTES(x) __builtin_bswap32(x)

constexpr int INPUT_SIZE = 784;
constexpr int HIDDEN_SIZE = 128;
constexpr int OUTPUT_SIZE = 10;

std::vector<double>& randomizeVector(std::vector<double>& input, int layer);
bool validateFileOpen(const std::ifstream& input);
double relu(double input);

int main() {
    // data for training
    const std::string train_label = "./dataset/train-labels.idx1-ubyte";
    const std::string train_images = "./dataset/train-images.idx3-ubyte";
    
    std::ifstream label_data(train_label, std::ios::binary);
    std::ifstream image_data(train_images, std::ios::binary);

    if (!validateFileOpen(image_data) || !validateFileOpen(label_data)) {
        std::cerr << "Cannot open image/label files\n";
        return 1;
    }

    // parse header for label_data
    uint32_t magic;
    uint32_t count;
    label_data.read(reinterpret_cast<char*>(&magic), 4);
    label_data.read(reinterpret_cast<char*>(&count), 4);
    // header is in big endian, i need to it to be in little endian
    magic = REVERSE_BYTES(magic);
    count = REVERSE_BYTES(count);
    // parse the payload — only image[0]
    uint8_t label;
    std::vector<u_int8_t> label_vector(1, 0);
    label_data.read(reinterpret_cast<char*>(&label), 1);
    label_vector[0] = label;

    // parse the header for image_data
    uint32_t NUM_IMAGE;
    uint32_t NUM_ROW;
    uint32_t NUM_COL;

    image_data.read(reinterpret_cast<char*>(&magic),4);
    image_data.read(reinterpret_cast<char*>(&NUM_IMAGE),4);
    image_data.read(reinterpret_cast<char*>(&NUM_ROW),4);
    image_data.read(reinterpret_cast<char*>(&NUM_COL),4);
    
    NUM_IMAGE = REVERSE_BYTES(NUM_IMAGE);
    NUM_ROW = REVERSE_BYTES(NUM_ROW);
    NUM_COL = REVERSE_BYTES(NUM_COL);

    const uint32_t pixels_per_image = NUM_ROW * NUM_COL;
    uint8_t pixel_value;
    std::vector<std::vector<uint8_t>> image_vector(
        1, std::vector<uint8_t>(pixels_per_image, 0)
    );

    for (uint32_t row = 0; row < NUM_ROW; row++) {
        for (uint32_t col = 0; col < NUM_COL; col++) {
            image_data.read(reinterpret_cast<char*>(&pixel_value), 1);
            image_vector[0][row * NUM_COL + col] = pixel_value;
        }
    }
    // start on the feedforward

    // layer 1
    std::vector<double> layer1(HIDDEN_SIZE, 0.0);
    std::vector<std::vector<double>> weight1(
        HIDDEN_SIZE, std::vector<double>(INPUT_SIZE, 0.0));
    std::vector<double> bias1(HIDDEN_SIZE, 0.0);

    // layer 2
    std::vector<double> output_layer(OUTPUT_SIZE, 0.0);
    std::vector<std::vector<double>> weight2(
        OUTPUT_SIZE, std::vector<double>(HIDDEN_SIZE, 0.0));
    std::vector<double> bias2(OUTPUT_SIZE, 0.0);

    for (auto& row : weight1) {
        randomizeVector(row, 1);
    }
    for (auto& row : weight2) {
        randomizeVector(row, 2);
    }


    // normalize image[0] to doubles
    const uint32_t im = 0;
    std::vector<double> x(pixels_per_image);
    for (uint32_t i = 0; i < pixels_per_image; i++) {
        x[i] = static_cast<double>(image_vector[im][i]) / 255.0;
    }
    
    // z1 for backprop of the hidden layer
    std::vector<double> z1(HIDDEN_SIZE);
    // forward pass layer 1
    for (int row = 0; row < HIDDEN_SIZE; row++) {
        double weighted_sum =  bias1[row];
        for (int col = 0; col < INPUT_SIZE; col++) {
            weighted_sum += x[col] * weight1[row][col];
        }
        z1[row] = weighted_sum;
        layer1[row] = relu(weighted_sum);
    }
    // forward pass layer 2
    for (int row = 0; row < OUTPUT_SIZE; row++) {
        double weighted_sum = bias2[row];
        for (int col = 0; col < HIDDEN_SIZE; col++) {
            weighted_sum += layer1[col] * weight2[row][col];
        }
        output_layer[row] = weighted_sum;
    }


    // implement softmax
    std::vector<double> probs(10,0.0);
    uint8_t label_value = label_vector[im];

    // use max-logits trick to stop overflow
    double max_logits = output_layer[0];
    for (int k = 1; k < OUTPUT_SIZE; k++) {
        if (output_layer[k] > max_logits)
            max_logits = output_layer[k];
    }
    double sum_exponents = 0;
    for (int k = 0; k < OUTPUT_SIZE; k++) {
        probs[k] = std::exp(output_layer[k] - max_logits);
        sum_exponents += probs[k];
    }

    for (int k = 0; k < OUTPUT_SIZE; k++) {
        probs[k] = probs[k] / sum_exponents;
    }
    
    return 0;
}

bool validateFileOpen(const std::ifstream& input) {
    if (!input.is_open()) {
        return false;
    }
    return true;
}

double relu(double input) {
    return std::max(0.0,input);
}

std::vector<double>& randomizeVector(std::vector<double>& input, int layer) {
    double limit;
    if (layer == 1) {
        // He init: uniform on [-sqrt(2/fan_in), sqrt(2/fan_in)], fan_in = 784
        limit = std::sqrt(2.0 / INPUT_SIZE);
    } else if (layer == 2) {
        // He init: fan_in = 128
        limit = std::sqrt(2.0 / HIDDEN_SIZE);
    } else {
        std::cerr << "randomizeVector: layer must be 1 or 2\n";
        return input;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-limit, limit);

    for (auto& num : input) {
        num = dis(gen);
    }
    return input;
}
