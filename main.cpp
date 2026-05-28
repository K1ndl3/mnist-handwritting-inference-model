#include <cstdint>
#include <iostream> 
#include <string>
#include <sys/types.h>
#include <vector>
#include <fstream>
#define REVERSE_BYTES(x) __builtin_bswap32(x)

bool validateFileOpen(const std::ifstream& input);

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

    u_int8_t pixel_value;
    std::vector<std::vector<std::vector<uint8_t>>> image_vector(
        NUM_IMAGE, std::vector<std::vector<uint8_t>> (
            NUM_ROW, std::vector<uint8_t> (NUM_COL,0)
        )
    );

    for (int image = 0; image < NUM_IMAGE; image++) {
        for (int row = 0; row < NUM_ROW; row++) {
            for (int col = 0; col < NUM_COL; col++) {
                image_data.read(reinterpret_cast<char*>(&pixel_value), 1);
                image_vector[image][row][col] = pixel_value;
            }
        }
    }

    return 0;
}

bool validateFileOpen(const std::ifstream& input) {
    if (!input.is_open()) {
        return false;
    }
    return true;
}