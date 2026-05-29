#include <cstdint>
#include <iostream> 
#include <string>
#include <sys/types.h>
#include <vector>
#include <fstream>
#include <algorithm>
#define REVERSE_BYTES(x) __builtin_bswap32(x)

constexpr int INPUT_SIZE = 784;
constexpr int HIDDEN_SIZE = 128;
constexpr int OUTPUT_SIZE = 10;

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
    // parse the payload
    uint8_t label;
    std::vector<u_int8_t> label_vector(count, 0);
    for (uint32_t i = 0; i < count; i++) {
        label_data.read(reinterpret_cast<char*>(&label), 1);
        label_vector[i] = label;
    }

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
        NUM_IMAGE, std::vector<uint8_t>(pixels_per_image, 0)
    );

    for (uint32_t image = 0; image < NUM_IMAGE; image++) {
        for (uint32_t row = 0; row < NUM_ROW; row++) {
            for (uint32_t col = 0; col < NUM_COL; col++) {
                image_data.read(reinterpret_cast<char*>(&pixel_value), 1);
                image_vector[image][row * NUM_COL + col] = pixel_value;
            }
        }
    }
    // start on the feedforward

    // layer 1
    std::vector<double> layer1(HIDDEN_SIZE, 0.1);
    std::vector<std::vector<double>> weight1(
        HIDDEN_SIZE, std::vector<double>(INPUT_SIZE, 0.1));
    std::vector<double> bias1(HIDDEN_SIZE, 0.1);

    // layer 2
    std::vector<double> output_layer(OUTPUT_SIZE, 0.1);
    std::vector<std::vector<double>> weight2(
        OUTPUT_SIZE, std::vector<double>(HIDDEN_SIZE, 0.01));
    std::vector<double> bias2(OUTPUT_SIZE, 0.1);


    // normalize the image vector to doubles
    // this only does one image
    std::vector<double> x(pixels_per_image);
    for (uint32_t i = 0; i < pixels_per_image; i++) {
        x[i] = static_cast<double>(image_vector[0][i]) / 255.0;
    }

    for (uint32_t i = 0; i < pixels_per_image; i++) {
        std::cout << x[i] << '\n';
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