#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "image.h"

int check_boundaries(image im, int var, int flag);
float calculate_hue(float r, float g, float b, float val, float con);
float* calculate_rgb(float hue_p, float chr,float val ,float x);

float get_pixel(image im, int x, int y, int c)
{
    x = check_boundaries(im, x, 0);
    y = check_boundaries(im, y, 1);

    return im.data[x + y*im.w + c*im.w*im.h];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (x < 0 || y < 0 || x > im.w || y > im.h)
        return;
    im.data[x + y*im.w + c*im.w*im.h] = v; 
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(&copy, &im, 3*im.w*im.h);
    return copy;
}

image rgb_to_grayscale(image im)
{
    int x; //col
    int y; //row

    float luma;

    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);

    for (x = 0; x < im.w; x++)
    {
        for (y = 0; y < im.h; y++)
        {
            luma =  get_pixel(im, x, y, 0)* 0.299 + \
                    get_pixel(im, x, y, 1)* 0.587 + \
                    get_pixel(im, x, y, 2)* 0.114;

            set_pixel(gray, x, y, 0, luma);
        }
    }
    
    return gray;
}

void shift_image(image im, int c, float v)
{
    int x; //col
    int y; //row

    float curr_pixel; //current pixel
    
    for (x = 0; x < im.w; x++)
    {
        for (y = 0; y < im.h; y++)
        {
            curr_pixel = get_pixel(im, x, y, c);
            set_pixel(im, x, y, c, v + curr_pixel);
        }
    }
}

void clamp_image(image im)
{
    int x; //col
    int y; //row
    int c; //chanel
    

    for (c = 0; c < 3; c++)
    {
        for (x = 0; x < im.w; x++)
        {
            for (y = 0; y < im.h; y++)
            {
                if (get_pixel(im, x, y, c) < 0)
                    set_pixel(im, x, y, c, 0);
                else if (get_pixel(im, x, y, c) > 1)
                    set_pixel(im, x, y, c, 1);
            }
        }
    }
    //nope, it's O(n^2)
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    int x;
    int y;

    float r; //red
    float g; //green
    float b; //blue

    float val; //value
    float min; //min
    float chr; //chroma
    float sat; //saturation
    float hue; 

    for (x = 0; x < im.w; x++)
    {
        for (y = 0; y < im.h; y++)
        {   
            /*get pixel values for 3 ch's*/
            r = get_pixel(im, x, y, 0);
            g = get_pixel(im, x, y, 1);
            b = get_pixel(im, x, y, 2);

            /*calculate val, sat and hue*/
            val = three_way_max(r, g, b);
            min = three_way_min(r, g, b);
            chr = val - min;
            sat = ((val == 0) ? 0 : chr / val);
            hue = calculate_hue(r, g, b, val, chr);

            /* store (h,s,c) in the same image,
            replace  (r,g,b) by (h, s, c)   */
            set_pixel(im, x, y, 0, hue);
            set_pixel(im, x, y, 1, sat);
            set_pixel(im, x, y, 2, val);

        }
    }
}

void hsv_to_rgb(image im)
{
    /*The function isn't perfect!
    My transform function saturates
    image slightly more than the one
    in the course. */

    int x;
    int y; 
    float h_prime;
    float hue;
    float sat; //saturation
    float val; //value
    float chr; //chroma
    float z;

    float *rgb;

    rgb = (float *)malloc(sizeof(float)*3);

    for (x = 0; x < im.w; x++)
    {
        for (y = 0; y < im.h; y++)
        {   
            /*get pixel values for 3 ch's*/
            hue = get_pixel(im, x, y, 0);
            sat = get_pixel(im, x, y, 1);
            val = get_pixel(im, x, y, 2);

            /*calculate chr, val and hue*/

            chr     = (1 - fabs(2*val - 1))*sat;
            h_prime = hue * (float) 6;
            z       = chr * (1 - fabs(fmod(h_prime, (float) 2) - 1));

            /*calculate r, g and b*/

            rgb     = calculate_rgb(h_prime, chr, val, z);

            /* store (r,g,b) in the same image,
            replace  (h,s,v) by (r, g, b)   */

            set_pixel(im, x, y, 0, rgb[0]);
            set_pixel(im, x, y, 1, rgb[1]);
            set_pixel(im, x, y, 2, rgb[2]);
        }
    }
}

//float* calculate_rgb(float hue_p, float chr,float val ,float x)

int check_boundaries(image im, int var, int flag)
{   
    if (var < 0)
        return 0;
    /* flag = 0 check x boundary */ 
    else if (flag == 0 && var > im.w)
        var = im.w;
    else if (flag == 1 && var > im.h)
        var = im.h;
    return var;
}

float calculate_hue(float r, float g, float b, float val, float chr)
{
    float hue;
    hue = 0;

    if (chr == 0)
        hue = 0;
    else if (val == r)
        hue = (g-b)/chr;
    else if (val == g)
        hue = (b-r)/chr + 2;
    else if (val == b)
        hue = (r-g)/chr + 4;
    else if (val == r)
        hue = (g-b)/chr;

    return ((hue < 0) ? hue/6 + 1 : hue/6);
}

float* calculate_rgb(float hue_p, float chr,float val ,float x)
{
    float r1;
    float g1;
    float b1;
    float min;
    float *rgb;

    r1 = g1 = b1 = 0;
    rgb = (float *)malloc(sizeof(float)*3);

    if (hue_p == 0)
        r1 = g1 = b1 = 0;
    else if(0 < hue_p && hue_p <= 1)
    {
        r1 = chr;
        g1 = x;
        b1 = 0;
    }
    else if(1 < hue_p && hue_p <= 2)
    {
        r1 = x;
        g1 = chr;
        b1 = 0;
    }
    else if(2 < hue_p && hue_p <= 3)
    {
        r1 = 0;
        g1 = chr;
        b1 = x;
    }
    else if(3 < hue_p && hue_p <= 4)
    {
        r1 = 0;
        g1 = x;
        b1 = chr;
    }
    else if(4 < hue_p && hue_p <= 5)
    {
        r1 = x;
        g1 = 0;
        b1 = chr;
    }
    else if(5 < hue_p && hue_p <= 6)
    {
        r1 = chr;
        g1 = 0;
        b1 = x;
    }

    min = val -  chr/(float) 2;

    rgb[0] = r1 + min;
    rgb[1] = g1 + min;
    rgb[2] = b1 + min;

    return rgb;
}