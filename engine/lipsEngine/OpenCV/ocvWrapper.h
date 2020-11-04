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
extern "C" OCV_API int ocvWarpPerspective(const SocvTuple *wparams, bool exportImage = false, int flag = OCW_INTER_LINEAR, TBorderType borderType = OCW_BORDER_CONSTANT);
extern "C" OCV_API int ocvWarpAffine(const SocvTuple *wparams, bool exportImage = false, int flag = OCW_INTER_LINEAR, TBorderType borderType = OCW_BORDER_CONSTANT);
extern "C" OCV_API int ocvFlip(int flipCode);
extern "C" OCV_API int ocvFlipInplace(int flipCode, TImgSelect iSel);
extern "C" OCV_API int ocvResize(int width, int height, int flag);
extern "C" OCV_API int ocvRotate(TRotateMode rmode = OCW_ROTATE_90_CCW);
// image npr
extern "C" OCV_API int ocvDetailEnhance(float sigma_s = 10,	float sigma_r = 0.15f);
extern "C" OCV_API int ocvEdgePreservingFilter(int flags = 1, float sigma_s = 60, float sigma_r = 0.4f);
extern "C" OCV_API int ocvPencilSketch(bool grayscale = true, float sigma_s = 60, float sigma_r = 0.07f, float shade_factor = 0.02f);
extern "C" OCV_API int ocvStylization(float	sigma_s = 60, float	sigma_r = 0.45f );
extern "C" OCV_API int ocvOilPainting(int size, int dynRatio);
// test
extern "C" OCV_API int ocvDnnReadModel(const wchar_t *model, const wchar_t *config, int dkId);
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
// feature detection
extern "C" OCV_API int ocvOdBegin(void);
extern "C" OCV_API int ocvOdEnd(void);
extern "C" OCV_API int ocvOdCalcFeatures(TFeature2DType type = OCW_FD_AKAZE,  TFeature2DChoice choice = OCW_FCH_SRCTGT, int maxSrcKeys = 0, int maxTgtKeys = 0);
extern "C" OCV_API int ocvOdCalcMatches(TMatchType type = OCW_MT_BRUTE_HAMMING, float matchParam = 0.15f, bool exportImage = true);
extern "C" OCV_API int ocvOdCalcHomography(unsigned int MinMatches = 6, THomographyType type = OCW_HOMO_RANSAC, bool exportImage = true, bool addAlpha = true, bool warpCrop = false);
extern "C" OCV_API int ocvOdReduceFeatures(TFeature2DReduction redType = OCW_HOSR_OUTLIERS, bool exportMatchImage = false);
extern "C" OCV_API int ocvOdClusterizeFeatures(int clusterNum, bool target = true, int maxIters = 10, double epsilon = 0.1, int attempts = 3, bool flagKpp = true);
extern "C" OCV_API int ocvOdRecalcHomography(void);
extern "C" OCV_API int ocvOdCalcNorm(TNormType normType = OCW_NORM_L2, bool normGray = false);
extern "C" OCV_API int ocvOdGetHomographyData(SocvHomography &homoData);
// template matching
extern "C" OCV_API int ocvTmBegin(void);
extern "C" OCV_API int ocvTmEnd(void);
extern "C" OCV_API int ocvTmMatchFirst(TTemplateMatchingModes tplMode = OCW_TEMPLMODE_CCORR_NORMED);
extern "C" OCV_API int ocvTmMatchNext(TTemplateReduction reductType = OCW_TEMPLREDUCT_MDIST, double threshold = 0.9, int mdist = 0);
extern "C" OCV_API int ocvTmGetData(SocvTemplateData &tmData);
// blob detector
extern "C" OCV_API int ocvBlobGetDefaultParams(SocvBlobParams *blobParams);
extern "C" OCV_API int ocvBlobDetector(SocvBlobParams *blobParams, SocvBlobData *blobData, bool drawKpts2Target = true);
// extra features
extern "C" OCV_API int ocvInpaint(TInpaintType type = OCW_INPAINT_NS, double radius = 3.0);
// test
extern "C" OCV_API int ocvColorize(int dkId);
extern "C" OCV_API int ocvSeamlessClone(int centerX, int centerY, TCloneType type = OCW_CLONE_MERGE);
extern "C" OCV_API int ocvDnnSuperResolution(const wchar_t *model, TDnnSuperResType type,  int scale);
extern "C" OCV_API int ocvColorTransfer(bool exportImage = false);
extern "C" OCV_API int ocvStaticSaliency(TSaliencyType type = OCW_SALIENCY_FINE_GRAINED);
// flood fill
extern "C" OCV_API int ocvFloodFillSimple(SocvPoint seed, int tolLo, int tolHi, int connectivity, int fillMode, unsigned char *pixBGR = 0);
extern "C" OCV_API int ocvFloodFillMasked(SocvPoint seed, int tolLo, int tolHi, int connectivity, int fillMode, unsigned char *pixBGR = 0, int maskVal = 255);
// mixed non-ocv related (MAT used only as image container)
extern "C" OCV_API int ocvImageDiff(int patchRadius = 3, int radius = 3, int iterations = 1, int err_tol = 3, int output_threshold = 60, int out_stype = 1); 
// TODO: patchmath inpaint