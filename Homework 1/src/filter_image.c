#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void normalize_channel(image source_image, float normalization_factor, int c)
{
	int x; //col
	int y; //row

	float pixel_value;
	for (x = 0; x < source_image.w; x++)
	{
		for (y = 0; y < source_image.h; y++)
		{
			pixel_value = get_pixel(source_image, x, y, c);
			set_pixel(source_image, x, y, c, pixel_value / normalization_factor);
		}
	}
}


void l1_normalize(image source_image)
{
	int x; //col
	int y; //row
	int c; //chanel

	float normalization_factor;

	for (c = 0; c < 3; c++)
	{
		for (x = 0; x < source_image.w; x++)
		{
			for (y = 0; y < source_image.h; y++)
			{
				normalization_factor += get_pixel(source_image, x, y, c);
			}
		}
	}

	for (c = 0; c < 3; c++)
		normalize_channel(source_image, normalization_factor, c);
}


image make_box_filter(int width)
{
	image box_filter = make_image(width, width, 1);

	int x;
	int y;

	for (x = 0; x < box_filter.w; x++)
	{
		for(y = 0; y < box_filter.h; y++)
		{
			set_pixel(box_filter, x, y, 0, 1.0); //segfaults
		}
	}

	normalize_channel(box_filter, (float) width*width, 0);

	return box_filter;
}


image convolve_image(image source_image, image image_filter, int preserve)
{
	int source_x;
	int filter_x;
	int correl_x; //cross-correlation x

	int source_y;
	int filter_y;
	int correl_y; //cross-correlation y

	int channel;

	float filter_coefficient;
	float correlated_pixel; //cross-correlated pixel
	float source_pixel;

	image target_image;

	assert(image_filter.c == 1 || image_filter.c == source_image.c);

	target_image = make_image(source_image.w, source_image.h, source_image.c);

	for (channel = 0; channel < source_image.c; channel++) 
	{
		for (source_x = 0; source_x < source_image.w; source_x++)
		{
			for (source_y = 0; source_y < source_image.h; source_y++)
			{
				for (filter_x = 0; filter_x < image_filter.w; filter_x++)
				{
					for (filter_y = 0; filter_y < image_filter.h; filter_y++)
					{	
						/*We slide the filter over the picture. Padding implemented
						in set_pixel function takes care of overflow.		 */
						correl_x = source_x + filter_x;
						correl_y = source_y + filter_y;
						if (preserve || (!preserve && source_image.c > 1))
						{
							filter_coefficient = get_pixel(image_filter, filter_x, filter_y, 0);
							source_pixel 	= get_pixel(source_image, correl_x, correl_y, channel);
							correlated_pixel += filter_coefficient*source_pixel;
						}
						set_pixel(target_image, source_x, source_y, channel, correlated_pixel);
					}
				}
				correlated_pixel = 0;
			}
		}
	}

	return target_image;
}

image make_highpass_filter()
{
	// TODO
	return make_image(1,1,1);
}

image make_sharpen_filter()
{
	// TODO
	return make_image(1,1,1);
}

image make_emboss_filter()
{
	// TODO
	return make_image(1,1,1);
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
	// TODO
	return make_image(1,1,1);
}

image add_image(image a, image b)
{
	// TODO
	return make_image(1,1,1);
}

image sub_image(image a, image b)
{
	// TODO
	return make_image(1,1,1);
}

image make_gx_filter()
{
	// TODO
	return make_image(1,1,1);
}

image make_gy_filter()
{
	// TODO
	return make_image(1,1,1);
}

void feature_normalize(image im)
{
	// TODO
}

image *sobel_image(image im)
{
	// TODO
	return calloc(2, sizeof(image));
}

image colorize_sobel(image im)
{
	// TODO
	return make_image(1,1,1);
}
