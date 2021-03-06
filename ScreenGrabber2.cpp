// ScreenGrabber2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

#include "ScreenGrabber2.h"

#include <iostream>
#include <algorithm>
#include <vector>

void ScreenGrabber2::BlitScreen()
{
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	hdc			= GetDC(0);
	hCaptureDC	= CreateCompatibleDC(hdc);
	hBitmap		= CreateCompatibleBitmap(hdc, screenWidth, screenHeight);
	hOld		= SelectObject(hCaptureDC, hBitmap);
	
	BitBlt(hCaptureDC, 0, 0, screenWidth, screenHeight, hdc, 0, 0, SRCCOPY | CAPTUREBLT);

	SelectObject(hCaptureDC, hOld);
	DeleteDC(hCaptureDC);
}

void ScreenGrabber2::GetPixelData()
{
	headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	imageSize = screenWidth * screenHeight * 4;

	buffer.resize(imageSize + headerSize);

	// ADD THE FILE HEADER TO THE DATA
	BITMAPFILEHEADER* fileHeader = (BITMAPFILEHEADER*)&buffer[0];
	memset(fileHeader, 0, sizeof(BITMAPFILEHEADER));

	fileHeader->bfType = 0x4d42;
	fileHeader->bfSize = imageSize + headerSize;
	fileHeader->bfReserved1 = 0;
	fileHeader->bfReserved2 = 0;
	fileHeader->bfOffBits = headerSize;

	// ADD THE INFO HEADER TO THE DATA
	BITMAPINFO* bitmapInfo = (BITMAPINFO*)&buffer[sizeof(BITMAPFILEHEADER)];
	memset(bitmapInfo, 0, sizeof(BITMAPINFOHEADER));

	bitmapInfo->bmiHeader.biHeight = screenHeight;
	bitmapInfo->bmiHeader.biWidth = screenWidth;
	bitmapInfo->bmiHeader.biPlanes = 1;
	bitmapInfo->bmiHeader.biBitCount = 24;
	bitmapInfo->bmiHeader.biSizeImage = imageSize;
	bitmapInfo->bmiHeader.biCompression = BI_RGB;



	BITMAPINFO MyBMInfo = { 0 };
	MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

	GetDIBits(hdc, hBitmap, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS);

	lpPixels = (BYTE*)&buffer.begin()[headerSize];

	MyBMInfo.bmiHeader.biCompression = BI_RGB;

	GetDIBits(hdc, hBitmap, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)lpPixels, &MyBMInfo, DIB_RGB_COLORS);

	//imageSize = MyBMInfo.bmiHeader.biSizeImage;

}

void ScreenGrabber2::ScreenCapture()
{
	BlitScreen();
	GetPixelData();
	/*
	// VECTOR WHICH HOLDS THE FINAL IMAGE
	bmpData.resize(imageSize + headerSize);
	//bmpData.assign(lpPixels, lpPixels + 5);
	//std::vector<char> bmpData(imageSize);

	// ADD THE FILE HEADER TO THE DATA
	BITMAPFILEHEADER* fileHeader = (BITMAPFILEHEADER*)&bmpData[0];
	memset(fileHeader, 0, sizeof(BITMAPFILEHEADER));

	fileHeader->bfType = 0x4d42;
	fileHeader->bfSize = imageSize + headerSize;
	fileHeader->bfReserved1 = 0;
	fileHeader->bfReserved2 = 0;
	fileHeader->bfOffBits = headerSize;

	// ADD THE INFO HEADER TO THE DATA
	BITMAPINFO* bitmapInfo = (BITMAPINFO*)&bmpData[sizeof(BITMAPFILEHEADER)];
	memset(bitmapInfo, 0, sizeof(BITMAPINFOHEADER));

	bitmapInfo->bmiHeader.biHeight = screenHeight;
	bitmapInfo->bmiHeader.biWidth = screenWidth;
	bitmapInfo->bmiHeader.biPlanes = 1;
	bitmapInfo->bmiHeader.biBitCount = 24;
	bitmapInfo->bmiHeader.biSizeImage = imageSize;
	bitmapInfo->bmiHeader.biCompression = BI_RGB;

	int i = headerSize;
	while (i < imageSize + headerSize)
	{
		bmpData[i++] = lpPixels[i];
	}
	*/


	//CLEAN-UP
	DeleteObject(hBitmap);
	ReleaseDC(NULL, hdc);
	delete[] lpPixels;
}

int main()
{
	ScreenGrabber2 grabber;
	grabber.ScreenCapture();
	return 0;
}

inline int GetBitmapBytes(int width, int height, int depth)
{
	return height * ((width * (depth == 32 ? 4 : 3) + 3) & ~3);
}