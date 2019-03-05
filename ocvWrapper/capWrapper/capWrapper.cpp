// capWrapper.cpp : This file contains the 'main' function. Program execution begins and ends there.
// This is a simple console application for testing ocvWrapper DLL APIs
#include "stdafx.h"
#include <iostream>
#include <string>
#include "ocvWrapper.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\features2d\features2d.hpp"
using namespace cv;

int main()
{
	ocvInit();
	ocvEnableAlpha(false);
	Mat imInput, imOutput, imMask;
	// read input image
    String inputFilename("C:/images/image_source.jpg");		// input image
	imInput = imread(inputFilename, IMREAD_COLOR);
	if (imInput.empty())                      // Check for invalid input
	{
		std::cout << "Could not open or find input image" << std::endl;
		return -1;
	}
	namedWindow("Inp image", WINDOW_AUTOSIZE);  // Create a window for display.
	imshow("Inp image", imInput);               // Show our image inside it.
	waitKey(1);
	// in case you want to copy source to target
	// imInput.copyTo(imOutput);
	//
	// read output image
	String outputFilename("C:/images/image_target.jpg");	// output image
	imOutput = imread(outputFilename, IMREAD_COLOR);
	if (imOutput.empty())                      // Check for invalid output
	{
		std::cout << "Could not open or find output image" << std::endl;
		return -1;
	}
	namedWindow("Out image", WINDOW_AUTOSIZE);   // Create a window for display.
	imshow("Out image", imOutput);               // Show our image inside it.
	waitKey(1);
	// set input image as source
	ocvSetImage(OCW_IMG_SOURCE_RGB, imInput.cols, imInput.rows, (void*)imInput.data, imInput.step);
	// set output image as target
	ocvSetImage(OCW_IMG_TARGET_RGB, imOutput.cols, imOutput.rows, (void*)imOutput.data, imOutput.step);
	// example of setting ROI
	// SocvRoi roi;
	// roi.left = 50;
	// roi.right = 300;
	// roi.top = 50;
	// roi.bottom = 300;
	// ocvSetRoi(OCW_ROI_ALL, roi);
	int rc = ocvDetailEnhance();    // example DLL function call
	imshow("Out image", imOutput);  // Refresh output image
	waitKey(0); // Wait for a keystroke in the window
	//	
	return 0;
}

