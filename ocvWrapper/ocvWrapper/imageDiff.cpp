#include "stdafx.h"
#include "imageDiff.h"
using namespace cv;
typedef unsigned char Byte;
// constructor
TImageDiff::TImageDiff(Mat *source, Mat *target, Mat *targetAlpha)
{
	FSource = source;
	FTarget = target;
	FTargetAlpha = targetAlpha;
	FSrcHeight = FSource->rows;
	FSrcWidth  = FSource->cols;
	FTgtHeight = FTarget->rows;
	FTgtWidth  = FTarget->cols;
	FOutData = new int **[FTgtHeight];
	for (int i = 0; i < FTgtHeight; i++)
	{
		FOutData[i] = new int *[FTgtWidth];
		for (int j = 0; j < FTgtWidth; j++)
		{
			FOutData[i][j] = new int [arrElems];
		for (int k = 0; k < 4; k++)
			FOutData[i][j][k] = 0;
		}
   }
	std::srand((int)std::time(0));
}
// destructor
TImageDiff::~TImageDiff()
{
	if (FOutData)
	{
		for (int i = 0; i < FTgtHeight; i++)
	    {
			for (int j = 0; j < FTgtWidth; j++)
				delete []FOutData[i][j];
		   delete []FOutData[i];
		}
		delete []FOutData;
   }
}
//-------------------------------------------------------------------------------------------------
// init outdata
//---------------------------------------------------------------------------
void TImageDiff::initOutData(int dia)
{
int mate = INT_MAX;
for (int y = 0; y < FTgtHeight; y++)
	{
	for (int x = 0; x < FTgtWidth; x++)
		{
		int dx = x;
		int dy = y;
		//ranmdomize inside probe diameter if pixel went across the border
		if (dx >= FTgtWidth)
		   dx = std::rand()%dia + FTgtWidth - dia;
		if (dy >= FTgtHeight)
		   dy = std::rand()%dia + FTgtHeight - dia;
		FOutData[y][x][idY] = dy;
		FOutData[y][x][idX] = dx;
		FOutData[y][x][idError] = INT_MAX;
		FOutData[y][x][msError] = 0;
		FOutData[y][x][csError] = 1;	// small error count
		}
	}
}
//---------------------------------------------------------------------------
// distance
int TImageDiff::distance(int &sx, int &sy, int &tx, int &ty)
{
	// actually quadrance
	int diff, d = 0;
	Byte *sptr = FSource->ptr<Byte>(sy, sx);
	Byte *tptr = FTarget->ptr<Byte>(sy, sx);
	for (int k = 0; k < 3; k++)     // tri kanala rgb24
	   {
	   diff = (int)sptr[k] - (int)tptr[k];
	   d += diff * diff;
	   }
	return d;
}
//---------------------------------------------------------------------------
// pixel dithering
void TImageDiff::pixelDT(int &sx, int &sy, int &tx, int &ty, int &radius, int &small_error, int &max_sec, int &min_sec)
{
	int y1 = ty - radius;
	int y2 = ty + radius;
	int x1 = tx - radius;
	int x2 = tx + radius;
	if (y1 < 0) y1 = 0;
	if (y2 > FTgtHeight) y2 = FTgtHeight;
	if (x1 < 0) x1 = 0;
	if (x2 > FTgtWidth)  x2 = FTgtWidth;
	int d, cumul_d = 0, cumul_size = 0;
	for (int y = y1; y < y2; y++)
		{
		for (int x = x1; x < x2; x++)
			{
			if (FOutData[ty][tx][idError] > 0)
			   {
				d = distance(sx, sy, x, y);
				if (d < FOutData[ty][tx][idError])
				   {
				   FOutData[ty][tx][idY] = y;
				   FOutData[ty][tx][idX] = x;
				   FOutData[ty][tx][idError] = d;
				   }
				cumul_d += d;
				cumul_size++;
				// small error test
				if (d < small_error)
				{
				   FOutData[ty][tx][csError]++;
				   if (FOutData[ty][tx][csError] > max_sec)
					  max_sec = FOutData[ty][tx][csError];
				}
				if (FOutData[ty][tx][csError] < min_sec)
				   min_sec = FOutData[ty][tx][csError];
			   }
			}
		}
	// mse - mean square error (or someting)
	if (cumul_size > 0)
		FOutData[ty][tx][msError] = cumul_d / cumul_size ;
	else
		FOutData[ty][tx][msError] = 0;
}
//---------------------------------------------------------------------------
int*** TImageDiff::ExecuteDT(int patchRadius, int radius, int iterations, int err_tol, int output_threshold, int out_type)
{
	int small_error = err_tol*err_tol;
	int dia = radius<<1;
	initOutData(dia);
	// single pixel ditthering
	int dx, dy;
	int min_sec = INT_MAX, max_sec = 1;
	int tx, ty;
	for (int y = 0; y < FSrcHeight; y++)
		{
		ty = y;
		if (ty >= FTgtHeight) ty = FTgtHeight - radius;
		for (int x = 0; x < FSrcWidth; x++)
			{
			tx = x;
			if (tx >= FTgtWidth) tx = FTgtWidth - radius;
			if (FOutData[ty][tx][idError] > 0)
			   {
				if (FTargetAlpha != 0)
				   {
				   if (FTargetAlpha->at<Byte>(ty, tx) == 0x00)
					  {
					  FOutData[ty][tx][idError] =  0;
					  continue;
					  }
				   }
				for (int i = 0; i < iterations; i++)
					{
					if (i > 0)
					   {
						// dither
						do {
						   dx = tx - radius + std::rand()%dia;
						   } while (dx < 0 || dx >= FTgtWidth);
						do {
						   dy = ty - radius + std::rand()%dia;
						   } while (dy < 0 || dy >= FTgtHeight);
					   }
					else
					   {
					   dx = tx;
					   dy = ty;
					   }
					pixelDT(x, y, dx, dy, patchRadius, small_error, max_sec, min_sec);
					}
			   }
			}
		}
	//
	if ((max_sec - min_sec) < 0)	// equal images
	   return FOutData;
	int min_d = INT_MAX, max_d = 0;
	switch (out_type)
		{
		case 0: // complicated scaling
			  {
			   if ((max_sec - min_sec) == 0)
					max_sec++;
			   float scale_sec;
			   for (int y = 0; y < FTgtHeight; y++)
				   {
				   for (int x = 0; x < FTgtWidth; x++)
					   {
					   scale_sec = 1.0f - (float)(FOutData[y][x][csError] - min_sec)/(float)(max_sec - min_sec);
					   FOutData[y][x][idError] = (int)(FOutData[y][x][idError] *  scale_sec);
					   if (FOutData[y][x][idError] < min_d)
						  min_d = FOutData[y][x][idError];
					   if (FOutData[y][x][idError] > max_d)
						  max_d = FOutData[y][x][idError];
					   }
				   }
			   float scale_d  = (float)max_d - (float)min_d;
			   for (int y = 0; y < FTgtHeight; y++)
				   {
				   for (int x = 0; x < FTgtWidth; x++)
					   {
					   FOutData[y][x][idError] = (int)((float)(FOutData[y][x][idError] - min_d) * 255.0f /scale_d );
					   if (FOutData[y][x][idError] <= output_threshold)
						   FOutData[y][x][idError] = 0;
					   }
				   }

			  }
			break;
		case 1:	 // simple scale on idError
	   		  {
				for (int y = 0; y < FTgtHeight; y++)
					{
						for (int x = 0; x < FTgtWidth; x++)
						{
							if (FOutData[y][x][idError] < min_d)
								min_d = FOutData[y][x][idError];
							if (FOutData[y][x][idError] > max_d)
								max_d = FOutData[y][x][idError];
						}
					}
		  	  for (int y = 0; y < FTgtHeight; y++)
				   {
				   for (int x = 0; x < FTgtWidth; x++)
					   {
						FOutData[y][x][idError] = (int)((float)(FOutData[y][x][idError] - FOutData[y][x][csError]  - min_d) * 255.0f / float (max_d - min_d));
					   // test
					   if (FOutData[y][x][idError] <= output_threshold)
						   FOutData[y][x][idError] = 0;
					   }
				   }   
			  }
			break;
		case 2:	 // simple scale on msError
	   		  {
				for (int y = 0; y < FTgtHeight; y++)
					{
					for (int x = 0; x < FTgtWidth; x++)
					{
						if (FOutData[y][x][msError] < min_d)
							min_d = FOutData[y][x][msError];
						if (FOutData[y][x][msError] > max_d)
							max_d = FOutData[y][x][msError];
						}
					}
		  	  for (int y = 0; y < FTgtHeight; y++)
				   {
				   for (int x = 0; x < FTgtWidth; x++)
					   {
						FOutData[y][x][idError] = (int)((float)(FOutData[y][x][msError] - FOutData[y][x][csError]  - min_d) * 255.0f / float (max_d - min_d));
					   // test
					   if (FOutData[y][x][idError] <= output_threshold)
						   FOutData[y][x][idError] = 0;
					   }
				   }   
			  }
			break;
		default: // no scaling - as is
			break;
		}
	// eliminate isolated pixels
	int ti, tj, pErr, cErr;
	for (int y = 0; y < FTgtHeight; y++)
		{
		for (int x = 0; x < FTgtWidth; x++)
			{
			pErr = FOutData[y][x][idError];
			if (pErr > 0)
			   {
				cErr = 0;
				for (int m = 0; m < 9; m++)
					{
					ti = y + K33i[m];
					tj = x + K33j[m];
					if (ti < 0 || ti >= FTgtHeight || tj < 0 || tj >= FTgtWidth)
						continue;
					if (FTarget->at<Byte>(ti, tj) > 0x00)
					   cErr += FOutData[ti][tj][idError];
				   }
				if (cErr == pErr)	// isolated pixel
				   FOutData[y][x][idError] = 0;
				else
				   cErr = 0;
				}
			}
		}
	return FOutData;
}


