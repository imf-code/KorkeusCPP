// Code for visualizing ascii-encoded elevation data

#pragma once

#include "resource.h"
#include "framework.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "KorkeusDEMO.h"

using namespace std;

// Calculates RGB values based on provided elevation data, used by FileIntoRGB()
void CalculateRGBValues(vector<float>& maparray, unsigned char*& rgbarray, MapMetadata& metadata) {

	// Calculate RGB values based on the max range of elevations
	float minValue = *min_element(maparray.begin(), maparray.end());
	float maxValue = *max_element(maparray.begin(), maparray.end());
	float mapRange = maxValue - minValue;

	int n;
	for (int i = 0; i < metadata.mapsize; i++) {
		n = i * 3;
		rgbarray[n] = 0;
		rgbarray[n + 1] = (unsigned char)round(((maparray[i] - minValue) / mapRange) * 255);
		rgbarray[n + 2] = 0;
	}
};

// Main function for opening and handling the file
// open file -> tokenize + convert to float -> convert to RGB bytes
// Target file hardcoded (for now) see TARGET_FILE in KorkeusDEMO.h to change path/file
bool FileIntoRGB(MapRGBData& mapRGB, MapMetadata& meta) {

	// Open file
	ifstream mapfile(TARGET_FILE);
	if (mapfile.is_open()) {

		// String stream
		stringstream mapstring;
		mapstring << mapfile.rdbuf();

		// Close file
		mapfile.close();

		// Read metadata
		// Horrible hackjob; see ParseTheData() in openfile.js in the javascript version for proper implementation
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
		meta.CalculateMapSize();

		// Tokenization + conversion to float
		// Implementation is a bit hacky, but fastest of the bunch I tried
		// Performance throttled by, among other things, running in a single thread
		vector<float> mapElevationData (meta.mapsize);
		char* temp = new char[10];
		for (int i = 0; i < meta.mapsize; i++) {
			mapstring >> temp;
			mapElevationData[i] = (float)atof(temp);
		}
		delete[] temp;
		mapstring.str("");

		// Calculate RGB values
		mapRGB.Initialize(meta.mapsize);
		CalculateRGBValues(mapElevationData, mapRGB.rgbdata, meta);
		return true;
	}
	else
	{
		return false;
	}
};