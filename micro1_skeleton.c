#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


#include <math.h>
#include <stdio.h>
#include <float.h>


void mirror_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void grayScale_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void sobelFiltering_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);

int main()
{
 
	int height;
	int width;
	int channel;

	char command;

 	printf("Take a picture? (y/n)\n");
 	scanf("%c", &command);

 	if(command == 'n')
 		exit(1);


 	printf("Cheeze !\r\n");
	system("libcamera-still --width 640 --height 480 -o image.bmp");
 	
 	
 	unsigned char* imgIn = stbi_load("image.bmp", &width, &height, &channel, 3);


	unsigned char* imgOut_mirror = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
	unsigned char* imgOut_grayScale = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
	unsigned char* imgOut_sobelFiltering = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
 	
	mirror_transform(imgIn, height, width, channel, imgOut_mirror);
	grayScale_transform(imgIn, height, width, channel, imgOut_grayScale);
	sobelFiltering_transform(imgOut_grayScale, height, width, channel, imgOut_sobelFiltering);


	stbi_write_bmp("image_mirror.bmp", width, height, channel, imgOut_mirror);
	stbi_write_bmp("image_grayScale.bmp", width, height, channel, imgOut_grayScale);
	stbi_write_bmp("image_sobelFiltering.bmp", width, height, channel, imgOut_sobelFiltering);
	
	stbi_image_free(imgIn);
	free(imgOut_mirror);
	free(imgOut_grayScale);
	free(imgOut_sobelFiltering);
 

	return 0;
}

void mirror_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	int x, y, c;

	for(y=0; y<height; y++)
		for(x=0; x<width; x++)
			for(c=0; c<channel; c++)
				out[y*640*3 + (639-x)*3 + c]=in[y*640*3 + x*3 + c];
}

void grayScale_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	int x, y, c;

	for(y=0; y<height; y++)
		for(x=0; x<width; x++)
			for(c=0; c<channel; c++)
				out[y*640*3 + x*3+c]=(in[y*640*3+x*3+0] + in[y*640*3+x*3+1] + in[y*640*3+x*3+2])/3;
}

void sobelFiltering_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	int x,y,c,i,j;
	int sumX, sumY;
	int maskSobelX[3][3]=
						{{ -1, 0, 1},
						 { -2, 0, 2},
						 { -1, 0, 1}};
	int maskSobelY[3][3]=
						{{ -1, -2, -1},
						 {  0,  0,  0},
						 {  1,  2, 1}};

	unsigned int newValue;

	for(y=1; y<(height-1); y++) {
		for(x=1; x<(width-1); x++) {
			sumX = 0;
			sumY = 0;
			for(i=0; i<3; i++) {
				for(j=0; j<3; j++) {
					sumX += in[(y+i-1)*640*3+(x+j-1)*3+0]*maskSobelX[i][j]; 
					sumY += in[(y+i-1)*640*3+(x+j-1)*3+0]*maskSobelY[i][j]; 
				}
			}
			newValue = (sumX > 0 ? sumX : -sumX) + (sumY > 0 ? sumY : -sumY);
			for(c=0; c<channel; c++)
				out[y*640*3 + x*3+c] = newValue;
		}
	}
}
