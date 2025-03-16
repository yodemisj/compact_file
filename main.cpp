#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>
#include <bitset>

using namespace std;

typedef struct Node {
    int value;
    char c;
    Node* left;
    Node* right;
    bool isLeaf;

    Node(int value, char c, Node* left, Node* right, bool isLeaf) {
        this->value = value;
        this->left = left;
        this->right = right;
        this->c = c;
        this->isLeaf = isLeaf;
    }
} Node;

struct Compare {
    bool operator()(Node* a, Node* b) {
        if (a->value == b->value) {
            return a->c > b->c; 
        }
        return a->value > b->value; 
    }
};

void getHuffmanCodes(Node* root, unordered_map<char, string>& huffmanCodes, string code = "") {
    if (root == nullptr) return;

    if(root->isLeaf) {
        huffmanCodes[root->c] = code;
        return;
    }

    getHuffmanCodes(root->left, huffmanCodes, code + "0");
    getHuffmanCodes(root->right, huffmanCodes, code + "1");
}

string getStringByHuffmanCode(string &code, Node* huffmanTree) {
    Node * current = huffmanTree;
    size_t codeSize = code.size();
    string result;
    int latestIdx=0;

    for (size_t i = 0; i < codeSize; i++) {
        char c = code[i];
        
        if(current->isLeaf) {
            result += current->c;
            latestIdx = i;
            current = huffmanTree;
        }
        
        if (c == '0') {
            if (current->left) current = current->left;
        } else {
            if (current->right) current = current->right;
        }
    }
    
    if(current->isLeaf) {
        code.erase(code.begin(), code.end());
        result += current->c;
    } else {
        code.erase(code.begin(), code.begin() + latestIdx);
    }

    return result;
}

Node* huffman(unordered_map<char, int> freqMap) {
    priority_queue<Node* , vector<Node*>, Compare> Q;
    for (const auto& pair : freqMap) {
        Node* node = new Node(pair.second, pair.first, nullptr, nullptr, true);
        Q.push(node);
    }
    for(int i = 1; i < freqMap.size(); i++) {
        Node* left = Q.top();
        Q.pop();
        Node* right = Q.top();
        Q.pop();
        char unique = right->c + left->c;
        Node* newNode = new Node(left->value + right->value, unique, left, right, false);
        Q.push(newNode);
    }
    return Q.top();
}

/* FILE */
unordered_map<char, int> calcFreq(const string path) {
    unordered_map<char, int> freqMap;
    char character;
    ifstream inputFile(path);
    inputFile >> noskipws;
    while (inputFile >> character) {
        freqMap[character]++;
    }
    
    inputFile.close();
    return freqMap;
}

void compactFile(const string sourcePath, const string destPath, Node* huffmanTree, unordered_map<char, int> freqMap) {
    unordered_map<char, string> huffmanCodes;
    ifstream inputFile(sourcePath);
    ofstream outputFile(destPath, ios::binary);
    unsigned char paddingBits = 0;
    char source;
    string buffer;

    outputFile.put(0);
    int mapSize = freqMap.size();
    outputFile.write(reinterpret_cast<char*>(&mapSize), sizeof(int));

    for (const auto& pair : freqMap) {
        cout << "char: '" <<pair.first << "'" << "count: '" << pair.second << "'"<< endl;
        outputFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(char));
        outputFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
    }

    getHuffmanCodes(huffmanTree, huffmanCodes);

    inputFile >> noskipws;

    while (inputFile >> source) {
        buffer += huffmanCodes[source];
       
        while (buffer.size() >= 8) {
            bitset<8> byte(buffer.substr(0, 8));
            unsigned char c = static_cast<unsigned char>(byte.to_ulong());
            outputFile.write(reinterpret_cast<char*>(&c), 1);
            buffer.erase(0, 8); 
        }
    }

    if (!buffer.empty()) {
        paddingBits = 8 - buffer.size();

        while (buffer.size() < 8) {
            buffer += "0";
        }
        bitset<8> lastByte(buffer);
        unsigned char c = static_cast<unsigned char>(lastByte.to_ulong());
        outputFile.write(reinterpret_cast<char*>(&c), 1);
    }

    outputFile.seekp(0);
    outputFile.put(paddingBits);

    inputFile.close();
    outputFile.close();
}

void descompactFile(const string sourcePath, const string destPath) {
    unordered_map<char, int> freqMap;
    ifstream inputFile(sourcePath, ios::binary);
    ofstream outputFile(destPath);
    char c, character;
    string stringByHuffman;
    vector<char> buffer;
    buffer.reserve(8);
    unsigned char paddingBits;
    int freq;
    int mapSize = 0;

    inputFile.read(reinterpret_cast<char*>(&paddingBits), 1);
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
    
    while (inputFile.read(&byte, 1)) {
        bitStream += bitset<8>(byte).to_string();
        
        if (inputFile.peek() == EOF && paddingBits > 0) {
            bitStream.erase(bitStream.end() - paddingBits, bitStream.end());
        }
        
        stringByHuffman = getStringByHuffmanCode(bitStream, root);
        
        outputFile.write(stringByHuffman.c_str(), stringByHuffman.size());
    }

    inputFile.close();
    outputFile.close();
}

/* FIM FILE */

int main (){
    unordered_map<char, int> freqMap = calcFreq("lorem_ipsum.txt");
    Node* root = huffman(freqMap);
    compactFile("lorem_ipsum.txt", "teste.huff", root, freqMap);
    descompactFile("teste.huff", "lorem_ipsum_Out.txt");
    return 0;
}