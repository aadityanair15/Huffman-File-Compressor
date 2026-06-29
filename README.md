# Huffman-File-Compressor

# C++ Lossless File Compressor (Huffman Coding)

A high-performance, lossless file compression and decompression engine built entirely from scratch in C++. This project implements the classic **Huffman Coding** algorithm to analyze file frequencies, construct optimal prefix-free binary trees, and compress data using low-level bitwise operations. 

## Performance Metrics
Tested on large plain-text datasets, this compression engine consistently achieves a **~42% reduction in file size**.

* **Original File:** `test.txt`
* **Output File:** `compressed.bin`
* **Compression Ratio:** ~42.1% space saved

## Technical Architecture

This utility doesn't just map strings to shorter strings; it performs genuine system-level binary packing. The architecture is split into three core phases:

### 1. Tree Generation (O(N log K))
* Utilizes a single pass over the file to build a frequency map (`std::unordered_map`).
* Feeds character frequencies into a Min-Heap (`std::priority_queue`) to dynamically construct an optimal binary prefix tree, ensuring the most common characters receive the shortest bit-codes.

### 2. Bitwise Packing 
* Standard characters take up 8 bits (1 byte). This compressor maps characters to variable-length bits (e.g., 'A' = `10`, 'E' = `011`).
* Uses low-level bitwise shifts (`<<`) and OR operations (`|`) to pack these variable-length codes into raw 8-bit bytes before writing them to the disk via `<fstream>`.

### 3. Custom File Header Protocol & Serialization
To ensure the completely blind decompressor can read the file, the compressor designs a custom binary `.bin` file format. The file header is constructed using **pre-order tree serialization**:
1. **Size Byte:** Reads the exact character length of the serialized tree.
2. **Delimiter:** A strict newline (`\n`) wall separating metadata from tree data.
3. **RAM Reconstruction:** The decompressor reads the exact length of the serialized string, uses the `new` keyword to dynamically allocate memory, and perfectly rebuilds the binary tree in RAM.
4. **Data Decoding:** With the "blueprint" rebuilt, the remaining raw bits are successfully decoded back into text.

## Skills & Concepts Demonstrated
* **Languages:** C++
* **Memory Management:** Pointers, dynamic allocation (`new`), avoiding memory leaks.
* **Data Structures:** Min-Heaps, Hash Maps, Binary Trees.
* **Systems Programming:** Raw binary File I/O, bitwise operators, custom file formatting.

## How to Run

1. Clone this repository to your local machine.
2. Ensure you have a C++ compiler installed (GCC/MinGW).
3. Place a sample text file named `test.txt` in the same directory.
4. Compile and run the code:
   ```bash
   g++ HuffmanFinal.cpp -o huffman
   ./huffman
