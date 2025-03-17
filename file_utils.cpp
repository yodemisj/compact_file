#include "file_utils.hpp"
#include <bitset>

unordered_map<char, int> calcFreq(const string& path) {
    unordered_map<char, int> freqMap;
    ifstream inputFile(path, ios::in);
    char character;
    inputFile >> noskipws;
    
    while (inputFile >> character) {
        freqMap[character]++;
    }

    inputFile.close();
    return freqMap;
}

void writeHeader(ofstream& outputFile, const unordered_map<char, int>& freqMap) {
    int mapSize = freqMap.size();
    outputFile.put(0);

    outputFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(int));

    for (const auto& pair : freqMap) {
        outputFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(char));
        outputFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
    }
}

void writeBitsToFile(ofstream& outputFile, string& buffer) {
    while (buffer.size() >= 8) {
        outputFile.put(static_cast<unsigned char>(bitset<8>(buffer.substr(0, 8)).to_ulong()));
        buffer.erase(0, 8);
    }
}

void processPadding(string& buffer, ofstream& outputFile) {
    unsigned char paddingBits = 0;
    if (!buffer.empty()) {
        paddingBits = 8 - buffer.size();
        buffer.append(paddingBits, '0');
        outputFile.put(static_cast<unsigned char>(bitset<8>(buffer).to_ulong()));
    }
    outputFile.seekp(0);
    outputFile.put(paddingBits);
}

void encodeAndWriteHuffmanBits(ifstream& inputFile, ofstream& outputFile, unordered_map<char, string>& huffmanCodes) {
    string buffer;
    char source;
    inputFile >> noskipws;
    while (inputFile >> source) {
        buffer += huffmanCodes[source];
        writeBitsToFile(outputFile, buffer);
    }

    processPadding(buffer, outputFile);
}

void compactFile(const string& sourcePath, const string& destPath, Node* huffmanTree, unordered_map<char, int> freqMap) {
    unordered_map<char, string> huffmanCodes;
    ifstream inputFile(sourcePath, ios::in);
    ofstream outputFile(destPath, ios::binary);

    writeHeader(outputFile, freqMap);

    getHuffmanCodes(huffmanTree, huffmanCodes);

    encodeAndWriteHuffmanBits(inputFile, outputFile, huffmanCodes);

    inputFile.close();
    outputFile.close();
}

void readFrequencyMap(ifstream& inputFile, unordered_map<char, int>& freqMap) {
    int freq, mapSize = 0;
    char c;

    inputFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

    for (int i = 0; i < mapSize; i++) {
        inputFile.read(&c, 1);
        inputFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        freqMap[c] = freq;
    }
}

void processBitStream(ifstream& inputFile, const unsigned char paddingBits, Node* root, ofstream& outputFile) {
    string bitStream;
    string stringByHuffman;
    char byte;
    
    while (inputFile.read(&byte, 1)) {
        bitStream += bitset<8>(byte).to_string();
        
        if (inputFile.peek() == EOF && paddingBits > 0) {
            bitStream.erase(bitStream.end() - paddingBits, bitStream.end());
        }
        
        stringByHuffman = getStringByHuffmanCode(bitStream, root);
        
        outputFile.write(stringByHuffman.c_str(), stringByHuffman.size());
    }
}

void descompactFile(const string& sourcePath, const string& destPath) {
    unordered_map<char, int> freqMap;
    ifstream inputFile(sourcePath, ios::binary);
    ofstream outputFile(destPath);

    unsigned char paddingBits;

    inputFile.read(reinterpret_cast<char*>(&paddingBits), 1);

    readFrequencyMap(inputFile, freqMap);

    Node* root = huffman(freqMap);

    processBitStream(inputFile, paddingBits, root, outputFile);

    inputFile.close();
    outputFile.close();
}
