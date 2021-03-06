#ifndef IMAGE_H 
#define IMAGE_H
#include <png.h>
#include <malloc.h>
#include <cstring>
#include <math.h>
#include <vector>

using namespace std;

struct Color {
	unsigned char r, g, b;
	Color(){ r=0; g=0; b=0; }
	Color(unsigned int hexcode){
		r = (hexcode >> 16) & 0xff;
		g = (hexcode >> 8) & 0xff;
		b = hexcode & 0xff;
	}
	Color(unsigned char a, unsigned char c, unsigned char d){
		r = a;
		g = c;
		b = d;
	}
};

struct ColorSchemeColor {
	Color c;
	float pos;
};

class ColorScheme {
public:
	void addColor(Color c, float pos);
	Color getColor(float pos);
private:
	vector<ColorSchemeColor> colors;
};

class Image {
public:
	Image(int width, int height);
	~Image();

	void writePNG(char *filename);

	void clearImage();
	void setColor(int x, int y, Color c);
	void setShade(int x, int y, unsigned char shade);
	void setShade(int x, int y, float shade);
	void setShade(int x, int y, double shade);

private:
	int width, height;
	unsigned char* img;
};

#endif
