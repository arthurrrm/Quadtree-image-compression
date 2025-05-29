#include <opencv2/opencv.hpp>
#include "Quadtree.hpp" 
#include <iostream>
#include <cstdlib> // For std::atoi

int main(int argc, char* argv[]) {    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path> [threshold]\n";
        return 1;
    }

    const char* image_path = argv[1];
    int threshold = 10; // Default threshold
    if (argc > 2) {
        threshold = std::atoi(argv[2]);
        if (threshold < 0) {
            std::cerr << "Error: Threshold must be non-negative.\n";
            return 1;
        }
    }

    cv::Mat img = cv::imread(image_path);
    if (img.empty()) {
        std::cerr << "Error: Cannot load image: " << image_path << "\n";
        return 1;
    }

    QuadtreeNode* root = buildQuadtree(img, 0, 0, img.cols, img.rows, threshold);
    if (root == nullptr) {
        std::cerr << "Error: Quadtree construction failed.\n";
        return 1;
    }

    // Draw the quadtree on the image
    cv::Mat output = img.clone();
    QuadtreeToImage(root, output);

    cv::namedWindow("Image", cv::WINDOW_NORMAL); // Enable resizing
    cv::imshow("Image", output);
    cv::resizeWindow("Image", 1000, 700);
    cv::moveWindow("Image", 400, 100);
    cv::waitKey(0);
    return 0;
}