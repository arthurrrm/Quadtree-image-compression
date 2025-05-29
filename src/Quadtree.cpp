#include "Quadtree.hpp"
#include <iostream>
#include <stack>
#include <vector>
#include <cstdint>

QuadtreeNode::QuadtreeNode(int _x, int _y, int _width, int _height) : x(_x), y(_y), width(_width), height(_height), isLeaf(true) {
    for (int i = 0; i < 4; i++) {
        children[i] = nullptr;
    }
}

QuadtreeNode::~QuadtreeNode() {
    for (int i = 0; i < 4; i++) { 
        delete children[i];
    }
}

QuadtreeNode* buildQuadtree(const cv::Mat& img, int x, int y, int width, int height, int threshold) {
    //std::cout << "Building quadtree for node at (" << x << ", " << y << ") with width " << width << " and height " << height << std::endl;
    QuadtreeNode* node = new QuadtreeNode(x, y, width, height);
    cv::Mat block = img(cv::Rect(x, y, width, height));

    cv::Scalar mean, stddev;
    cv::meanStdDev(block, mean, stddev);
    node->value = cv::Vec3b(mean[0], mean[1], mean[2]);

    if ((stddev[0] > threshold || stddev[1] > threshold || stddev[2] > threshold) && (width > 1 && height > 1)) {
        node->isLeaf = false; 
        int halfWidth = width / 2;
        int halfHeight = height / 2;

        int offsetW = 0;
        int offsetH = 0;
        if (width%2 != 0)
            offsetW = 1;
        if (height%2 != 0)
            offsetH = 1;
        node->children[0] = buildQuadtree(img, x, y, halfWidth, halfHeight, threshold); // Top-left
        node->children[1] = buildQuadtree(img, x + halfWidth, y, halfWidth + offsetW, halfHeight, threshold); // Top-right
        node->children[2] = buildQuadtree(img, x, y + halfHeight, halfWidth, halfHeight + offsetH, threshold); // Bottom-left
        node->children[3] = buildQuadtree(img, x + halfWidth, y + halfHeight, halfWidth + offsetW, halfHeight + offsetH, threshold); // Bottom-right
    }
    return node;
}

void colorizeNode(const QuadtreeNode* node,const cv::Mat& img) {
        cv::rectangle(img, cv::Point(node->x, node->y), cv::Point(node->x + node->width -1, node->y + node->height -1), cv::Scalar(node->value[0], node->value[1], node->value[2]), -1);
}

void QuadtreeToImage(const QuadtreeNode* node, cv::Mat& img) {
    if (node->isLeaf) {
        colorizeNode(node, img);
    } else {
        for (int i = 0; i < 4; i++) {
            if (node->children[i]) {
                QuadtreeToImage(node->children[i], img);
            }
        }
    }
}

void drawRectangle(const cv::Mat& img, const QuadtreeNode* node) { 
        cv::rectangle(img, cv::Point(node->x, node->y), cv::Point(node->x + node->width, node->y + node->height), cv::Scalar(0, 0, 0), 1);
}

void VisualizeQuadtree(const QuadtreeNode* node, cv::Mat& img) {
    if (node->isLeaf) {
        drawRectangle(img, node);
    } else if (node->width >= 8 && node->height >= 8) {
        for (int i = 0; i < 4; i++) {
            if (node->children[i]) {
                VisualizeQuadtree(node->children[i], img);
            }
        }
    }
    else {
        drawRectangle(img, node);
    }
}

void CountLeaves(const QuadtreeNode* node, int& count) {
    if (node->isLeaf) {
        count++;
    } else {
        for (int i = 0; i < 4; i++) {
            if (node->children[i]) {
                CountLeaves(node->children[i], count);
            }
        }
    }
}

