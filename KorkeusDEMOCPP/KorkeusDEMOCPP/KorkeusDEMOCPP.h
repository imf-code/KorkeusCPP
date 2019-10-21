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
void CalculateRGBValues(vector<double>& maparray, unsigned char*& rgbarray, MapMetadata& metadata) {

	// Calculate RGB values based on the max range of elevations
	double minValue = *min_element(maparray.begin(), maparray.end());
	double maxValue = *max_element(maparray.begin(), maparray.end());
	double mapRange = maxValue - minValue;

	int n;
	for (int i = 0; i < metadata.MapSize(); i++) {
		n = i * 3;
		rgbarray[n] = 0;
		rgbarray[n + 1] = (unsigned char)round(((maparray[i] - minValue) / mapRange) * 255);
		rgbarray[n + 2] = 0;
	}
};

// Main function for opening and handling the file
// From string to tokenized float to RGB
// Target file hardcoded, for now
bool FileIntoRGB(MapRGBData& mapRGB, MapMetadata& meta) {

	// Open file
	ifstream mapfile(".\\files\\L3324.asc");
	if (mapfile.is_open()) {

		// String stream
		stringstream mapstring;
		mapstring << mapfile.rdbuf();

		// Close file
		mapfile.close();

		// Read metadata
		// Horrible hack version
		std::string* ignore = new std::string;
		for (int i = 0; i < 13; i++) {
			switch (i) {
			case 1:
				mapstring >> meta.mapwidth;
				break;
			case 3:
				mapstring >> meta.mapheight;
				break;
			case 11:
				mapstring >> meta.nodatavalue;
				break;
			default:
				mapstring >> *ignore;
			}
		}
		delete ignore;

		// Tokenization + conversion to float
		// Quite hacky
		vector<double> mapElevationData;
		char* temp = new char[10];

		for (int i = 0; i < meta.MapSize(); i++) {
			mapstring >> temp;
			mapElevationData.push_back(atof(temp));
		}
		delete[] temp;
		mapstring.str("");

		// Calculate RGB values
		mapRGB.Initialize(meta.MapSize());
		CalculateRGBValues(mapElevationData, mapRGB.rgbdata, meta);
		return true;
	}
	else
	{
		return false;
	}
};