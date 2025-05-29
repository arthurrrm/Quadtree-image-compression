#ifndef QUADTREE_H
#define QUADTREE_H

#include <opencv2/opencv.hpp>
#include <iostream>

struct QuadtreeNode {
    int x, y;
    int width, height;
    cv::Vec3b value;
    bool isLeaf;
    QuadtreeNode* children[4];

    QuadtreeNode(int _x, int _y, int _width, int _height);
    ~QuadtreeNode();
};

// Function declarations
QuadtreeNode* buildQuadtree(const cv::Mat&, int, int, int, int, int);
void QuadtreeToImage(const QuadtreeNode*, cv::Mat&);
void colorizeNode(const QuadtreeNode*,const cv::Mat&);
void drawRectangle(const cv::Mat&, const QuadtreeNode* node);
void VisualizeQuadtree(const QuadtreeNode* node, cv::Mat& img);
void CountLeaves(const QuadtreeNode* node, int& count);
void SerializeQuadtreeOptimized(const QuadtreeNode* node, int width, int height, std::ostream& out);
QuadtreeNode* DeserializeQuadtreeOptimized(std::istream& in, int& width, int& height);
#endif
