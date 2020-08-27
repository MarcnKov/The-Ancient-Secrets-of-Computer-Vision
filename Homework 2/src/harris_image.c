#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include "matrix.h"
#include <time.h>

// Frees an array of descriptors.
// descriptor *d: the array.
// int n: number of elements in array.
void free_descriptors(descriptor *d, int n)
{
	int i;
	for(i = 0; i < n; ++i){
		free(d[i].data);
	}
	free(d);
}

// Create a feature descriptor for an index in an image.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.
descriptor describe_index(image im, int i)
{
	int w = 5;
	descriptor d;
	d.p.x = i%im.w;
	d.p.y = i/im.w;
	d.data = calloc(w*w*im.c, sizeof(float));
	d.n = w*w*im.c;
	int c, dx, dy;
	int count = 0;
	// If you want you can experiment with other descriptors
	// This subtracts the central value from neighbors
	// to compensate some for exposure/lighting changes.
	for(c = 0; c < im.c; ++c){
		float cval = im.data[c*im.w*im.h + i];
		for(dx = -w/2; dx < (w+1)/2; ++dx){
			for(dy = -w/2; dy < (w+1)/2; ++dy){
				float val = get_pixel(im, i%im.w+dx, i/im.w+dy, c);
				d.data[count++] = cval - val;
			}
		}
	}
	return d;
}

// Marks the spot of a point in an image.
// image im: image to mark.
// ponit p: spot to mark in the image.
void mark_spot(image im, point p)
{
	int x = p.x;
	int y = p.y;
	int i;
	for(i = -9; i < 10; ++i){
		set_pixel(im, x+i, y, 0, 1);
		set_pixel(im, x, y+i, 0, 1);
		set_pixel(im, x+i, y, 1, 0);
		set_pixel(im, x, y+i, 1, 0);
		set_pixel(im, x+i, y, 2, 1);
		set_pixel(im, x, y+i, 2, 1);
	}
}

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptor *d: corners in the image.
// int n: number of descriptors to mark.
void mark_corners(image im, descriptor *d, int n)
{
	int i;
	for(i = 0; i < n; ++i){
		mark_spot(im, d[i].p);
	}
}

// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row image of the filter.
image make_1d_gaussian(float sigma)
{
	// TODO: optional, make separable 1d Gaussian.
	return make_image(1,1,1);
}

// Smooths an image using separable Gaussian filter.
// image im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed image.
image smooth_image(image im, float sigma)
{
	if(1){
		image g = make_gaussian_filter(sigma);
		image s = convolve_image(im, g, 1);
		free_image(g);
		return s;
	} else {
		// TODO: optional, use two convolutions with 1d gaussian filter.
		// If you implement, disable the above if check.
		return copy_image(im);
	}
}

image mult_image(image a, image b)
{
	assert(a.w == b.w && a.h == b.h);
	int i, j;
	float product;
	image p = make_image(a.w, b.h, 1);
	for(i = 0; i < p.w; i++)
	{
		for(j = 0; j < p.h; j++)
		{
			product = get_pixel(a,i,j,0)*get_pixel(b,i,j,0);
			set_pixel(p, i, j, 0, product);
		}
	}
	return p;
}

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.

image structure_matrix(image im, float sigma)
{
	image im_dx, 	im_dy 		 	= make_image(im.w,im.h,1);
	image im_dxdx, 	im_dydy,  im_dxdy 	= make_image(im.w,im.h,1);
	image im_wdxdx, im_wdydy, im_wdxdy 	= make_image(im.w,im.h,1);

	//1//Calculate image derivatives Ix and Iy.
	im_dx = convolve_image(im, make_gx_filter(), 0);
	im_dy = convolve_image(im, make_gy_filter(), 0);
	
	//2//Calculate measures IxIx, IyIy, and IxIy.
	im_dxdx = mult_image(im_dx, im_dx);
	im_dydy = mult_image(im_dy, im_dy);
	im_dxdy = mult_image(im_dx, im_dy);

	//3//Calculate structure matrix components as weighted sum of nearby measures.
	im_wdxdx = convolve_image(im_dxdx, make_gaussian_filter(sigma), 1);
	im_wdydy = convolve_image(im_dydy, make_gaussian_filter(sigma), 1);
	im_wdxdy = convolve_image(im_dxdy, make_gaussian_filter(sigma), 1);

	image S = make_image(im.w, im.h, 3);
	int x, y;
	for (x = 0; x < im.w; x++)
	{
		for (y = 0; y < im.h; y++)
		{
			set_pixel(S, x, y, 0, get_pixel(im_wdxdx, x, y, 0));
			set_pixel(S, x, y, 1, get_pixel(im_wdydy, x, y, 0));
			set_pixel(S, x, y, 2, get_pixel(im_wdxdy, x, y, 0));	
		}
	}
	
	free_image(im_dx);
   	free_image(im_dy);

	free_image(im_dxdx);
   	free_image(im_dydy);
    	free_image(im_dxdy);

    	free_image(im_wdxdx);
   	free_image(im_wdydy);
    	free_image(im_wdxdy);

	return S;
}


// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// returns: a response map of cornerness calculations.

image cornerness_response(image S)
{
	image R = make_image(S.w, S.h, 1);
	// TODO: fill in R, "cornerness" for each pixel using the structure matrix.
	// We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.

	int x;
	int y;

	float dxdx;
	float dydy;
	float dxdy;
	float det;
	float trace;
	float alpha;
	float cornerness;

	alpha = 0.06;
	for (x = 0; x < R.w; x++)
	{
		for (y = 0; y < R.h; y++)
		{	
			dxdx = get_pixel(S, x, y, 0);
            		dydy = get_pixel(S, x, y, 1);
            		dxdy = get_pixel(S, x, y, 2);

			det		= dxdx*dydy - pow(dxdy, 2);
			trace 		= dxdx + dydy;
			cornerness 	= det - alpha * pow(trace,2);
			set_pixel(R, x, y, 0, cornerness);
		}
	}
	return R;
}

int check_bounds(image im, int pos, int flag)
{
	// check x
	if	(flag == 0 && (pos >= im.w || pos < 0))
		return 0;
	else if (flag == 1 && (pos >= im.h || pos < 0))
		return 0;
	return 1;
}


// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: image with only local-maxima responses within w pixels.
image nms_image(image im, int w)
{
	image r = copy_image(im);
	// TODO: perform NMS on the response map.
	// for every pixel in the image:
	//     for neighbors within w:
	//         if neighbor response greater than pixel response:
	//             set response to be very low (I use -999999 [why not 0??])

	int x;
	int y;
	int n; //neighbours
	int i;
	
	float pxel_resp;
	float nghb_resp;

	int move[4][2] = 
	{           // 	DIRECTION
		{1, 0}, //  north
		{-1,0}, //  south
		{0, 1}, //  east
		{0,-1}  // 	west
	};
	
	for (x = 0; x < im.w; x++)
	{
		for (y = 0; y < im.h; y++)
		{
			for (n = 0; n < w; n++)
			{
				pxel_resp = get_pixel(im, x+n, y+n, 0);
				for(i = 0; i < 4; i++)
				{
					nghb_resp = get_pixel(im, x+n+move[i][0], y+n+move[i][1], 0);
					if (nghb_resp > pxel_resp && (check_bounds(r, n+x, 0) && check_bounds(r, n+y, 1)))
						set_pixel(r, n+x, n+y, 0, -999999.0f);
				}				
			}
		}
	}
	return r;
}

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
descriptor *harris_corner_detector(image im, float sigma, float thresh, int nms, int *n)
{
	int 	i;
	int count;

	// Calculate structure matrix
	image S = structure_matrix(im, sigma);

	
	// Estimate cornerness
	image R = cornerness_response(S);
	
	
	// Run NMS on the responses
	image Rnms = nms_image(R, nms);



	//TODO: count number of responses over threshold
	
	count = 0;
	for (i = 0; i < Rnms.w*Rnms.h ; i++)
		if (Rnms.data[i] > thresh)
			count++;

	*n = count; // <- set *n equal to number of corners in image.
	descriptor *d = calloc(count, sizeof(descriptor));
	//TODO: fill in array *d with descriptors of corners, use describe_index.
	
	for (i = 0; i < Rnms.w*Rnms.h ; i++)
		if (Rnms.data[i] > thresh)
			*d++ = describe_index(im, i);
    	d -= count; //point to the first index of an array of descriptors

	free_image(S);
	free_image(R);
	free_image(Rnms);

	//descriptor *d = calloc(1, sizeof(descriptor));
	return d;
}

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detect_and_draw_corners(image im, float sigma, float thresh, int nms)
{
	int n = 0;
	descriptor *d = harris_corner_detector(im, sigma, thresh, nms, &n);
	mark_corners(im, d, n);
}
