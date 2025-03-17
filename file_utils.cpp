#include "file_utils.hpp"
#include <bitset>

unordered_map<char, int> calcFreq(const string& path) {
    unordered_map<char, int> freqMap;
    ifstream inputFile(path, ios::in);
    char character;

    while (inputFile.get(character)) {
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

void compactFile(const string& sourcePath, const string& destPath, Node* huffmanTree, unordered_map<char, int> freqMap) {
    unordered_map<char, string> huffmanCodes;
    ifstream inputFile(sourcePath, ios::in);
    ofstream outputFile(destPath, ios::binary);
    string buffer;

    writeHeader(outputFile, freqMap);

    getHuffmanCodes(huffmanTree, huffmanCodes);

    char source;
    while (inputFile.get(source)) {
        buffer += huffmanCodes[source];

        writeBitsToFile(outputFile, buffer);
    }

    processPadding(buffer, outputFile);

    inputFile.close();
    outputFile.close();
}

void descompactFile(const string& sourcePath, const string& destPath) {
    ifstream inputFile(sourcePath, ios::binary);
    ofstream outputFile(destPath);

    unsigned char paddingBits;
    inputFile.read(reinterpret_cast<char*>(&paddingBits), 1);

    unordered_map<char, int> freqMap;
    int mapSize;
    inputFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

    for (int i = 0; i < mapSize; i++) {
        char c;
        int freq;
        inputFile.read(&c, 1);
        inputFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        freqMap[c] = freq;
    }

    Node* root = huffman(freqMap);

    string bitStream;
    char byte;
    while (inputFile.get(byte)) {
        bitStream += bitset<8>(byte).to_string();
    }

    outputFile << getStringByHuffmanCode(bitStream, root);
    inputFile.close();
    outputFile.close();
}
