
#include <FindGrid.h>
tuple<int, int, int, int> locateGrid()
{

    // Load the template image (replace with your actual grid cell image)
    Mat templateImage = imread("grid_cell.png");

    // Check if template image loaded successfully
    if (templateImage.empty()) {
        cerr << "Error: Could not load template image." << endl;
        return make_tuple(-100, -100, 0, 0);
    }

    // Capture the screen (replace with your screen capture method)
    Mat screenImage;
    // ... (Your screen capture code here) ...

    // Check if screen capture was successful (replace with your error handling)
    if (screenImage.empty())
    {
        cerr << "Error: Could not capture screen." << endl;
        return make_tuple(-100, -100, 0, 0);
    }

    // Convert images to grayscale for better matching
    Mat screenGray, templateGray;
    cvtColor(screenImage, screenGray, COLOR_BGR2GRAY);
    cvtColor(templateImage, templateGray, COLOR_BGR2GRAY);

    // Perform template matching using TM_CCOEFF_NORMED (normalized correlation)
    Mat result;
    matchTemplate(screenGray, templateGray, result, TM_CCOEFF_NORMED);

    // Find the best match location
    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // Check if a good match was found (adjust threshold as needed)
    if (maxVal > 0.95) {
        // Top left corner is the max location
        int topLeftX = maxLoc.x;
        int topLeftY = maxLoc.y;

        // Get the grid dimensions from the template size
        int gridWidth = templateImage.cols;
        int gridHeight = templateImage.rows;

        return make_tuple(topLeftX, topLeftY, gridWidth, gridHeight);
    }
}
