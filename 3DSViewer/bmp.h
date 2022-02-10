//bmp.h
#include <windows.h>
#ifndef BMP_H
#define BMP_H



class AUX_RGBImageRec 
{
   void convertBGRtoRGB();
public:
	byte* data;

	DWORD sizeX;
	DWORD sizeY;
   bool NoErrors;
   AUX_RGBImageRec(): NoErrors(false), data(NULL) {};
   AUX_RGBImageRec(const char *FileName);
   ~AUX_RGBImageRec();
   bool loadFile(const char *FileName);
   friend AUX_RGBImageRec *auxDIBImageLoad(const char *FileName);
};
#endif
