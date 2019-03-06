// Copyright (C) 2011 Pierre Moulon
// Modifications: 2016 Siniša Petriæ
//
#ifndef SP_PIXELTYPES_H_
#define SP_PIXELTYPES_H_

#include <iostream>
using namespace std;
// 1 - channel templated pixel type
template <class T> class spCh1Pixel
  {
  public:
  T ch[1];
  //------------------------------
  spCh1Pixel()
	{
	ch[0] = T(0);
	}
  //------------------------------
  spCh1Pixel(T value)
	{
	ch[0] = value;
	}
  template<class Z>
  inline spCh1Pixel operator / (const Z & val) const
	{
	return spCh1Pixel<T>((T((Z)(ch[0])) / val));
	}
  template<class Z>
  inline spCh1Pixel operator * (const Z & val) const
	{
	return spCh1Pixel<T>(T((Z)(ch[0]) * val));
	}
  template<class Z>
  inline spCh1Pixel operator + (const Z & val) const
	{
	return spCh1Pixel<T>(T((Z)(ch[0]) + val));
	}
  inline spCh1Pixel operator + (const spCh1Pixel<T> & val) const
	{
	return spCh1Pixel<T>(T(ch[0] + val.ch[0]));
	}
  inline spCh1Pixel operator ^ (const spCh1Pixel<T> & val) const
	{
	return spCh1Pixel<T>(T(ch[0] ^ val.ch[0]));
	}
  inline spCh1Pixel operator << (const unsigned int &val) const
	{
	return spCh1Pixel<T>(T(ch[0]<<val));
	}
  inline spCh1Pixel operator >> (const unsigned int &val) const
	{
	return spCh1Pixel<T>(T(ch[0]>>val));
	}
  template<class Z>
  inline bool operator == (const Z & val)  const
	{
	return (ch[0] == val.ch[0]);
	}
  template<class Z>
  inline bool operator > (const Z & val)  const
	{
	return (ch[0] > val.ch[0]);
	}
  template<class Z>
  inline bool operator >= (const Z & val) const
	{
	return (ch[0] >= val.ch[0]);
	}
  };
// 3 - channels templated pixel type (BGR)
template <class T> class spCh3Pixel
  {
  public:
	 T  ch[3];
  //------------------------------
  spCh3Pixel()
	{
	ch[0] = ch[1] = ch[2] = T(0);
	}
  //------------------------------
  spCh3Pixel(T red, T green, T blue)
	{
	ch[2] = red;
	ch[1] = green;
	ch[0] = blue;
	}
  //explicit Ch3Pixel(const Base & val) : Base(val) {}
  explicit inline spCh3Pixel(const T t[3])
	{
	ch[0] = t[0];
	ch[1] = t[1];
	ch[2] = t[2];
	}
  explicit inline spCh3Pixel(const Byte val)
	{
	ch[0] = ch[1] = ch[2] = val;
	}
  explicit inline spCh3Pixel(const int *valRGB)
	{
	ch[0] = T(valRGB[2]);
	ch[1] = T(valRGB[1]);
	ch[2] = T(valRGB[0]);
	}
  explicit inline spCh3Pixel(const unsigned int val)
	{
	ch[0]  = T((val<<8)>>24);
	ch[1]  = T((val<<16)>>24);
	ch[2]  = T((val<<24)>>24);
    }
  // Return gray
  inline operator T() const { return T(0.3*ch[2]+0.59*ch[1]+0.11*ch[0]);}

  template<class Z>
  inline spCh3Pixel operator / (const Z & val) const
  {
	return spCh3Pixel<T>(T((Z)(ch[2]) / val),
					T((Z)(ch[1]) / val),
					T((Z)(ch[0]) / val) );
  }

  template<class Z>
  inline spCh3Pixel operator * (const Z & val) const
  {
	return spCh3Pixel<T>(T((Z)(ch[2]) * val),
					T((Z)(ch[1]) * val),
					T((Z)(ch[0]) * val) );
  }
  template<class Z>
  inline spCh3Pixel operator + (const Z & val) const
  {
	return spCh3Pixel<T>(T((Z)(ch[2]) + val),
					T((Z)(ch[1]) + val),
					T((Z)(ch[0]) + val) );
  }
  inline spCh3Pixel operator + (const spCh3Pixel<T> & val) const
  {
	return spCh3Pixel<T>(T(ch[2] + val.ch[2]),
					T(ch[1] + val.ch[1]),
					T(ch[0] + val.ch[0]));
  }
  inline spCh3Pixel operator ^ (const spCh3Pixel<T> & val) const
  {
	return spCh3Pixel<T>(T(ch[2] ^ val.ch[2]),
					T(ch[1] ^ val.ch[1]),
					T(ch[0] ^ val.ch[0]));
  }
  inline spCh3Pixel operator << (const unsigned int &val) const
	{
	return spCh3Pixel<T>(T(ch[2]<<val),
						 T(ch[1]<<val),
						 T(ch[0]<<val));
	}
  inline spCh3Pixel operator >> (const unsigned int &val) const
	{
	return spCh3Pixel<T>(T(ch[2]>>val),
						 T(ch[1]>>val),
						 T(ch[0]>>val));
	}
};
typedef spCh3Pixel<unsigned char> ptRGB;
typedef spCh1Pixel<unsigned char> ptGray;
typedef spCh1Pixel<float> ptFloat;
typedef spCh3Pixel<float> ptFloat3;
/*
#define chRed ch[2]
#define chGreen ch[1]
#define chBlue ch[0]
*/
/*
const spRGB  WHITE(255,255,255);
const spRGB  BLACK(0,0,0);
const spRGB  BLUE(0,0,255);
const spRGB  RED(255,0,0);
const spRGB  GREEN(0,255,0);
const spRGB  YELLOW(255,255,0);
const spRGB  CYAN(0,255,255);
const spRGB  MAGENTA(255,0,255);
*/

#endif

