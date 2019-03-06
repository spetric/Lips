//Copyright (C) 2016 Siniša Petriæ
#ifndef SP_IMAGE_H
#define SP_IMAGE_H
#include <Types.hpp>

using namespace std;

// Container for a 2D image
// This class ensure that the image have a width and a height
// Data is saved in row major format
//---------------------------
enum speEdgeOptions {eopSkip, eopWrap, eopBorder};
enum speOrientation {orAsIs, orInvert};

template <class T> class spImage
  {
  public:
	typedef T Tpixel;
  speEdgeOptions PixelOverFlow;
  //------------------------------
  // constructor - allocate image width height
  //------------------------------
  spImage (int width, int height, bool contiguous = false, speOrientation orientation = orAsIs)
	{
	initVars();
	_height = height;
	_width = width;
	calculateAlignement();
	_data = new T*[_height];
   	_orientation = orientation;
	_allocateMemory(contiguous, sizeof(T));
	//int test = sizeof(T);
	SetRoi();
	}
  //------------------------------
  // constructor - allocate image width height
  //------------------------------
  spImage (int width, int height, unsigned int scanlineAlignment, bool contiguous = true, speOrientation orientation = orAsIs)
	{
	initVars();
	_height = height;
	_width = width;
	_scanlineAlignment = scanlineAlignment;
	calculateAlignement();
	_data = new T*[_height];
   	_orientation = orientation;
	_allocateMemory(contiguous, sizeof(T));
	SetRoi();
	}
  //------------------------------
  // constructor - copy image
  //------------------------------
  spImage(const spImage<T> *I, bool contiguous = false)
	{
	initVars();
	_height    = I->_height;
	_width     = I->_width;
	_pad 	   = I->_pad;
	_scanlineAlignment = I->_scanlineAlignment;
	_orientation = I->_orientation;
	calculateAlignement();
	if (I->_original_buffer)	// copy original buffer
	   {
	   _original_buffer = GetMemory(_stride * _height);
	   memcpy(_original_buffer, I->_original_buffer, _stride * _height);
	   _data = new T*[_height];
	   fillDataFromBuffer();
	   }
	else
	   {
	   _data = new T*[_height];
	   _allocateMemory(contiguous, sizeof(T));
	   // copy  data
	   for (int i = 0; i < _height; i++)
		   memcpy(_data[i], I->_data[i], _width * sizeof(T) + _pad);   // a more i preko stride-a
	   }
	SetRoi();
	}
  //------------------------------
  // constructor - share/no-share image
  //------------------------------
  spImage(void **scanLineData, int width, int height, bool shared = true)
	{
	initVars();
	constructFromScanLine(scanLineData, width, height, shared);
	SetRoi();
	}
  //------------------------------
  // constructor - share/no-share image
  //------------------------------
  spImage(Byte **scanLineData, int width, int height, bool shared = true)
	{
	initVars();
	constructFromScanLine(reinterpret_cast<void**>(scanLineData), width, height, shared);
	SetRoi();
	}
  //------------------------------
  // constructor - raw image void
  //------------------------------
  spImage(void *buffer, int width, int height, unsigned int scanlineAlignment, bool shared = true, speOrientation orientation = orAsIs)
	{
	initVars();
	constructFromBuffer(buffer, width, height, scanlineAlignment, shared, orientation);
	_orientation = orientation;
	SetRoi();
	}
  //------------------------------
  // constructor - raw image  byte
  //------------------------------
  spImage(Byte *buffer, int width, int height, unsigned int scanlineAlignment, bool shared = true, speOrientation orientation = orAsIs)
	{
	initVars();
	constructFromBuffer(reinterpret_cast <void*>(buffer), width, height, scanlineAlignment, shared, orientation);
	_orientation = orientation;
	SetRoi();
	}
  //------------------------------
  // destructor
  //------------------------------
  virtual ~spImage()
	{
	_freeMemory();
	}
  //------------------------------
  // accessors/getters methods
  //------------------------------
  // Retrieve the width of the image
  int Width()  const { return _width; }
  // Retrieve the height of the image
  int Height() const { return _height; }
  // Retrieve image alignement
  unsigned int Alignment() const { return _scanlineAlignment; }
  // Rrtrieve stride
  unsigned int Stride() const { return _stride; }
  // Return the depth in byte of the pixel (unsigned char will return 1)
  int Depth() const  { return sizeof(Tpixel);}
  // Is image contiguous
  //bool IsContiguous() const  { return (bool)_contiguous_memory;}
  bool IsContiguous() const  { return (bool)_original_buffer;}
  //------------------------------
  // image memory access
  //------------------------------
  // data block
  T ** Data() const {return _data;}
  // contiguous memory pointer
  //T * GetContiguous() const  { return _contiguous_memory;}
  // original buffer pointer
  void * GetContiguous() const  { return _original_buffer;}
  void * GetOriginalBuffer() const  { return _original_buffer;}
  // random pixel access
  //inline const T & operator()(int y, int x) const {return _data[y][x];}
  // random pixel access
  inline T & operator()(int y, int x) {return _data[y][x];}
  // random pixel access
  //inline const T & Pixel(int y, int x) const {return _data[y][x];}
  // random pixel access
  inline T & Pixel(int y, int x) {return _data[y][x];}
  // are imagea equal
  inline bool operator==(const spImage<T> *img) const
	{
	if (_width == img->_width && _height == img->_height)
	   {
	   for (int i = 0; i < _height; i++)
		   {
		   if (0 != memcmp(_data[i], img->_data[i], _width))
			  return false;
		   }
	   return true;
	   }
	return false;
	}
  //------------------------------
  // accessors/getters methods
  //------------------------------

  // Tell if a point is inside the image.
  bool Contains(int y, int x) const
	{
	return x < _width && y < _height;
	}
  // Tell if a point is inside the image.
  bool Contains(double y, double x) const
	{
	return (x >=0 && x <_width && y >=0 && y <_height);
	}
  void SetRoi(void)
	{
	_roi.top = 0;
	_roi.left = 0;
	_roi.bottom = _height;
	_roi.right = _width;
	}
  void SetRoi(TRect *rect, bool chkBoundaries = false)
	{
	_roi.top = rect->top;
	_roi.left = rect->left;
	_roi.bottom = rect->bottom;
	_roi.right = rect->right;
	_roi.NormalizeRect();
	if (chkBoundaries)
	   checkRoiBoundaries();
	}
  void SetRoi(int x1, int y1, int x2, int y2, bool chkBoundaries = false)
	{
	_roi.top = y1;
	_roi.left = x1;
	_roi.bottom = y2;
	_roi.right = x2;
	_roi.NormalizeRect();
	if (chkBoundaries)
	   checkRoiBoundaries();
	}
  TRect &GetRoi()
	{
    return _roi;
    }
  //------------------------------
  // Utils
  //------------------------------
  // Fill with color
  //------------------------------
  void Fill(const T &val)
    {
	// this should work faster
	for (int j = 0; j < _width; j++)
			_data[0][j] = val;
	for (int i = 1; i < _height; i++)
		memcpy(_data[i], _data[0], _width * sizeof(T));
	}
  //------------------------------
  // Fill with zeros
  //------------------------------
  void Zero(void)
	{
	// this sohould work fast
	//if (_contiguous_memory)
	if (_original_buffer)
	   {
	   if (_orientation == orAsIs)
		   //memset(static_cast<void*>(_data[0]), 0, (sizeof(T) * _width + _align) * _height);
		   memset(static_cast<void*>(_data[0]), 0, _stride * _height);
	   else
		   //memset(static_cast<void*>(_data[_height-1]), 0, (sizeof(T) * _width + _align) * _height);
		   memset(static_cast<void*>(_data[_height-1]), 0, _stride * _height);
	   }
	else
	   {
	   memset(static_cast<void*>(_data[0]), 0, sizeof(T) * _width);
	   for (int i = 1; i < _height; i++)
		   memcpy(_data[i], _data[0], _width * sizeof(T));
	   }
	}
  //------------------------------
  // Fill roi with zeros
  //------------------------------
  void ZeroRoi(void)
	{
	memset(static_cast<void*>(_data[_roi.top]), 0, sizeof(T) * _width);
	for (int i = _roi.top + 1; i < _roi.bottom; i++)
		 memcpy(_data[i], _data[_roi.top], _width * sizeof(T));
	}
  //------------------------------
  // copy image data
  //------------------------------
  void Copy(const spImage<T> *I)
	{
	//T **inData = I->_data;
	int height = min(_height, I->_height);
	int width  = min(_width,  I->_width);
	// copy  data
	for (int i = 0; i < height; i++)
		 memcpy(_data[i], I->_data[i], width * sizeof(T));
	}
  //------------------------------
  // copy roi image data
  //------------------------------
  void CopyRoi(const spImage<T> *I)
	{
	int roi_width = min(_roi.Width(), I->_roi.Width());
	// Pazi: u verziji 0.5 je bilo krivo, vidi dole!!!
	// for (int i = _roi.top, j = I->_roi.top; i < _roi.bottom || j < I->_roi.bottom ; i++, j++)
	for (int i = _roi.top, j = I->_roi.top; i < _roi.bottom && j < I->_roi.bottom ; i++, j++)
		{
		memcpy(_data[i] + _roi.left, I->_data[j] + I->_roi.left, roi_width * sizeof(T));
		}
	}
  //------------------------------
  // copy roi to external buffer
  //------------------------------
  void CopyRoi2External(void *buffer)
	{
	int roi_width = _roi.Width();
	T* tPtr = static_cast<T*>(buffer);
	for (int i = _roi.top; i < _roi.bottom; i++)
		{
		memcpy(tPtr, _data[i] + _roi.left, roi_width * sizeof(T));
		tPtr += roi_width;
		}
	}
  //------------------------------
  // copy external buffer to roi
  //------------------------------
  void CopyExternal2Roi(void *buffer)
	{
	int roi_width = _roi.Width();
	T* tPtr = static_cast<T*>(buffer);
	for (int i = _roi.top; i < _roi.bottom; i++)
		{
		memcpy(_data[i] + _roi.left, tPtr, roi_width * sizeof(T));
		tPtr += roi_width;
		}
	}
  //------------------------------
  // copy raw data (images with equal size)
  //------------------------------
  void Copy(void *buffer, speOrientation orientation = orAsIs)
	{
	void *currPtr = buffer;
	if (orientation == orAsIs)
	   {
	   for (int i = 0; i < _height; i++)
		   {
		   memcpy(_data[i], static_cast<T*>(currPtr), _width * sizeof(T));
		   currPtr = static_cast<void*>((Byte*)currPtr + _stride);
		   }
	   }
	else
	   {
	   for (int i = _height-1; i >= 0; i--)
		   {
		   memcpy(_data[i], static_cast<T*>(currPtr), _width * sizeof(T));
		   currPtr = static_cast<void*>((Byte*)currPtr + _stride);
		   }
	   }
	}
  //------------------------------
  // Set safe index
  //------------------------------
  bool SetSafeIndex (int *index, int *jndex)
	{
	if (*index > 0 && *index < _height && *jndex > 0 && *jndex < _width)
	   return true;
	if (PixelOverFlow == eopWrap)
	   {
	   if (*index < 0)
		  *index = _height + *index;
	   else if ( *index >= _height)
		  *index =  *index - _height;
	   if (*jndex < 0)
		  *jndex = _width + *jndex;
	   else if ( *jndex >= _width)
		  *jndex =  *jndex - _width;
	   }
	else if (PixelOverFlow == eopBorder)
	   {
	   if (*index < 0)
		  *index = 0;
	   else if ( *index >= _height)
		  *index =  _height - 1;
	   if (*jndex < 0)
		  *jndex = 0;
	   else if ( *jndex >= _width)
		  *jndex =  _width - 1;
	   }
	else   //eopSkip
	   return false;
	return true;
	}
protected :
  // Internal data :
  T ** _data;
  //void *_contiguous_memory;
  void *_original_buffer;
  int _width;
  int _height;
  unsigned int _pad;
  unsigned int _scanlineAlignment;
  unsigned int _stride;
  bool _shared;
  TRect _roi;
  speOrientation _orientation;
  // Internal methods
  //------------------------------
  void initVars(void)
	{
	_pad = 0;
	_scanlineAlignment = 0;
	_stride = 0;
	_original_buffer = 0;
	//_contiguous_memory = 0;
	_shared = false;
	_orientation = orAsIs;
	PixelOverFlow = eopBorder;
	}
  //------------------------------
  void calculateAlignement(void)
	{
	// _align and _scanlineAlignemnt must be preset
	_stride = _width * sizeof(T);
	if (_scanlineAlignment > 0)
	   _pad = _stride % _scanlineAlignment;         // alignemnt
	if (_pad > 0) _pad =  _scanlineAlignment - _pad;
	_stride = _stride + _pad;   				    // stride
    }
  //------------------------------
  inline void* getScanline(void *buffer, int i)
	{
	return static_cast<void*>(static_cast<Byte*>(_original_buffer) + i * _stride);
	}
  //------------------------------
  void _allocateMemory(bool contiguous, int pix_size)
	{
	const int n = _width*_height;
	//const size_t pix_size = sizeof(T);
	if (contiguous) // contiguous memory
	   {
	   //_contiguous_memory = static_cast<T*>(GetMemory(_stride * pix_size + n*_align));
	   //_contiguous_memory = static_cast<T*>(GetMemory(_stride * _height));
	   _original_buffer = GetMemory(_stride * _height);
	   if (_orientation == orAsIs)
		  {
		  for (int i = 0; i < _height; i++)
			  _data[i] =  static_cast<T*>(getScanline(_original_buffer, i));
		  }
	   else
		  {
		  for (int i = _height - 1 ; i >= 0; i--)
			  _data[i] =  static_cast<T*>(getScanline(_original_buffer, _height - i - 1));
		  }
		   //_data[i] =  static_cast<T*>((void*)((Byte*)_original_buffer + i * _stride)); <- ok
		   //_data[i] = (_contiguous_memory + (i * _width) + _align);  // ako je float pušimo s ovim _align!!!
	   //_original_buffer = (void *)_contiguous_memory;
	   }
	else 	// discontiguous memory
	   {
	   //_contiguous_memory = 0;
	   _original_buffer = 0;
	   if (_orientation == orAsIs)
		  {
		   for (int i = 0; i < _height; i++)
			   _data[i] = static_cast<T*>(GetMemory(_width * pix_size +_pad));
		  }
	   else
		  {
		  for (int i = _height - 1 ; i >= 0; i--)
			   _data[i] = static_cast<T*>(GetMemory(_width * pix_size +_pad));
		  }
	   }
	}
  //------------------------------
  void _freeMemory(void)
	{
	if (!_data)
	   return;
	if (_shared)
	   {
	   for (int i = 0; i < _height; i++)
		   _data[i] = 0;
	   }
	else if (_original_buffer)
	   {
	   for (int i = 0; i < _height; i++)
		   _data[i] = 0;
	   FreeMemory(_original_buffer);
       _original_buffer = 0;
	   }
	/*
	else if (_contiguous_memory)
	   {
	   for (int i = 0; i < _height; i++)
		   _data[i] = 0;
	   FreeMemory(_contiguous_memory);
	   }
    */
	else
	   {
	   for (int i = 0; i < _height; i++)
		   {
		   FreeMemory(_data[i]);
		   _data[i] = 0;
		   }
	   }
	delete [] _data;
	_data = 0;
	//_contiguous_memory = 0;
	_width = _height = 0;
	}
  //---------------------------------
  void fillDataFromBuffer(void)
	{
	void *currPtr = _original_buffer;
	if (_orientation == orAsIs)
	   {
	   for (int i = 0; i < _height; i++)
		   {
		   _data[i] = static_cast <T*>(currPtr);
		   currPtr = static_cast<void*>((Byte*)currPtr + _stride);
		   }
	   }
	else
	   {
	   for (int i = _height-1; i >= 0; i--)
		   {
		   _data[i] = static_cast <T*>(currPtr);
		   currPtr = static_cast<void*>((Byte*)currPtr + _stride);
		   }
	   }
	}
  //---------------------------------
  void constructFromScanLine(void **scanLineData, int width, int height, bool shared = true)
	{
	_width = width;
	_height = height;
	_shared = shared;
	_pad = 0;
	_scanlineAlignment = 0;
	//_contiguous_memory = 0;
	_original_buffer = 0;
	calculateAlignement();
	if (!_shared)
	   {
	   _data = new T*[_height];
	   _allocateMemory(false, sizeof(T));	// always dis-contiguous if not shared
	   }
	else
	   _data = new T*[_height];
	for (int i = 0; i < _height; i++)
		{
		if (_shared)
		   _data[i] = static_cast <T*>(scanLineData[i]);
		else
		   memcpy(_data[i], static_cast<T*>(scanLineData[i]), _width * sizeof(T));
		}
	}
  //---------------------------------
  void constructFromBuffer(void *buffer, int width, int height, unsigned int scanlineAlignment, bool shared = true, speOrientation orientation = orAsIs)
	{
	_width = width;
	_height = height;
	_shared = shared;
	_pad = 0;
	_scanlineAlignment = scanlineAlignment;
	//_contiguous_memory = 0;
	_original_buffer = 0;
	_orientation = orientation;
	calculateAlignement();
	if (!_shared)
	   {
	   _original_buffer = GetMemory(_stride * _height);
	   memcpy(_original_buffer, buffer, _stride * _height);
		_data = new T*[_height];
	   //_allocateMemory(true, sizeof(T));	// always contiguous
	   }
	else
	   {
	   _original_buffer = buffer;
	   _data = new T*[_height];
	   }
	fillDataFromBuffer();
	}
  //---------------------------------
  void checkRoiBoundaries(void)
	{
	if (_roi.top < 0)
		_roi.top =  0;
	if (_roi.left < 0)
		_roi.left = 0;
	if (_roi.bottom > _height)
		_roi.bottom = _height;
	if (_roi.right > _width)
	   _roi.right = _width;
	}
};
#endif // SP_IMAGE_H

