# Quadtree Image Compression

A C++ project that demonstrates image compression and visualization using the quadtree data structure. The project leverages OpenCV for image processing and visualization.

## Features

- Compresses images using a quadtree, splitting regions based on color variance.
- Visualizes the compressed image by coloring each quadtree leaf node with its average color.
- Adjustable threshold for controlling the compression level.
- Simple command-line interface.

## Requirements

- C++ compiler (supporting C++11 or later)
- [OpenCV](https://opencv.org/) (tested with OpenCV 4.x)
- CMake (version 3.10 or higher)

## Build Instructions

1. **Clone the repository:**
   ```bash
   git clone <repo-url>
   cd Quadtree-image-compression
   ```

2. **Build with CMake:**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

Run the executable with the path to an image and an optional threshold value:

```bash
./quadtree_compression <image_path> [threshold]
```

- `<image_path>`: Path to the input image (e.g., `../tests/images/flower.png`)
- `[threshold]`: (Optional) Non-negative integer controlling the color variance threshold for splitting nodes (default: 10).

**Example:**
```bash
./quadtree_compression ../tests/images/flower.png 15
```

A window will display the compressed image. You can resize the window as needed.

## How It Works

- The image is recursively divided into quadrants.
- If the color variance in a region exceeds the threshold, it is subdivided further.
- Leaf nodes are colored with the average color of their region.
- The result is a compressed, blocky representation of the original image.



