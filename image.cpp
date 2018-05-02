#include "image.h"

void ColorScheme::addColor(Color c, float pos) {
	int i;
	ColorSchemeColor csc;
	csc.c = c;	
	csc.pos = pos;
	for(i = 0; i < colors.size() && colors.at(i).pos < pos; ++i);
	colors.insert(colors.begin() + i, csc);
}

double lerp(double a, double b, double f){ return (a * (1.0 - f)) + (b * f); }
Color lerp(Color* a, Color* b, double f){
	Color ret;
	ret.r = lerp(a->r, b->r, f);
	ret.g = lerp(a->g, b->g, f);
	ret.b = lerp(a->b, b->b, f);
	return ret;
}

Color ColorScheme::getColor(float pos) {
	int i;
	for(i = 0; i < colors.size() && colors.at(i).pos < pos; ++i);
	ColorSchemeColor a = colors.at(i);
	ColorSchemeColor b = colors.at(i == 0 ? 0 : i - 1);
       	return lerp(&(a.c), &(b.c), (pos - a.pos)/(b.pos - a.pos));  	
}

Image::Image(int w, int h){
	width = w;
	height = h;
	img = (unsigned char*)calloc(width*height*3, sizeof(char));
}

Image::~Image(){
	delete img;
}

void Image::writePNG(char* filename){
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	int rowsize;
	png_bytep row;

	fp = fopen(filename, "wb");
	if(fp == NULL) goto finalise;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) goto finalise;

	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL) goto finalise;

	if(setjmp(png_jmpbuf(png_ptr))) goto finalise;

	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	
	rowsize = 3*width*sizeof(png_byte);
	row = (png_bytep) malloc(rowsize);

	for(int y = 0; y < height; y++) {
		memcpy(row, img+y*rowsize, rowsize);
		png_write_row(png_ptr, row);
	}

	png_write_end(png_ptr, NULL);

finalise:
	if(fp != NULL) fclose(fp);
	if(png_ptr != NULL) png_destroy_write_struct(&png_ptr, &info_ptr);
	if(row != NULL) free(row);
}

void Image::clearImage(){
	memset(img, 0, width*height*3);
}

void Image::setColor(int x, int y, Color c){
	img[(x+y*width)*3] = c.r;
	img[(x+y*width)*3+1] = c.g;
	img[(x+y*width)*3+2] = c.b;
}

void Image::setShade(int x, int y, unsigned char shade){
	memset(img + (x+y*width)*3, shade, 3);
}

void Image::setShade(int x, int y, float shade){
	setShade(x, y, (unsigned char)floor(shade*255));
}

void Image::setShade(int x, int y, double shade){
	setShade(x, y, (float)shade);
}
