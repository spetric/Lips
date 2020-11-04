#pragma once
// consts
const int OCW_ERR_NO_SOURCE = 1;
const int OCW_ERR_NO_TARGET = 2;
const int OCW_ERR_BAD_TYPE = 3;
const int OCW_ERR_BAD_PARAM = 4;
const int OCW_ERR_EMPTY_DATA = 5;
const int OCW_ERR_BAD_INDEX = 6;
const int OCW_ERR_NULL_OBJECT = 7;
const int OCW_ERR_NO_EXPORT = 8;
const int OCW_ERR_EXCEPTION = 9;
const int OCW_ERR_NO_SOURCE_MASK = 10;
const int OCW_ERR_BAD_SOURCE_MASK = 11; // not 8U1
const int OCW_ERR_ALPHATARGET_EMPTY = 12;
const int OCW_ERR_BAD_DATA = 13;
const int OCW_ERR_SIZE_DIFF = 14;
const int OCW_ERR_NOT_IMPLEMENTED = 15;
// enums
enum TImgType {
	OCW_IMG_SOURCE_RGB = 0, OCW_IMG_SOURCE_RGBA, OCW_IMG_TARGET_RGB, OCW_IMG_TARGET_RGBA, OCW_MASK_SOURCE, OCW_MASK_TARGET
};
enum TImgSelect { OCW_IMAGE_SOURCE = 0, OCW_IMAGE_TARGET, OCW_IMAGE_EXPORT };
enum TRoiType { OCW_ROI_ALL = 0, OCW_ROI_SOURCE, OCW_ROI_TARGET };
enum TBorderType {OCW_BORDER_CONSTANT = 0, OCW_BORDER_REPLICATE, OCW_BORDER_REFLECT, OCW_BORDER_WRAP, OCW_BORDER_REFLECT_101,
	OCW_BORDER_TRANSPARENT, OCW_BORDER_REFLECT101 = OCW_BORDER_REFLECT_101,
	OCW_BORDER_DEFAULT = OCW_BORDER_REFLECT_101, OCW_BORDER_ISOLATED = 16
};
enum TMorphShape { OCW_MORPH_RECT = 0, OCW_MORPH_CROSS, OCW_MORPH_ELLIPSE };
enum TMorphType {OCW_MORPH_ERODE = 0, OCW_MORPH_DILATE, OCW_MORPH_OPEN, OCW_MORPH_CLOSE, OCW_MORPH_GRADIENT, OCW_MORPH_TOPHAT,
	OCW_MORPH_BLACKHAT, OCW_MORPH_HITMISS
};
enum TThresholdType {OCW_THRESH_BINARY = 0, OCW_THRESH_BINARY_INV, OCW_THRESH_TRUNC, OCW_THRESH_TOZERO, OCW_THRESH_TOZERO_INV,
	OCW_THRESH_MASK, OCW_THRESH_OTSU, OCW_THRESH_TRIANGLE
};
enum TAdaptiveThresholdType { OCW_ADAPTIVE_THRESH_MEAN_C = 0, OCW_ADAPTIVE_THRESH_GAUSSIAN_C };
enum TDistanceType {OCW_DST_USER = -1, OCW_DIST_L1 = 1, OCW_DIST_L2 = 2, OCW_DIST_C = 3, OCW_DIST_L12 = 4,
	OCW_DIST_FAIR = 5, OCW_DIST_WELSCH = 6, OCW_DIST_HUBER = 7
};
enum TDistanceMaskType { OCW_DIST_MASK_3 = 3, OCW_DIST_MASK_5 = 5, OCW_DIST_MASK_PRECISE = 0 };
enum TInterpolationFlag { OCW_INTER_NEAREST = 0, OCW_INTER_LINEAR = 1, OCW_INTER_CUBIC = 2, OCW_INTER_AREA = 3, OCW_INTER_LANCZOS4 = 4, OCW_INTER_LINEAR_EXACT = 5,
	OCW_INTER_MAX = 7, OCW_WARP_FILL_OUTLIERS = 8, OCW_WARP_INVERSE_MAP = 16
};
enum TInterpolationMask { OCW_INTER_BITS = 5, OCW_INTER_BITS2 = OCW_INTER_BITS * 2, OCW_INTER_TAB_SIZE = 1 << OCW_INTER_BITS, OCW_INTER_TAB_SIZE2 = OCW_INTER_TAB_SIZE * OCW_INTER_TAB_SIZE };
enum TWarpPolarMode { OCW_WARP_POLAR_LINEAR = 0, OCW_WARP_POLAR_LOG = 256 };
enum TFaceModelType { OCW_FACE_KAZEMI = 0, OCW_FACE_LBF, OCW_FACE_AAM };
enum TEdgeType { OCW_ET_NONE = 0, OCW_ET_CANNY, OCW_ET_SOBEL_V, OCW_ET_SOBEL_H };
enum TKernelFunc { OCW_KF_COS = 0, OCW_KF_SIN = 1, OCW_KF_COS_C = 2, OCW_KF_SIN_C = 3, OCW_KF_TRIANGLE = 4, OCW_KF_SQUARE = 10 };
enum TContourRetrievalMode { OCW_RETR_EXTERNAL = 0, OCW_RETR_LIST = 1, OCW_RETR_CCOMP = 2, OCW_RETR_TREE = 3, OCW_RETR_FLOODFILL = 4 };
enum TContourApproximationMode { OCW_CHAIN_APPROX_NONE = 1, OCW_CHAIN_APPROX_SIMPLE = 2, OCW_CHAIN_APPROX_TC89_L1 = 3, OCW_CHAIN_APPROX_TC89_KCOS = 4 };
enum TFeature2DType { OCW_FD_AKAZE = 0, OCW_FD_ORB, OCW_FD_BRISK, OCW_FD_KAZE, OCW_FD_SURF, OCW_FD_SIFT, OCW_FD_FAST_DAISY, OCW_FD_FAST_FREAK};
enum TFeature2DChoice {OCW_FCH_SRCTGT = 0, OCW_FCH_SRC, OCW_FCH_TGT, OCW_FCH_SRCTGT_MASK, OCW_FCH_SRC_MASK, OCW_FCH_TGT_MASK};
enum TMatchType { OCW_MT_BRUTE_HAMMING = 0, OCW_MT_BFM, OCW_MT_FLANN };   //TODO: etc...
enum THomographyType { OCW_HOMO_LS = 0, OCW_HOMO_RANSAC, OCW_HOMO_LMEDS, OCW_HOMO_RHO };
enum TCloneType {OCW_CLONE_NORMAL = 1, OCW_CLONE_MERGE, OCW_CLONE_MT};
enum TInpaintType {OCW_INPAINT_NS = 0, OCW_INPAINT_TELEA, OCW_INPAINT_FUZZY_ONESTEP, OCW_INPAINT_FUZZY_MULTISTEP, OCW_INPAINT_FUZZY_ITERATIVE, OCW_INPAINT_FSR_BEST, OCW_INPAINT_FSR_FAST, OCW_INPAINT_SHIFT_MAP };
enum TDnnSuperResType {OCW_SUPERRES_ESPCN = 0, OCW_SUPERRES_FSRCNN, OCW_SUPERRES_LAPSRN, OCW_SUPERRES_EDSR};
enum TSaliencyType {OCW_SALIENCY_FINE_GRAINED = 0, OCW_SALIENCY_SPECTRAL_RESIDUAL};
enum TFeature2DReduction {OCW_HOSR_OUTLIERS = 0, OCW_HOSR_RKD_ASIS, OCW_HOSR_RKD_ADJUST, OCW_HOSR_TGT_MASK, OCW_HOSR_CLUSTER}; // for multiple objects and enhanecment
enum TRotateMode {OCW_ROTATE_90_CW = 0, OCW_ROTATE_180, OCW_ROTATE_90_CCW};
enum TNormType { OCW_NORM_INF = 1, OCW_NORM_L1 = 2, OCW_NORM_L2 = 4, OCW_NORM_L2SQR = 5, OCW_NORM_HAMMING = 6, OCW_NORM_HAMMING2 = 7, OCW_NORM_TYPE_MASK = 7, OCW_NORM_RELATIVE = 8, OCW_NORM_MINMAX = 32};
enum TTemplateMatchingModes { OCW_TEMPLMODE_SQDIFF = 0, OCW_TEMPLMODE_SQDIFF_NORMED = 1, OCW_TEMPLMODE_CCORR = 2, OCW_TEMPLMODE_CCORR_NORMED = 3, OCW_TEMPLMODE_CCOEFF = 4, OCW_TEMPLMODE_CCOEFF_NORMED = 5 };
enum TTemplateReduction {OCW_TEMPLREDUCT_POINT = 0, OCW_TEMPLREDUCT_FR, OCW_TEMPLREDUCT_HR, OCW_TEMPLREDUCT_FHR, OCW_TEMPLREDUCT_MDIST};
// structs
struct SocvRoi
{
	int left;
	int right;
	int top;
	int bottom;
};
struct SocvImageData
{
	int width;
	int height;
	int channels;
	void *imageBuff;
	int imageStride;
	void *alphaBuff;
	int alphaStride;
};
struct SocvTuple
{
	int rows;
	int cols;
	float **src;
	float **dst;
};
struct SocvObjectData
{
	unsigned int numObjects;
	int *bRects;   // bounding rect => mat: cols = 5, rows = numObjects, type = 16s, content = (x, y, width, height, numLandmarkPts) 
	float *lPts;   // landmark points => mat: cols = 2, rows = numObjects * sum(bRects[i][4]
};
struct SocvPoint
{
	int x;
	int y;
};
struct SocvContourPts
{
	int hierarchy[4];		// for simplified output (contour iteration)
	unsigned int numPts;
	SocvPoint *points;
};
struct SocvContourData
{
	unsigned int numContours;
	int *hierarchy;
	SocvContourPts *contours;
};
struct SocvLineData
{
	unsigned int numLines;
	void *line;
};
struct SocvBlobParams
{
	unsigned char blobColor; 
	bool 	filterByArea;
	bool 	filterByCircularity;
	bool 	filterByColor;
	bool 	filterByConvexity; 
	bool 	filterByInertia; 
	float 	maxArea; 
	float 	maxCircularity; 
	float 	maxConvexity; 
	float 	maxInertiaRatio; 
	float 	maxThreshold; 
	float 	minArea;
	float 	minCircularity; 
	float 	minConvexity;
	float 	minDistBetweenBlobs;
	float 	minInertiaRatio; 
	unsigned int minRepeatability;
	float 	minThreshold; 
	float 	thresholdStep;

};
struct SocvBlobData
{
	// output data
	unsigned int numBlobs;

};
struct SocvHomography
{
	TFeature2DType fType;
	int  maxSrcKeys;
	int  maxTgtKeys;
	TMatchType mType;
	float matchParam;
	unsigned int initMinMatches;
	unsigned int minMatches;
	THomographyType hType;
	bool exportImage;
	bool addAlpha;
	bool warpCrop;
	int matches;
	int inliers;
	int outliers;
	int shiftX;
	int shiftY;
	int bbTop;
	int bbLeft;
	int bbWidth;
	int bbHeight;
	double stNorm;	// source-target norm
};
struct SocvTemplateData
{
	int posX;
	int posY;
	double minVal;
	double maxVal;
	bool valid;
};

