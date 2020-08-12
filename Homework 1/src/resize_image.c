#include <math.h>
#include "image.h"
#include <stdio.h>


float nn_interpolate(image im, float x, float y, int c)
{
	return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image source_image, int target_width, int target_height)
{
	image target_image;
	target_image = make_image(target_width, target_height, source_image.c);

	int x;
	int y;
	int c;

	/*source image array coordinates*/
	float source_x;
	float source_y;
	/*nearest neighbour pixel*/
	float interpolated_pixel;

	for (c = 0; c < 3; c++)
	{
		for (x = 0; x < target_width; x++)
		{	
			source_x = (float) (x * source_image.w)  / (float) target_width;        	
			for (y = 0; y < target_height; y++)
			{	
				source_y = (float) (y * source_image.h) / (float) target_height;
				interpolated_pixel = nn_interpolate(source_image, source_x, source_y, c);
				set_pixel(target_image, x, y, c, interpolated_pixel);
			}
		}
	}
	return target_image;
}


float bilinear_interpolate(image source_image, float x, float y, int c)
{
	/*http://supercomputingblog.com/graphics/coding-bilinear-interpolation */

	int x1;
	int y1;
	int x2;
	int y2;

	float interpolated_pixel;

	float q11;
	float q21;
	float q12;
	float q22;

	float r1;
	float r2;

	x1 = floor(x);
	y1 = floor(y);
	x2 = ceil(x);
	y2 = ceil(y);

	q11 = get_pixel(source_image, x1, y1, c);
	q21 = get_pixel(source_image, x2, y1, c);
	q12 = get_pixel(source_image, x1, y2, c);
	q22 = get_pixel(source_image, x2, y2, c);

	r1 = (x2 - x)*q11 + (x - x1)*q21;
	r2 = (x2 - x)*q12 + (x - x1)*q22;

	interpolated_pixel = (y2 - y)*r1 + (y - y1)*r2;

	return interpolated_pixel;
}


image bilinear_resize(image source_image, int target_width, int target_height)
{
	image target_image;
	target_image = make_image(target_width, target_height, source_image.c);

	int x;
	int y;
	int c; //channel

	float interpolated_pixel;
	float source_x;
	float source_y;

	float width_ratio;
	float adjust_width;

	float height_ratio;
	float adjust_height;

	width_ratio  = (float) source_image.w / target_width;
	height_ratio = (float) source_image.h / target_height;

	adjust_width  = -0.5 + 0.5 * width_ratio;
	adjust_height = -0.5 + 0.5 * height_ratio;

	for (c = 0; c < 3; c++)
	{
		for (x = 0; x < target_width; x++)
		{
			source_x = x*width_ratio + adjust_width;        	
			for (y = 0; y < target_height; y++)
			{	
				source_y = y*height_ratio + adjust_height;
				interpolated_pixel = bilinear_interpolate(source_image, source_x, source_y, c);
				set_pixel(target_image, x, y, c, interpolated_pixel);
			}
		}
	}
	return target_image;
}
