#include "Quadtree.hpp"


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
