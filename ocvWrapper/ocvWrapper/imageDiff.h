#pragma once
#include "opencv2/opencv.hpp"
class TImageDiff
{
public:
	const int idX = 1;
	const int idY = 0;
	const int idError = 2;
	const int csError = 3;
	const int msError = 4;
	const int arrElems = 5;
	const int K33i[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
	const int K33j[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
private:
	cv::Mat *FSource;
	cv::Mat *FTarget;
	cv::Mat *FTargetAlpha;
	int ***FOutData;
	int FPatchSize, FTgtHeight, FTgtWidth, FSrcHeight, FSrcWidth;
	void initOutData(int dia);
	int  distance(int &sx, int &sy, int &tx, int &ty);
   	void pixelDT(int &sx, int &sy, int &tx, int &ty, int &radius, int &small_error, int &max_sec, int &min_sec);
public:
	TImageDiff(cv::Mat *source, cv::Mat *target, cv::Mat *targetAlpha);
	// executors
	int*** ExecuteDT(int patchRadius, int radius, int iterations, int err_tol, int output_threshold, int out_type = 0);
	~TImageDiff();
};

