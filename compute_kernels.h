
#include "helper.h"
#include "image.h"
#include "filter.h"

#ifndef _CHROME_KEYING_H_
#define _CHROME_KEYING_H_


void apply_default_ck(ImageRGB &fg, ImageRGB &output);
void apply_openmp_ck(ImageRGB &fg, ImageRGB &output);
void apply_simd_intrinsics_ck(ImageRGB &fg, ImageRGB &output);


void convolve_default(ImageRGB &input, Filter &filter, ImageRGB &output);
void convolve_simd_intrinsics(ImageRGB &input, Filter &filter, ImageRGB &output);

#endif  // _CHROME_KEYING_H_