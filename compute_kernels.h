
#include "helper.h"
#include "image.h"
#include "filter.h"

#ifndef _CHROME_KEYING_H_
#define _CHROME_KEYING_H_

namespace keying {
    void apply_default(ImageRGB &fg, ImageRGB &output);
    void apply_openmp(ImageRGB &fg, ImageRGB &output);
    void apply_simd_intrinsics(ImageRGB &fg, ImageRGB &output);
}

namespace conv {
    void apply_default(ImageRGB &input, Filter &filter, ImageRGB &output);
    void apply_simd_intrinsics(ImageRGB &input, Filter &filter, ImageRGB &output);
}
#endif  // _CHROME_KEYING_H_