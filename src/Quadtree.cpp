#include "Quadtree.hpp"


#define DEBUG 0


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
    QuadtreeNode* node = new QuadtreeNode(x, y, width, height);
    cv::Mat block = img(cv::Rect(x, y, width, height));

    cv::Scalar mean, stddev;
    cv::meanStdDev(block, mean, stddev);
    node->value = cv::Vec3b(mean[0], mean[1], mean[2]);

    if ((stddev[0] > threshold || stddev[1] > threshold || stddev[2] > threshold) && (width > 1 && height > 1)) {

        node->isLeaf = false;
        int halfWidth = width / 2;
        int halfHeight = height / 2;

        node->children[0] = buildQuadtree(img, x, y, halfWidth, halfHeight, threshold); // Top-left
        node->children[1] = buildQuadtree(img, x + halfWidth, y, halfWidth, halfHeight, threshold); // Top-right
        node->children[2] = buildQuadtree(img, x, y + halfHeight, halfWidth, halfHeight, threshold); // Bottom-left
        node->children[3] = buildQuadtree(img, x + halfWidth, y + halfHeight, halfWidth, halfHeight, threshold); // Bottom-right
    }
    else {
        if (DEBUG) drawRectangle(img, node);  // Colorize the node if it's a leaf node
    }
    return node;
}


void drawRectangle(const cv::Mat& img, const QuadtreeNode* node) {  // Only draw rectangles larger than 10x10 pixels
        cv::rectangle(img, cv::Point(node->x, node->y), cv::Point(node->x + node->width, node->y + node->height), cv::Scalar(0, 0, 0), 1);
}

void colorizeNode(const QuadtreeNode* node,const cv::Mat& img) {
        cv::rectangle(img, cv::Point(node->x, node->y), cv::Point(node->x + node->width, node->y + node->height), cv::Scalar(node->value[0], node->value[1], node->value[2]), -1);
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
