#include <opencv2/opencv.hpp>
#include "Quadtree.hpp" 
#include <iostream>
#include <cstdlib> // For std::atoi
#include <fstream>

#define DEBUG 0

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n  " << argv[0] << " --compress <image_path> [threshold]\n  " << argv[0] << " --load <qtc_path>\n";
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "--compress") {
        std::string image_path = argv[2];
        int threshold = 10;
        if (argc > 3) {
            threshold = std::atoi(argv[3]);
            if (threshold < 0) {
                std::cerr << "Error: Threshold must be non-negative.\n";
                return 1;
            }
        }

        // Apply the quadtree compression
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
        
        // Compose .qtc file path in the same directory as the image
        std::string qtc_path = image_path;
        size_t last_dot = qtc_path.find_last_of('.');
        if (last_dot != std::string::npos) 
            qtc_path = qtc_path.substr(0, last_dot);
        qtc_path += ".qtc";
        std::ofstream out(qtc_path, std::ios::binary);
        if (!out) {
            std::cerr << "Error: Cannot open file for writing: " << qtc_path << std::endl;
            return 1;
        }
        SerializeQuadtreeOptimized(root, img.cols, img.rows, out);
        out.close();
        std::cout << "Compressed quadtree saved to: " << qtc_path << std::endl;

        // Count the number of rectangles in the quadtree
        int count = 0;
        CountLeaves(root, count);
        std::cout << "Number of rectangles: " << count << std::endl;

        // Convert the quadtree to an OpenCV image
        cv::Mat output = cv::Mat::zeros(img.size(), CV_8UC3);
        QuadtreeToImage(root, output);
        if (DEBUG) 
            VisualizeQuadtree(root, output);
        

        // Save the output image as PNG with '_compressed' in the filename
        std::string output_path = image_path;
        size_t dot_pos = output_path.find_last_of('.');
        if (dot_pos != std::string::npos)
            output_path = output_path.substr(0, dot_pos) + "_compressed" + output_path.substr(dot_pos);
        else
            output_path += "_compressed.png";
        if (!cv::imwrite(output_path, output))
            std::cerr << "Error: Could not save the output image to " << output_path << std::endl;
        else
            std::cout << "Compressed image saved to: " << output_path << std::endl;

        // Show the result
        cv::namedWindow("Image", cv::WINDOW_NORMAL);
        cv::imshow("Image", output);
        cv::resizeWindow("Image", 1000, 700);
        cv::moveWindow("Image", 400, 100);
        cv::waitKey(0);
        return 0;


    } else if (mode == "--load") {
        std::string qtc_path = argv[2];
        std::ifstream in(qtc_path, std::ios::binary);
        if (!in) {
            std::cerr << "Error: Cannot open compressed file: " << qtc_path << std::endl;
            return 1;
        }
        int width = 0, height = 0;
        QuadtreeNode* root = DeserializeQuadtreeOptimized(in, width, height);
        in.close();
        if (!root) {
            std::cerr << "Error: Failed to load quadtree from file." << std::endl;
            return 1;
        }
        cv::Mat output(height, width, CV_8UC3, cv::Scalar(0,0,0));
        QuadtreeToImage(root, output);
        cv::namedWindow("Image", cv::WINDOW_NORMAL);
        cv::imshow("Image", output);
        cv::resizeWindow("Image", 1000, 700);
        cv::moveWindow("Image", 400, 100);
        cv::waitKey(0);
        return 0;
    } else {
        std::cerr << "Unknown option: " << mode << std::endl;
        std::cerr << "Usage:\n  " << argv[0] << " --compress <image_path> [threshold]\n  " << argv[0] << " --load <qtc_path>\n";
        return 1;
    }
}