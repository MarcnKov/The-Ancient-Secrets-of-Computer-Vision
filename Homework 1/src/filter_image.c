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

	for (c = 0; c < source_image.c; c++)
	{
		for (x = 0; x < source_image.w; x++)
		{
			for (y = 0; y < source_image.h; y++)
			{
				normalization_factor += get_pixel(source_image, x, y, c);
			}
		}
	}

	for (c = 0; c < source_image.c; c++)
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
	int target_channels;

	float filter_coefficient;
	float correlated_pixel; //cross-correlated pixel
	float source_pixel;

	image target_image;

	assert(image_filter.c == 1 || image_filter.c == source_image.c);

	if ((image_filter.c == source_image.c || image_filter.c == 1) && preserve)
		target_channels = source_image.c;
	else
		target_channels = 1;

	target_image = make_image(source_image.w, source_image.h, target_channels);

	for (source_x = 0; source_x < source_image.w; source_x++)
	{
		for (source_y = 0; source_y < source_image.h; source_y++)
		{
			for (channel = 0; channel < source_image.c; channel++) 
			{
				if (channel == 2 && !preserve)
				{
					set_pixel(target_image, source_x, source_y, 0, correlated_pixel);
					correlated_pixel = 0;
				}
				else if (preserve)
					correlated_pixel = 0;
				for (filter_x = 0; filter_x < image_filter.w; filter_x++)
				{	
					for (filter_y = 0; filter_y < image_filter.h; filter_y++)
					{	
						/*We slide the filter over the picture. Padding implemented
						  in set_pixel function takes care of overflow.		 */
						correl_x = source_x + filter_x - image_filter.w/2;
						correl_y = source_y + filter_y - image_filter.h/2;

						if (image_filter.c == 1)		
							filter_coefficient = get_pixel(image_filter, filter_x, filter_y, 0);
						else
							filter_coefficient = get_pixel(image_filter, filter_x, filter_y, channel); 

						source_pixel 	= get_pixel(source_image, correl_x, correl_y, channel);
						correlated_pixel += filter_coefficient*source_pixel;

						/*Preserve all channels. Apply filter on each channel.*/
						if (preserve)
							set_pixel(target_image, source_x, source_y, channel, correlated_pixel);
						/*Produce a target_image with 1 channel. Perform the following opeartion:
							
						R conv Filter \
                  					       \
						G conv Filter    => Sum => output
                            				       /
						B conv Filter /

						*/
						else
							correlated_pixel += correlated_pixel;	
					}

				}
			}

		}
	}

	return target_image;
}

image make_highpass_filter()
{
	image highpass_filter;
	highpass_filter = make_image(3,3,1);

	set_pixel(highpass_filter, 0, 0, 0, 0);
	set_pixel(highpass_filter, 1, 0, 0, -1);
	set_pixel(highpass_filter, 2, 0, 0, 0);

	set_pixel(highpass_filter, 0, 1, 0, -1);
	set_pixel(highpass_filter, 1, 1, 0, 4);
	set_pixel(highpass_filter, 2, 1, 0, -1);

	set_pixel(highpass_filter, 0, 2, 0, 0);
	set_pixel(highpass_filter, 1, 2, 0, -1);
	set_pixel(highpass_filter, 2, 2, 0, 0);

	return highpass_filter;
}

image make_sharpen_filter()
{
	image sharpen_filter;
	sharpen_filter = make_image(3,3,1);

	set_pixel(sharpen_filter, 0, 0, 0, 0);
	set_pixel(sharpen_filter, 1, 0, 0, -1);
	set_pixel(sharpen_filter, 2, 0, 0, 0);

	set_pixel(sharpen_filter, 0, 1, 0, -1);
	set_pixel(sharpen_filter, 1, 1, 0, 5);
	set_pixel(sharpen_filter, 2, 1, 0, -1);

	set_pixel(sharpen_filter, 0, 2, 0, 0);
	set_pixel(sharpen_filter, 1, 2, 0, -1);
	set_pixel(sharpen_filter, 2, 2, 0, 0);

	return sharpen_filter;
}

image make_emboss_filter()
{
	image emboss_filter;
	emboss_filter = make_image(3,3,1);

	set_pixel(emboss_filter, 0, 0, 0, -2);
	set_pixel(emboss_filter, 1, 0, 0, -1);
	set_pixel(emboss_filter, 2, 0, 0, 0);

	set_pixel(emboss_filter, 0, 1, 0, -1);
	set_pixel(emboss_filter, 1, 1, 0, 1);
	set_pixel(emboss_filter, 2, 1, 0, 1);

	set_pixel(emboss_filter, 0, 2, 0, 0);
	set_pixel(emboss_filter, 1, 2, 0, 1);
	set_pixel(emboss_filter, 2, 2, 0, 2);

	return emboss_filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
	
	image gaussian_filter;

	int size_tmp;
	int filter_x;
	int filter_y;
	int filter_size;

	float gaussian_arg;
	float gaussian_param;
	float gaussian_pixel;
	
	size_tmp 	= (int) 6 *  ceilf(sigma); 	
	filter_size 	= size_tmp % 2 == 0 ? size_tmp+1 : size_tmp;
	gaussian_filter = make_image(filter_size, filter_size, 1);
	
	for (filter_x = 0; filter_x < filter_size; filter_x++)
	{
		for (filter_y = 0; filter_y < filter_size; filter_y++)
		{	
			gaussian_arg    = 1.0f/ (TWOPI*sigma*sigma);
			gaussian_param 	= (float) (filter_x*filter_x + filter_y*filter_y) / (2*sigma*sigma);
			gaussian_pixel 	= gaussian_arg*exp(-gaussian_param);
			set_pixel(gaussian_filter, filter_x, filter_y, 0, gaussian_pixel);
		}
	}	
	l1_normalize(gaussian_filter);

	return gaussian_filter;
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
