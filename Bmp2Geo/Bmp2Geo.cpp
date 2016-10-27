/*
* LICENCE
* copyright 2016 ~ ****
* Some rights reserved.
* Author: HUFANGYUAN
* Released under CC BY-NC
*/
// Bmp2Geo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>
using namespace std;

unsigned char* ReadBMP(char* filename, int& width, int& height) {
	FILE* f;
	fopen_s(&f, filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	// extract image height and width from header
	width = *(int*)&info[18];
	height = *(int*)&info[22];

	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);

	for (int i = 0; i < size; i += 3) {
		unsigned char tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}

	return data;
}

unsigned char* ReadBMP_padded(char* filename, int& width, int& height) {
	FILE* f;
	fopen_s(&f, filename, "rb");
	if (f == NULL) throw "Argument Exception";
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	// extract image height and width from header
	width = *(int*)&info[18];
	height = *(int*)&info[22];

	int row_padded = (width * 3 + 3) & (~3);
	unsigned char* data_row = new unsigned char[row_padded];
	unsigned char* data = new unsigned char[3 * width* height];

	for (int i = 0; i < height; i++) {
		fread(data_row, sizeof(unsigned char), row_padded, f);
		for (int j = 0; j < width; j ++) {
			// Convert (B, G, R) to (R, G, B)
			unsigned char b = data_row[3*j];
			unsigned char g = data_row[3*j + 1];
			unsigned char r = data_row[3*j + 2];
			const int id = 3*(width*i + j);
			data[id] = r;
			data[id + 1] = g;
			data[id + 2] = b;
		}
	}
	delete[] data_row;
	fclose(f);
	return data;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int width = 0, height = 0;
	int NP = 0;
	double DP;
	
	cout << " input initial particle spacing: " << endl;
	cin >> DP;
	const unsigned char* const image = ReadBMP_padded("Geo.bmp", width, height);
	cout << " width: " << width << ", height: " << height << endl;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			const int id = 3 * (i*width + j);
			const unsigned char r = image[id];
			const unsigned char g = image[id + 1];
			const unsigned char b = image[id + 2];
			if (r != 0xFF || g != 0xFF || b != 0xFF) NP++;
		}
	}

	ofstream file("Geo.in", ofstream::out);
	file << scientific << setprecision(6) << 0 << endl;
	file << scientific << setprecision(6) << DP << endl;
	file << NP << endl;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			const int id = 3 * (i*width + j);
			const unsigned char r = image[id];
			const unsigned char g = image[id+1];
			const unsigned char b = image[id+2];
			if (r == 0xFF && g == 0xFF && b == 0xFF) continue;
			const double x = DP* j;
			const double y = DP* i;
			///fluid
			if (r == 0x00 && g == 0x00 && b == 0xFF) {
				file << 0 << std::scientific << std::setprecision(6) << " " << x << " " << y << " " << 0.0 << " " << 0.0 << " " << 0.0 << endl;
			}
			///wall
			else if (r == 0xFF && g == 0x00 && b == 0x00) {
				file << 1 << std::scientific << std::setprecision(6) << " " << x << " " << y << " " << 0.0 << " " << 0.0 << " " << 0.0 << endl;
			}
			///dummy
			else if (r == 0x00 && g == 0x00 && b == 0x00) {
				file << 2 << std::scientific << std::setprecision(6) << " " << x << " " << y << " " << 0.0 << " " << 0.0 << " " << 0.0 << endl;
			}
			///inlet
			else if (r == 0xFF && g == 0xFF && b == 0x00) {
				file << 3 << std::scientific << std::setprecision(6) << " " << x << " " << y << " " << 0.0 << " " << 0.0 << " " << 0.0 << endl;
			}
			///outlet
			else if (r == 0x00 && g == 0xFF && b == 0xFF) {
				file << 4 << std::scientific << std::setprecision(6) << " " << x << " " << y << " " << 0.0 << " " << 0.0 << " " << 0.0 << endl;
			}
		}
	}
	file.close();
	std::cout << " Writing Geo. done. " << std::endl;
	
	return 0;
}

