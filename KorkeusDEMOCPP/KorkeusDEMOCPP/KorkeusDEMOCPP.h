// Code for visualizing ascii elevation data

#pragma once

#include "resource.h"
#include "framework.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

// Holds metadata about the map
class MapMetadata {
public:
	int mapwidth = 0;
	int mapheight = 0;
	int nodatavalue = 0;

	long MapSize() { return (mapwidth * mapheight); };
};

// Holds an array of RGB values
class MapRGBData {
public:
	unsigned char* rgbdata;
	void Initialize(int koko) { rgbdata = new unsigned char[koko * 3]; };
	~MapRGBData() { delete[] rgbdata; };
};

// Holds a necessary bitmap header
// Also has function for outputting picture
class BitmapData {
	int width, height;
	void* pMap;
	HBITMAP bitmapHandle;
	HDC hdc;

public:
	BITMAPINFOHEADER mapHeader;
	BITMAPINFO mapInfo;

	BitmapData(HDC thdc, HBITMAP thandle, void* tpMap, int twidth, int theight) {
		width = twidth;
		height = theight;
		pMap = tpMap;
		hdc = thdc;
		bitmapHandle = thandle;

		mapHeader = {
		   sizeof(BITMAPINFOHEADER),	//bV5Size
		   width,		//bV5Width
		   -height,		//bV5Height
		   1,			//bV5Planes
		   24,			//bV5BitCount
		   BI_RGB,		//bV5Compression
		   0,			//bV5SizeImage
		   0,			//bV5XPelsPerMeter
		   0,			//bV5YPelsPerMeter
		   0,			//bV5ClrUsed
		   0,			//bV5ClrImportant
		};

		mapInfo = {
			mapHeader
		};
	};

	int DIBtoDDB() {
		int asdf = SetDIBits(hdc, bitmapHandle, 1, height, pMap, &mapInfo, DIB_PAL_COLORS);
		if (asdf == 0) {
			::MessageBox(NULL, __T("Failed to set DIBits."), __T("Error"), MB_OK);
			return NULL;
		}
		else return asdf;
	};
};

// Calculates elevation data into RGB values
void CalculateRGBValues(vector<double>&, unsigned char*&, MapMetadata&);

// Main function for opening and handling the file
// From string to tokenized float to RGB
// Target file hardcoded, for now
bool FileIntoRGB(MapRGBData&, MapMetadata&);