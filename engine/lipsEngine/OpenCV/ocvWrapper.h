#pragma once
#ifdef OCVWRAPPER_EXPORTS
#define OCV_API __declspec(dllexport)
#else
#define OCV_API __declspec(dllimport)
#endif
#include "ocvGlobals.h"
// NOT exported functions (internal)
//int ocv_checkImgAndRoi();
//TocvDataKeeper* ocv_createDK(void);
// DLL entries
// init
extern "C" OCV_API int ocvInit(void);
// close (cleanup)
extern "C" OCV_API int ocvClean(void);
extern "C" OCV_API int ocvCreateDK(void);
extern "C" OCV_API int ocvGetLastDKIdx(void);
extern "C" OCV_API int ocvClearDK(int idx);
// helpers
extern "C" OCV_API int ocvEnableAlpha(bool value);
// set images
extern "C" OCV_API int ocvSetImage(TImgType type, int width, int height, void *imageBuff, int imageStride, void *alphaBuff = 0, int alphaStride = 0);
// roi
extern "C" OCV_API int ocvSetRoi(TRoiType type, SocvRoi roi);
extern "C" OCV_API int ocvClearRoi(TRoiType type);
// object data cleanup
extern "C" OCV_API int ocvReleaseObject(SocvObjectData *exportData); // obsolet
// exported image data
extern "C" OCV_API SocvImageData* ocvGetExportImage(void);
// dyadic manipulation
extern "C" OCV_API int ocvCopy(TImgSelect inpType, TImgSelect outType);
// image processing - filters
extern "C" OCV_API int ocvBlur(unsigned int kX, unsigned int kY, TBorderType type = OCW_BORDER_DEFAULT);
extern "C" OCV_API int ocvMedianBlur(unsigned int kSize);
extern "C" OCV_API int ocvGaussianBlur(unsigned int kX, unsigned int kY, double sigmaX, double sigmaY, TBorderType type = OCW_BORDER_DEFAULT);
extern "C" OCV_API int ocvBilateralFilter(int d, double	sigmaColor, double sigmaSpace, TBorderType type = OCW_BORDER_DEFAULT);
extern "C" OCV_API int ocvLaplacian(unsigned int ksize = 1, double scale = 1, double delta = 0, bool cv2gray = false, TBorderType type = OCW_BORDER_DEFAULT);
extern "C" OCV_API int ocvScharr(unsigned int dx, unsigned int dy, double scale = 1, double delta = 0, bool cv2gray = false, TBorderType type = OCW_BORDER_DEFAULT);
extern "C" OCV_API int ocvSobel(unsigned int dx, unsigned int dy, int ksize = 3, double scale = 1, double delta = 0, bool cv2gray = false, TBorderType type = OCW_BORDER_DEFAULT);
extern "C" OCV_API int ocvBoxFilter(unsigned int kX, unsigned int kY, bool normalize = true, TBorderType type = OCW_BORDER_DEFAULT);
extern "C" OCV_API int ocvSqrBoxFilter(unsigned int kX, unsigned int kY, bool normalize = true, TBorderType type = OCW_BORDER_DEFAULT);
extern "C" OCV_API int ocvDilate(unsigned int kX, unsigned int kY, TMorphShape shape = OCW_MORPH_RECT, unsigned int iterations = 1, TBorderType type = OCW_BORDER_CONSTANT);
extern "C" OCV_API int ocvErode(unsigned int kX, unsigned int kY, TMorphShape shape = OCW_MORPH_RECT, unsigned int iterations = 1, TBorderType type = OCW_BORDER_CONSTANT);
extern "C" OCV_API int ocvMorphologyEx(unsigned int kX, unsigned int kY, TMorphType opType, TMorphShape shape = OCW_MORPH_RECT, unsigned int iterations = 1, TBorderType borType = OCW_BORDER_CONSTANT);
extern "C" OCV_API int ocvThreshold(double thresh, double maxval, TThresholdType type);
extern "C" OCV_API int ocvAdaptiveThreshold(double maxval, TAdaptiveThresholdType method, TThresholdType type, unsigned int radius,  double C = 2);
extern "C" OCV_API int ocvDistanceTransform(bool gray = true, TDistanceType type = OCW_DIST_L1, TDistanceMaskType maskSize = OCW_DIST_MASK_3);
extern "C" OCV_API int ocvCanny(double threshold1 = 1, double threshold2 = 3, unsigned int apertureSize = 3, bool L2gradient = false);
extern "C" OCV_API int ocvFilter2D(unsigned int kernelSize, float *kernel, float div = 1.0, float bias = 0.0, TBorderType borderType = OCW_BORDER_CONSTANT);
// image processing - geometric transforms
extern "C" OCV_API int ocvWarpPolar(int x, int y, double maxRadius, bool exportImage = false, int flag = OCW_INTER_LINEAR);
extern "C" OCV_API int ocvWarpPerspective(const SocvWarpParams &wparams, bool exportImage = false, int flag = OCW_INTER_LINEAR, TBorderType borderType = OCW_BORDER_CONSTANT);
extern "C" OCV_API int ocvWarpAffine(const SocvWarpParams &wparams, bool exportImage = false, int flag = OCW_INTER_LINEAR, TBorderType borderType = OCW_BORDER_CONSTANT);
extern "C" OCV_API int ocvFlip(int flipCode);
extern "C" OCV_API int ocvFlipInplace(int flipCode, TImgSelect iSel);
// image npr
extern "C" OCV_API int ocvDetailEnhance(float sigma_s = 10,	float sigma_r = 0.15f);
extern "C" OCV_API int ocvEdgePreservingFilter(int flags = 1, float sigma_s = 60, float sigma_r = 0.4f);
extern "C" OCV_API int ocvPencilSketch(bool grayscale = true, float sigma_s = 60, float sigma_r = 0.07f, float shade_factor = 0.02f);
extern "C" OCV_API int ocvStylization(float	sigma_s = 60, float	sigma_r = 0.45f );
extern "C" OCV_API int ocvOilPainting(int size, int dynRatio);
// test
extern "C" OCV_API int ocvDnnReadModel(const wchar_t *model, int dkId);
extern "C" OCV_API int ocvStyleTransfer(int dkId, double scale = 1.0, double meanR = 103.939, double meanG = 116.779, double meanB = 123.680, bool normResult = false, bool swapRB = false);
extern "C" OCV_API int ocvLoadClassifierAndFaceModel(const wchar_t *cascade_name, const wchar_t *model_name, TFaceModelType type, int dkId);
extern "C" OCV_API int ocvFaceLandmarkDetector(TImgSelect type, int dkId, bool singleFace = true, bool extrapolate = true, double scaleFactor = 1.05, unsigned int minNeighbors = 3, unsigned int minSize = 30);
extern "C" OCV_API int ocvSwapFaces(TImgSelect type1, TImgSelect type2, int dkId);
// contour/lines detection
extern "C" OCV_API int ocvFindContours(TEdgeType edgeType, TContourRetrievalMode retMode, TContourApproximationMode appMode, SocvContourData *exportContours);
extern "C" OCV_API int ocvReleaseContours(SocvContourData *importContours);
extern "C" OCV_API int ocvDetectLines(TEdgeType type, SocvLineData *exportLines, bool c2bw = true);
extern "C" OCV_API int ocvHoughLines(TEdgeType type, SocvLineData *exportLines, bool c2bw = true);
//
extern "C" OCV_API int ocvGaborFilter(unsigned int kernelSize = 31, double theta = 0, double sigma = 1, double lambda = 1, double gamma = 0.02, double psi = 0, double ksFactor = 0);
extern "C" OCV_API int ocvGaborius(unsigned int step = 16, unsigned int kernelSize = 31, double sigma = 4, double lambda = 10, double gamma = 0.5, double psi = 0, double ksFactor = 1.5);
extern "C" OCV_API int ocvTrigonometrius(int fType = OCW_KF_COS, unsigned int step = 16, unsigned int kernelSize = 31, double hSigma = 0.5, double freq = 3, double ksFactor = 1.5, int margin = -1, double vSigma = 0);
// extra features
extern "C" OCV_API int ocvInpaint(unsigned int patchSize, bool zeroSource);
