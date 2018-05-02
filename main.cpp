#include "image.h"
#include <cstdio>
#include <cstdlib>
#include <math.h>

struct RenderingContext {
	double x;
	double y;
	double scale;
	unsigned int max_iter;
	unsigned int x_px;
	unsigned int y_px;
	ColorScheme cs;
	Color in_color;
};

void make_frame(RenderingContext* ctx, const char* fmt, int frame_num) {
	double x_scale = ctx->scale;
	double y_scale = ctx->scale*1.0*(ctx->y_px*1.0/ctx->x_px);
	double x_offset = ctx->x - x_scale/2;
	double y_offset = ctx->y - y_scale/2;

	unsigned int* hist = (unsigned int*)calloc(ctx->max_iter+1, sizeof(unsigned int));
	memset(hist, 0, (ctx->max_iter+1) * sizeof(unsigned int));
	unsigned int* image = (unsigned int*)calloc(ctx->x_px*ctx->y_px, sizeof(unsigned int));

	printf("Starting image %d\n", frame_num);
	fflush(stdout);
	for(int i = 0; i < ctx->x_px; ++i) {
		for(int j = 0; j < ctx->y_px; ++j) {
			double x0 = (i*1.0/ctx->x_px)*x_scale + x_offset;
			double y0 = (j*1.0/ctx->y_px)*y_scale + y_offset;
			double x  = 0.0;
			double y  = 0.0;
			int iter = 0;
			while(x*x + y*y < 4 && iter < ctx->max_iter) {
				double xtemp = x*x - y*y + x0;
				double ytemp = fabs(2*x*y) + y0;
				if (x == xtemp && y == ytemp) {
					iter = ctx->max_iter;
					break;
				}
				x = fabs(xtemp);
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
			int iter = image[(i+j*ctx->x_px)];
			if (iter == ctx-> max_iter) {
				img.setColor(i, j, ctx->in_color);
			} else {
				img.setColor(i, j, ctx->cs.getColor(hist[iter]*1.0/(ctx->x_px*ctx->y_px)));
			}
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
	//Color c0, c1, c2, c3, c4;
	//c0.r = 0; c0.g = 7; c0.b = 100;
	//c1.r = 32; c1.g = 107; c1.b = 203;
	//c2.r = 237; c2.g = 255; c2.b = 255;
	//c3.r = 255; c3.g = 170; c3.b = 0;
	//c4.r = 0; c4.g = 2; c4.b = 0;
	//ColorScheme cs;
	//cs.addColor(c0, 0);
	//cs.addColor(c1, 0.16);
	//cs.addColor(c2, 0.42);
	//cs.addColor(c3, 0.6425);
	//cs.addColor(c4, 0.8575);
	//cs.addColor(c0, 1);

	//ColorScheme cs;
	//cs.addColor(Color(0xfff7f3), 9.0/9);
	//cs.addColor(Color(0xfde0dd), 8.0/9);
	//cs.addColor(Color(0xfcc5c0), 7.0/9);
	//cs.addColor(Color(0xfa9fb5), 6.0/9);
	//cs.addColor(Color(0xf768a1), 5.0/9);
	//cs.addColor(Color(0xdd3497), 4.0/9);
	//cs.addColor(Color(0xae017e), 3.0/9);
	//cs.addColor(Color(0x7a0177), 2.0/9);
	//cs.addColor(Color(0x49006a), 1.0/9);
	//cs.addColor(Color(0x000000), 0.0/9);
	
	ColorScheme cs;
	cs.addColor(Color(0x000000), 0);
	cs.addColor(Color(0xbb2200), 0.8);
	cs.addColor(Color(0xff7700), 1);

	RenderingContext ctx;
	//ctx.x = -1.7590170270659;
	//ctx.y = 0.01916067191295;
	//ctx.scale = 1.1e-12;
	ctx.x = -1.75;
	ctx.y = -0.035;
	ctx.scale = 0.2;
	ctx.max_iter = 20000;
	ctx.x_px = 1920;
	ctx.y_px = 1080;
	ctx.cs = cs;
	ctx.in_color = Color();
	make_frame(&ctx, "test.png", 0);
}
