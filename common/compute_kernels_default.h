
#ifndef _COMPUTE_KERNELS_DEFAULT_H_
#define _COMPUTE_KERNELS_DEFAULT_H_

#include "helper.h"
#include "image.h"
#include "filter.h"


namespace keying {
    void apply_default(ImageRGB &fg, ImageRGB &output);
}

namespace conv {
    void apply_default(ImageRGB &input, Filter &filter, ImageRGB &output);
}

#endif  // _COMPUTE_KERNELS_DEFAULT_H_