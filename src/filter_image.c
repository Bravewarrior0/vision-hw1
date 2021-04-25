#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0;
    for (int k = 0; k < im.c; ++k)
        for (int j = 0; j < im.h; ++j)
            for (int i = 0; i < im.w; ++i)
                sum += get_pixel(im, i, j, k);
    for (int k = 0; k < im.c; ++k)
        for (int j = 0; j < im.h; ++j)
            for (int i = 0; i < im.w; ++i)
                set_pixel(im, i, j, k, get_pixel(im, i, j, k)/sum);
}

image make_box_filter(int w)
{
    image filter = make_image(w,w,1);
    for (int j = 0; j < w; ++j)
        for (int i = 0; i < w; ++i)
            set_pixel(filter, i, j, 0, 1.0f);
    l1_normalize(filter);
    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(im.c == filter.c || filter.c == 1 || "filter channels should be equal image channels or 1");
    image out;
    if (preserve)
        out = make_image(im.w, im.h, im.c);
    else
        out = make_image(im.w, im.h, 1);

    for (int k = 0; k < im.c; ++k)
    {
        for (int j = 0; j < im.h; ++j)
        {
            for (int i = 0; i < im.w; ++i)
            {
                float value = 0.f;
                for (int y = 0; y < filter.h; ++y)
                {
                    for (int x = 0; x < filter.w; ++x)
                    {
                        value += get_pixel(im, i + x - filter.w / 2, j + y - filter.h / 2, k) *
                                    get_pixel(filter, x, y, k);
                    }
                }
                if (preserve)
                    set_pixel(out, i, j, k, value);
                else
                {
                    float current_value = get_pixel(out, i, j, 0);
                    set_pixel(out, i, j, 0, current_value + value);
                }
            }
        }
    }
    return out;
}

image make_highpass_filter()
{
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, 0.f);
    set_pixel(filter, 0, 1, 0, -1.f);
    set_pixel(filter, 0, 2, 0, 0.f);
    set_pixel(filter, 1, 0, 0, -1.f);
    set_pixel(filter, 1, 1, 0, 4.f);
    set_pixel(filter, 1, 2, 0, -1.f);
    set_pixel(filter, 2, 0, 0, 0.f);
    set_pixel(filter, 2, 1, 0, -1.f);
    set_pixel(filter, 2, 2, 0, 0.f);
    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, 0.f);
    set_pixel(filter, 0, 1, 0, -1.f);
    set_pixel(filter, 0, 2, 0, 0.f);
    set_pixel(filter, 1, 0, 0, -1.f);
    set_pixel(filter, 1, 1, 0, 5.f);
    set_pixel(filter, 1, 2, 0, -1.f);
    set_pixel(filter, 2, 0, 0, 0.f);
    set_pixel(filter, 2, 1, 0, -1.f);
    set_pixel(filter, 2, 2, 0, 0.f);
    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -2.f);
    set_pixel(filter, 0, 1, 0, -1.f);
    set_pixel(filter, 0, 2, 0, 0.f);
    set_pixel(filter, 1, 0, 0, -1.f);
    set_pixel(filter, 1, 1, 0, 1.f);
    set_pixel(filter, 1, 2, 0, 1.f);
    set_pixel(filter, 2, 0, 0, 0.f);
    set_pixel(filter, 2, 1, 0, 1.f);
    set_pixel(filter, 2, 2, 0, 2.f);
    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    int width = (int) (6 * sigma) + 1;
    float sigma2 = sigma * sigma;
    image filter = make_image(width, width, 1);
    for (int j = 0; j < filter.h; ++j)
    {
        for(int i = 0; i < filter.w; ++i)
        {
            int x = i - filter.w/2;
            int y = j - filter.h/2;
            float value = 1.f /(TWOPI * sigma2) * exp(-(x * x + y *y)/(2 * sigma2));
            set_pixel(filter, i, j, 0, value);
        }
    }
    l1_normalize(filter);
    return filter;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c && "images should be equal sizes");
    image result = make_image(a.w, a.h, a.c);
    int size = a.w * a.h * a.c;
    for (int i = 0; i < size; ++i)
        result.data[i] = a.data[i] + b.data[i];
    return result;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c && "images should be equal sizes");
    image result = make_image(a.w, a.h, a.c);
    int size = a.w * a.h * a.c;
    for (int i = 0; i < size; ++i)
        result.data[i] = a.data[i] - b.data[i];
    return result;
}

