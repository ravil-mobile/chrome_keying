#ifndef _CHROME_KEYING_H_
#define _CHROME_KEYING_H_

#include "helper.h"

ImageRGB apply_default_ck(ImageRGB &fg, ImageRGB &bg);
ImageRGB apply_openmp_ck(ImageRGB &fg, ImageRGB &bg);
ImageRGB apply_simd_intrinsics_ck(ImageRGB &fg, ImageRGB &bg);


#endif  // _CHROME_KEYING_H_