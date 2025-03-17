#include "file_utils.hpp"

int main(int argc, char* argv[]) {
    char operation = argv[1][0]; 
    string inputFile = argv[2];
    string outputFile = argv[3];

    if (operation == 'C' || operation == 'c') {
        auto freqMap = calcFreq(inputFile);
        Node* root = huffman(freqMap);
        compactFile(inputFile, outputFile, root, freqMap);
    } else if (operation == 'D' || operation == 'd') {
        descompactFile(inputFile, outputFile);
    }
    
    return 0;
}
