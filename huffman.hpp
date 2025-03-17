#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>

using namespace std;

struct Node {
    int value;
    char c;
    Node* left;
    Node* right;
    bool isLeaf;

    Node(int value, char c, Node* left, Node* right, bool isLeaf);
};

struct Compare {
    bool operator()(Node* a, Node* b);
};

void getHuffmanCodes(Node* root, unordered_map<char, string>& huffmanCodes, string code = "");

string getStringByHuffmanCode(string& code, Node* huffmanTree);

Node* huffman(unordered_map<char, int> freqMap);

#endif