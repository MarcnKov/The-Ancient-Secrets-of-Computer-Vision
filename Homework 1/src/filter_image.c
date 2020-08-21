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
	// http://www.songho.ca/dsp/convolution/convolution.html#convolution_2d
	int source_x;
	int filter_x;
	int correl_x;
	int center_x;

	int source_y;
	int filter_y;
	int correl_y;
	int center_y;

	int channel;
	int target_channels;

	float filter_coefficient;
	float correlated_pixel;
	float source_pixel;

	image target_image;

	assert(image_filter.c == 1 || image_filter.c == source_image.c);

	target_channels = (preserve) ? source_image.c : 1;	
	target_image 	= make_image(source_image.w, source_image.h, target_channels);

	center_x 	= image_filter.w/2;
	center_y 	= image_filter.h/2;

	for (source_y = 0; source_y < source_image.h; source_y++)
	{
		for (source_x = 0; source_x < source_image.w; source_x++)
		{
			if (!preserve)
				correlated_pixel = 0;
			for(channel=0; channel<source_image.c; channel++)
			{
				if (preserve)
					correlated_pixel = 0;
				for (filter_y = 0; filter_y < image_filter.h; filter_y++)
				{	
					for (filter_x = 0; filter_x < image_filter.w; filter_x++)
					{
						correl_x = source_x + filter_x - center_x;
						correl_y = source_y + filter_y - center_y;

						if (image_filter.c == 1)
							filter_coefficient = get_pixel(image_filter, filter_x, filter_y, 0);
						else
							filter_coefficient = get_pixel(image_filter, filter_x, filter_y, channel); 
						source_pixel 		 = get_pixel(source_image, correl_x, correl_y, channel);
						correlated_pixel 	+= filter_coefficient*source_pixel;
					}
				}
				if (preserve)
					set_pixel(target_image, source_x, source_y, channel, correlated_pixel);
				else
					set_pixel(target_image, source_x, source_y, 0, correlated_pixel);
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
	int center_x;
	int center_y;
	int filter_y;

	int filter_size;

	float gaussian_arg;
	float gaussian_param;
	float gaussian_pixel;

	size_tmp 	= (int) 6 *  ceilf(sigma); 	
	filter_size 	= size_tmp % 2 == 0 ? size_tmp+1 : size_tmp;
	gaussian_filter = make_image(filter_size, filter_size, 1);	
	center_x 	= gaussian_filter.w/2;
	center_y	= gaussian_filter.h/2;

	for (filter_x = 0; filter_x < filter_size; filter_x++)
	{
		for (filter_y = 0; filter_y < filter_size; filter_y++)
		{	
			gaussian_arg    = 1.0f/ (TWOPI*sigma*sigma);
			gaussian_param 	= (float) (pow(center_x - filter_x,2) + pow(center_y-filter_y,2)) / (2*sigma*sigma);
			gaussian_pixel 	= gaussian_arg*exp(-gaussian_param);
			set_pixel(gaussian_filter, filter_x, filter_y, 0, gaussian_pixel);
		}
	}	
	l1_normalize(gaussian_filter);

	return gaussian_filter;
}

image add_image(image image1, image image2)
{
	int x;
	int y;
	int c;

	float pixel1;
	float pixel2;

	image added_image;

	assert(image1.c == image2.c && image1.w == image2.w && image1.h == image2.h);

	added_image = make_image(image1.w, image1.h, image1.c);

	for (c = 0; c < image1.c; c++)
	{
		for (x = 0; x < image1.w; x++)
		{
			for (y = 0; y < image1.h; y++)
			{
				pixel1 = get_pixel(image1, x, y, c);
				pixel2 = get_pixel(image2, x, y, c);
				set_pixel(added_image, x, y, c, pixel1+pixel2);
			}
		}
	}
	return added_image;
}

image sub_image(image image1, image image2)
{
	int x;
	int y;
	int c;

	float pixel1;
	float pixel2;

	image subtracted_image;

	assert(image1.c == image2.c && image1.w == image2.w && image1.h == image2.h);

	subtracted_image = make_image(image1.w, image1.h, image1.c);

	for (c = 0; c < image1.c; c++)
	{
		for (x = 0; x < image1.w; x++)
		{
			for (y = 0; y < image1.h; y++)
			{
				pixel1 = get_pixel(image1, x, y, c);
				pixel2 = get_pixel(image2, x, y, c);
				set_pixel(subtracted_image, x, y, c,pixel1 - pixel2);

			}
		}
	}
	return subtracted_image;
}

image make_gx_filter()
{
	image gx_filter;
	gx_filter = make_image(3,3,1);

	set_pixel(gx_filter, 0, 0, 0, -1);
	set_pixel(gx_filter, 1, 0, 0, 0);
	set_pixel(gx_filter, 2, 0, 0, 1);

	set_pixel(gx_filter, 0, 1, 0, -2);
	set_pixel(gx_filter, 1, 1, 0, 0);
	set_pixel(gx_filter, 2, 1, 0, 2);

	set_pixel(gx_filter, 0, 2, 0, -1);
	set_pixel(gx_filter, 1, 2, 0, 0);
	set_pixel(gx_filter, 2, 2, 0, 1);

	return gx_filter;
}

image make_gy_filter()
{

	image gy_filter;
	gy_filter = make_image(3,3,1);

	set_pixel(gy_filter, 0, 0, 0, -1);
	set_pixel(gy_filter, 1, 0, 0, -2);
	set_pixel(gy_filter, 2, 0, 0, -1);

	set_pixel(gy_filter, 0, 1, 0, 0);
	set_pixel(gy_filter, 1, 1, 0, 0);
	set_pixel(gy_filter, 2, 1, 0, 0);

	set_pixel(gy_filter, 0, 2, 0, 1);
	set_pixel(gy_filter, 1, 2, 0, 2);
	set_pixel(gy_filter, 2, 2, 0, 1);

	return gy_filter;
}

void feature_normalize(image source_image)
{
	float min;
	float max;
	float range;
	float pixel_value;
	float scaled_pixel;
	float current_pixel;

	int c; //channel
	int i;
	int source_x;
	int source_y;

	min = INFINITY;
	max = -1.0;

	for(i = 0; i < source_image.w * source_image.h * source_image.c; i++)
	{
		pixel_value = source_image.data[i];
		if(pixel_value < min)
			min = pixel_value;
		else if(pixel_value > max)
			max = pixel_value;
	}

	range = max - min;
	for(c = 0; c < source_image.c; c++)
	{
		for (source_x = 0; source_x < source_image.w; source_x++)
		{
			for (source_y = 0; source_y < source_image.h; source_y++)
			{
				current_pixel	= get_pixel(source_image, source_x, source_y, c);
				scaled_pixel	= (range != 0) ? (current_pixel - min) / range : 0;
				set_pixel(source_image, source_x, source_y, c, scaled_pixel);
			}
		}
	}	
	/* 		FEATURE NORMALIZATION:
	 *
	 * 		  pixel_value  - min(2D grid)
	 * scaled_pixel = ---------------------------
	 * 		  max(2D grid) - min(2D grid)  	
	 * 
	 * */
}

image *sobel_image(image source_image)
{

	int source_x;
	int source_y;
	int c;

	float gx_pixel;
	float gy_pixel;
	float magnitude;
	float direction; 

	image magnitude_im;
	image direction_im;
	image gx_source_im;
	image gy_source_im;
	image *target_array_of_im; 	

	target_array_of_im = (image *)calloc(2, sizeof(image));

	gx_source_im = convolve_image(source_image, make_gx_filter(), 0);
	gy_source_im = convolve_image(source_image, make_gy_filter(), 0);

	magnitude_im = make_image(gx_source_im.w, gx_source_im.h, gx_source_im.c);
	direction_im = make_image(gx_source_im.w, gx_source_im.h, gx_source_im.c);

	for (c = 0; c < gx_source_im.c; c++)
	{
		for (source_x = 0; source_x < gx_source_im.w; source_x++)
		{
			for (source_y = 0; source_y < gx_source_im.h; source_y++)
			{
				gx_pixel = get_pixel(gx_source_im, source_x, source_y, c);
				gy_pixel = get_pixel(gy_source_im, source_x, source_y, c);

				magnitude = sqrt(pow(gx_pixel,2) + pow(gy_pixel,2));		
				direction = atan2f(gy_pixel,gx_pixel);

				set_pixel(magnitude_im, source_x, source_y, c, magnitude);
				set_pixel(direction_im, source_x, source_y, c, direction);
			}
		}
	}

	target_array_of_im[0] = magnitude_im;
	target_array_of_im[1] = direction_im;

	return target_array_of_im;
}

image colorize_sobel(image im)
{
	// TODO
	return make_image(1,1,1);
}
