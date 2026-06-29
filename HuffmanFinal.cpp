#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>


using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    
    Node(char c, int f, Node* l = nullptr, Node* r = nullptr) {
        ch = c; freq = f; left = l; right = r;
    }
};

struct comp {
    bool operator()(Node* l, Node* r) { return l->freq > r->freq; }
};

void getCodes(Node* root, string code, unordered_map<char, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) huffmanCode[root->ch] = code;
    getCodes(root->left, code + "0", huffmanCode);
    getCodes(root->right, code + "1", huffmanCode);
}

void serializeTree(Node* root, string& treeData) {
    if (!root) return;
    if (!root->left && !root->right) {
        treeData += '1'; treeData += root->ch;
    } else {
        treeData += '0';
        serializeTree(root->left, treeData);
        serializeTree(root->right, treeData);
    }
}

Node* deserializeTree(const string& treeData, int& index) {
    if (index >= (int)treeData.size()) return nullptr;

    if (treeData[index] == '1') {
        index++;                  
        char ch = treeData[index];
        index++;                  
        return new Node(ch, 0); 
    } else {
        index++;                  
        Node* leftChild = deserializeTree(treeData, index);
        Node* rightChild = deserializeTree(treeData, index);
        return new Node('\0', 0, leftChild, rightChild);
    }
}

void compressFile(const string& inFile, const string& outFile) {
    ifstream in(inFile, ios::binary);
    if (!in) { cout << "Error opening input file.\n"; return; }

    unordered_map<char, int> freq;
    char byte;
    while (in.get(byte)) freq[byte]++;
    
    in.clear(); in.seekg(0, ios::beg); 

    priority_queue<Node*, vector<Node*>, comp> pq;
    for (auto pair : freq) pq.push(new Node(pair.first, pair.second));

    while (pq.size() > 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();
        pq.push(new Node('\0', left->freq + right->freq, left, right));
    }
    Node* root = pq.top();

    unordered_map<char, string> huffmanCode;
    getCodes(root, "", huffmanCode);

    string treeHeader = "";
    serializeTree(root, treeHeader);

    ofstream out(outFile, ios::binary);
    
    out << treeHeader.size() << '\n';
    out << treeHeader;

    unsigned char buffer = 0;
    int bitCount = 0;

    while (in.get(byte)) {
        for (char bit : huffmanCode[byte]) {
            buffer = (buffer << 1) | (bit - '0'); 
            bitCount++;

            if (bitCount == 8) {
                out.put(buffer);
                buffer = 0; bitCount = 0;
            }
        }
    }

    if (bitCount > 0) {
        buffer = buffer << (8 - bitCount);
        out.put(buffer);
    }

    in.close(); out.close();
    cout << "Successfully compressed to " << outFile << "\n";
}

void decompressFile(const string& inFile, const string& outFile) {
    ifstream in(inFile, ios::binary);
    if (!in) { cout << "Error opening compressed file.\n"; return; }

    int headerSize;
    in >> headerSize; 
    in.get();         

    string treeHeader;
    treeHeader.resize(headerSize); 
    in.read(&treeHeader[0], headerSize);

    int index = 0;
    Node* root = deserializeTree(treeHeader, index);
    if (!root) { cout << "Error rebuilding tree.\n"; return; }

    ofstream out(outFile, ios::binary);
    Node* curr = root;
    char byte;

    while (in.get(byte)) {
        for (int i = 7; i >= 0; i--) {
            int bit = (byte >> i) & 1; 

            if (bit == 0) curr = curr->left;
            else curr = curr->right;

            if (!curr->left && !curr->right) {
                out.put(curr->ch); 
                curr = root;       
            }
        }
    }

    in.close(); out.close();
    cout << "Successfully decompressed to " << outFile << "\n";
}

int main() {
    string originalFile = "test.txt";
    string compressedFile = "compressed.bin";
    string decompressedFile = "restored_test.txt";

    cout << "Starting Huffman Process...\n\n";
    
    compressFile(originalFile, compressedFile);
    decompressFile(compressedFile, decompressedFile);

    // Get sizes using standard fstream (No <filesystem> required!)
    ifstream origStream(originalFile, ios::binary | ios::ate);
    double origSize = origStream.tellg();                      
    origStream.close();

    ifstream compStream(compressedFile, ios::binary | ios::ate);
    double compSize = compStream.tellg();
    compStream.close();

    double spaceSaved = ((origSize - compSize) / origSize) * 100.0;

    cout << "\n--- Compression Metrics ---\n";
    cout << "Original Size:   " << origSize << " bytes\n";
    cout << "Compressed Size: " << compSize << " bytes\n";
    cout << "Space Saved:     " << spaceSaved << "%\n";

    return 0;
}