#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{
    image resized_im = make_image(w, h, im.c);

    float aw = im.w/(float)w;
    float bw = -0.5 + im.w/(w*2.0f);
    float ah = im.h/(float)h;
    float bh = -0.5 + im.h/(h*2.0f);

    for (int k = 0; k < resized_im.c; ++k)
        for (int j = 0; j < h; ++j)
            for (int i = 0; i < w; ++i)
                set_pixel(resized_im, i, j, k, nn_interpolate(im, i*aw + bw, j*ah + bh, k));

    return resized_im;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // p1--d1----d2--p2
    // |d3   |       |
    // q1----q0------q2
    // |d4   |       |
    // p3------------p4
    int i1 = floor(x);
    int i2 = i1 + 1;
    int j1 = floor(y);
    int j2 = j1 + 1;
    float p1 = get_pixel(im, i1, j1, c);
    float p2 = get_pixel(im, i2, j1, c);
    float p3 = get_pixel(im, i1, j2, c);
    float p4 = get_pixel(im, i2, j2, c);
    float d1 = fabs(x - i1), d2 = 1 - d1;
    float d3 = fabs(y - j1), d4 = 1 - d3;
    float q1 = d4 * p1 + d3 * p3;
    float q2 = d4 * p2 + d3 * p4;
    float q0 = d1 * q2 + d2 * q1;
    return q0;
}

image bilinear_resize(image im, int w, int h)
{
    image resized_im = make_image(w, h, im.c);
    // remember it needs padding to work right
    float aw = im.w/(float)w; // coordinate space paramters
    float bw = -0.5 + 0.5*aw;
    float ah = im.h/(float)h;
    float bh = -0.5 + 0.5*ah;

    for (int k = 0; k < im.c; ++k)
        for (int j = 0; j < h; ++j)
            for (int i = 0; i < w; ++i)
            {
                // map to the old coordinate space
                float x = i*aw + bw;
                float y = j*ah + bh;
                set_pixel(resized_im, i, j, k, bilinear_interpolate(im, x, y, k));
            }

    return resized_im;
}

