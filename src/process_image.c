#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

#define EPS .0001

float get_pixel(image im, int x, int y, int c)
{
    // we use clamp padding strategy
    if (x < 0) x = 0;
    else if( x >= im.w) x = im.w - 1;
    if (y < 0) y = 0;
    else if( y >= im.h) y = im.h - 1;
    if (c < 0) c = 0;
    else if( c >= im.c) c = im.c - 1;

    int idx = x + im.w*y + im.w*im.h*c;
    return im.data[idx];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if(x >= im.w || x < 0 || y >= im.h || y < 0 || c >= im.c || c < 0)
        return;
    int idx = x + im.w*y + im.w*im.h*c;
    im.data[idx] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, im.w*im.h*im.c*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for (int j = 0; j < im.h; ++j)
        for (int i = 0; i< im.w; ++i)
            gray.data[i + im.w*j] = 0.299 * im.data[i + im.w*j] + 
                                    0.587 * im.data[i + im.w*j + im.w*im.h] + 
                                    0.114 * im.data[i + im.w*j + im.w*im.h*2];
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int j = 0; j < im.h; ++j)
        for (int i = 0; i< im.w; ++i)
            im.data[i + im.w*j + im.w*im.h*c] += v;
}

void clamp_image(image im)
{
    int size = im.w*im.h*im.c;
    for (int i = 0; i < size; ++i)
    {
        if (im.data[i] < 0.f)
            im.data[i] = 0.f;
        else if (im.data[i] > 0.f)
            im.data[i] = 1.f;
    }
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
    for (int j = 0; j < im.h; ++j)
        for (int i = 0; i< im.w; ++i)
        {
            float R = get_pixel(im, i, j, 0);
            float G = get_pixel(im, i, j, 1);
            float B = get_pixel(im, i, j, 2);
            float V = three_way_max(R, G, B);
            float m = three_way_min(R, G, B);
            float c = V - m;
            float S = V == 0.f? 0 : c / V;
            float H = 0;
            if (c != 0)
            { 
                float h = 0;
                if (fabs(R - V) <= EPS)
                    h = (G - B) / c;
                else if (fabs(G - V) <= EPS)
                    h = (B - R) / c + 2;
                else if (fabs(B - V) <= EPS)
                    h = (R - G) / c + 4;

                if (h < 0)
                    H = h/6 + 1;
                else
                    H = h/6;
            }

            set_pixel(im, i, j, 0,  H);
            set_pixel(im, i, j, 1,  S);
            set_pixel(im, i, j, 2,  V);
        }
}

void hsv_to_rgb(image im)
{
    for (int j = 0; j < im.h; ++j)
        for (int i = 0; i< im.w; ++i)
        {
            float H = get_pixel(im, i, j, 0) * 360;
            float S = get_pixel(im, i, j, 1);
            float V = get_pixel(im, i, j, 2);
            float c = V * S;
            float m = V - c;
            float x = c * (1 - fabs(fmodf(H / 60, 2) - 1));
            float R = 0.f, G = 0.f, B = 0.f;

            if (H < 60)
            {
                R = c;
                G = x;
            }
            else if(H < 120)
            {
                R = x;
                G = c;
            }
            else if(H < 180)
            {
                G = c;
                B = x;
            }
            else if(H < 240)
            {
                G = x;
                B = c;
            }
            else if(H < 300)
            {
                R = x;
                B = c;
            }
            else if(H < 360)
            {
                R = c;
                B = x;
            }

            set_pixel(im, i, j, 0,  R + m);
            set_pixel(im, i, j, 1,  G + m);
            set_pixel(im, i, j, 2,  B + m);
        }
}