image make_gx_filter()
{
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -1.f);
    set_pixel(filter, 0, 1, 0, 0.f);
    set_pixel(filter, 0, 2, 0, 1.f);
    set_pixel(filter, 1, 0, 0, -2.f);
    set_pixel(filter, 1, 1, 0, 0.f);
    set_pixel(filter, 1, 2, 0, 2.f);
    set_pixel(filter, 2, 0, 0, -1.f);
    set_pixel(filter, 2, 1, 0, 0.f);
    set_pixel(filter, 2, 2, 0, 1.f);
    return filter;
}

image make_gy_filter()
{
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -1.f);
    set_pixel(filter, 0, 1, 0, -2.f);
    set_pixel(filter, 0, 2, 0, -1.f);
    set_pixel(filter, 1, 0, 0, 0.f);
    set_pixel(filter, 1, 1, 0, 0.f);
    set_pixel(filter, 1, 2, 0, 0.f);
    set_pixel(filter, 2, 0, 0, 1.f);
    set_pixel(filter, 2, 1, 0, 2.f);
    set_pixel(filter, 2, 2, 0, 1.f);
    return filter;
}

void feature_normalize(image im)
{
    float min = get_pixel(im, 0, 0, 0);
    float max = get_pixel(im, 0, 0, 0);
    int size = im.w * im.h * im.c;
    for (int i = 0; i < size; ++i)
    {
        if(min > im.data[i])
            min = im.data[i];
        else if (max < im.data[i])
            max = im.data[i];
    }
    float range = max - min;
    for (int i = 0; i < size; ++i)
    {
        if (range != 0.f)
            im.data[i] = (im.data[i] - min) / range;
        else
            im.data[i] = 0.f;
    }
}

image *sobel_image(image im)
{
    image* out = calloc(2, sizeof(image));
    image mag = make_image(im.w, im.h, 1);
    image dir = make_image(im.w, im.h, 1);
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();
    image gx = convolve_image(im, gx_filter, 0);
    image gy = convolve_image(im, gy_filter, 0);
    int size = im.w * im.h;
    for (int i = 0; i < size; ++i)
    {
        mag.data[i] = sqrtf(gx.data[i] * gx.data[i] + gy.data[i] * gy.data[i]);
        dir.data[i] = atan2f(gx.data[i], gy.data[i]);
    }
    free_image(gx_filter);
    free_image(gy_filter);
    free_image(gx);
    free_image(gy);
    out[0] = mag;
    out[1] = dir;
    return out;
}

image colorize_sobel(image im)
{
    image* sobel = sobel_image(im);
    feature_normalize(sobel[0]);
    feature_normalize(sobel[1]);
    image color_sobel =  make_image(im.w, im.h, im.c);
    for (int j = 0; j < im.h; ++j)
    {
        for (int i = 0; i < im.w; ++i)
        {
            float mag = get_pixel(sobel[0], i ,j ,0);
            float angle = get_pixel(sobel[1], i ,j ,0);
            set_pixel(color_sobel, i, j, 0, angle);
            set_pixel(color_sobel, i, j, 1, mag);
            set_pixel(color_sobel, i, j, 2, mag);
        }
    }
    hsv_to_rgb(color_sobel);

    free_image(sobel[0]);
    free_image(sobel[1]);
    free(sobel);
    return color_sobel;
}
