// Code for visualizing ascii elevation data

#pragma once

#include "resource.h"
#include "framework.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "KorkeusDEMOCPP.h"

using namespace std;

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