// Helper for writing bits
class BitWriter {
    std::vector<uint8_t> buffer;
    uint8_t current_byte = 0;
    int bit_pos = 0;
public:
    void writeBit(bool bit) {
        if (bit) current_byte |= (1 << bit_pos);
        bit_pos++;
        if (bit_pos == 8) {
            buffer.push_back(current_byte);
            current_byte = 0;
            bit_pos = 0;
        }
    }
    void flush() {
        if (bit_pos > 0) buffer.push_back(current_byte);
        current_byte = 0;
        bit_pos = 0;
    }
    void writeToStream(std::ostream& out) {
        flush();
        size_t sz = buffer.size();
        out.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
        out.write(reinterpret_cast<const char*>(buffer.data()), sz);
    }
};

// Helper for reading bits
class BitReader {
    std::vector<uint8_t> buffer;
    size_t byte_pos = 0;
    int bit_pos = 0;
public:
    BitReader(std::istream& in) {
        size_t sz;
        in.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        buffer.resize(sz);
        in.read(reinterpret_cast<char*>(buffer.data()), sz);
    }
    bool readBit() {
        if (byte_pos >= buffer.size()) return false;
        bool bit = (buffer[byte_pos] >> bit_pos) & 1;
        bit_pos++;
        if (bit_pos == 8) {
            bit_pos = 0;
            byte_pos++;
        }
        return bit;
    }
};

// Recursive helper for serialization
void serializeQuadtreeBits(const QuadtreeNode* node, BitWriter& bw, std::vector<cv::Vec3b>& colors) {
    if (!node) return;
    bw.writeBit(node->isLeaf);
    if (node->isLeaf) {
        colors.push_back(node->value);
    } else {
        for (int i = 0; i < 4; ++i) {
            serializeQuadtreeBits(node->children[i], bw, colors);
        }
    }
}

void SerializeQuadtreeOptimized(const QuadtreeNode* node, int width, int height, std::ostream& out) {
    // Write image size
    out.write(reinterpret_cast<const char*>(&width), sizeof(width));
    out.write(reinterpret_cast<const char*>(&height), sizeof(height));
    // Write structure bits and collect colors
    BitWriter bw;
    std::vector<cv::Vec3b> colors;
    serializeQuadtreeBits(node, bw, colors);
    bw.writeToStream(out);
    // Write colors
    for (const auto& color : colors) {
        out.write(reinterpret_cast<const char*>(&color), sizeof(color));
    }
}

// Recursive helper for deserialization
QuadtreeNode* deserializeQuadtreeBits(BitReader& br, std::istream& in, int x, int y, int width, int height) {
    bool isLeaf = br.readBit();
    QuadtreeNode* node = new QuadtreeNode(x, y, width, height);
    node->isLeaf = isLeaf;
    if (isLeaf) {
        in.read(reinterpret_cast<char*>(&node->value), sizeof(node->value));
    } else {
        int halfWidth = width / 2;
        int halfHeight = height / 2;
        int offsetW = (width % 2 != 0) ? 1 : 0;
        int offsetH = (height % 2 != 0) ? 1 : 0;
        node->children[0] = deserializeQuadtreeBits(br, in, x, y, halfWidth, halfHeight);
        node->children[1] = deserializeQuadtreeBits(br, in, x + halfWidth, y, halfWidth + offsetW, halfHeight);
        node->children[2] = deserializeQuadtreeBits(br, in, x, y + halfHeight, halfWidth, halfHeight + offsetH);
        node->children[3] = deserializeQuadtreeBits(br, in, x + halfWidth, y + halfHeight, halfWidth + offsetW, halfHeight + offsetH);
    }
    return node;
}

QuadtreeNode* DeserializeQuadtreeOptimized(std::istream& in, int& width, int& height) {
    // Read image size
    in.read(reinterpret_cast<char*>(&width), sizeof(width));
    in.read(reinterpret_cast<char*>(&height), sizeof(height));
    BitReader br(in);
    return deserializeQuadtreeBits(br, in, 0, 0, width, height);
}
