#include <iostream> 
#include <string>
#include <vector>
#include <fstream>


bool validateFileOpen(const std::ifstream& input);

int main() {
    // data for training
    const std::string train_label = "./dataset/train-labels.idx1-ubyte";
    const std::string train_images = "./dataset/train-images.idx3-ubyte";

    std::ifstream label_data(train_label, std::ios::binary);
    std::ifstream train_data(train_images, std::ios::binary);

    if (!validateFileOpen(train_data) || !validateFileOpen(label_data)) {
        std::cerr << "Cannot open image/label files\n";
        return 1;
    }


    
    return 0;
}

bool validateFileOpen(const std::ifstream& input) {
    if (!input.is_open()) {
        return false;
    }
}