#include "image.h"
#include <cstdio>
#include <cstdlib>

struct RenderingContext {
	double x;
	double y;
	double scale;
	unsigned int max_iter;
	unsigned int x_px;
	unsigned int y_px;
};

Color c0, c1, c2, c3, c4;
double lerp(double a, double b, double f){ return (a * (1.0 - f)) + (b * f); }
void lerp(Color* out, Color* a, Color* b, double f){
	out->r = lerp(a->r, b->r, f);
	out->g = lerp(a->g, b->g, f);
	out->b = lerp(a->b, b->b, f);
}
Color get_color(double pos) {
	Color ret;
	if (pos < 0.16) {
		lerp(&ret, &c0, &c1, pos/0.16);
	} else if (pos < 0.42) {
		lerp(&ret, &c1, &c2, (pos - 0.16)/(0.42-0.16));
	} else if (pos < 0.6425) {
		lerp(&ret, &c2, &c3, (pos - 0.42)/(0.6425 - 0.42));
	} else if (pos < 0.8575) {
		lerp(&ret, &c3, &c4, (pos - 0.6425)/(0.8575 - 0.6425));
	} else {
		lerp(&ret, &c4, &c0, (pos - 0.8575)/(1.0 - 0.8575));
	}
	return ret;
}

void make_frame(RenderingContext* ctx, const char* fmt, int frame_num) {
	double x_offset = ctx->x - ctx->scale/2;
	double y_offset = ctx->y - ctx->scale/2;

	unsigned int* hist = (unsigned int*)calloc(ctx->max_iter+1, sizeof(unsigned int));
	memset(hist, 0, (ctx->max_iter+1) * sizeof(unsigned int));
	unsigned int* image = (unsigned int*)calloc(ctx->x_px*ctx->y_px, sizeof(unsigned int));

	printf("Starting image %d\n", frame_num);
	fflush(stdout);
	for(int i = 0; i < ctx->x_px; ++i) {
		for(int j = 0; j < ctx->y_px; ++j) {
			double x0 = (i*1.0/ctx->x_px)*ctx->scale + x_offset;
			double y0 = (j*1.0/ctx->y_px)*ctx->scale + y_offset;
			double x  = 0.0;
			double y  = 0.0;
			int iter = 0;
			while(x*x + y*y < 4 && iter < ctx->max_iter) {
				double xtemp = x*x - y*y + x0;
				double ytemp = 2*x*y + y0;
				if (x == xtemp && y == ytemp) {
					iter = ctx->max_iter;
					break;
				}
				x = xtemp;
				y = ytemp;
				++iter;
			}
			image[(i + j*ctx->x_px)] = iter;
			hist[iter] += 1;
		}
		if(i%(ctx->x_px/10) == 0){
			printf("Line %d\n", i);
			fflush(stdout);
		}
	}

	printf("Calculating color map...\n");
	fflush(stdout);
	unsigned int total = 0;
	for(int i = 0; i < ctx->max_iter+1; ++i) {
		total += hist[i];
		hist[i] = total;
	}

	Image img(ctx->x_px, ctx->y_px);
	printf("Starting coloring...\n");
	fflush(stdout);
	for(int i = 0; i < ctx->x_px; ++i) {
		for(int j = 0; j < ctx->y_px; ++j) {
			img.setColor(i, j, get_color(hist[image[(i+j*ctx->x_px)]]*1.0/(ctx->x_px*ctx->y_px)));
		}
	}

	printf("Saving...\n");
	fflush(stdout);
	char buff[1024];
	sprintf(buff, fmt, frame_num);
	img.writePNG(buff);

	delete hist;
	delete image;
}

int main(){
	c0.r = 0; c0.g = 7; c0.b = 100;
	c1.r = 32; c1.g = 107; c1.b = 203;
	c2.r = 237; c2.g = 255; c2.b = 255;
	c3.r = 255; c3.g = 170; c3.b = 0;
	c4.r = 0; c4.g = 2; c4.b = 0;

	RenderingContext ctx;
	ctx.x = 0.432539867562512;
	ctx.y = 0.226118675951765;
	ctx.scale = 3.2e-6;
	ctx.max_iter = 20000;
	ctx.x_px = 1024;
	ctx.y_px = 1024;
	make_frame(&ctx, "test.png", 0);
}
