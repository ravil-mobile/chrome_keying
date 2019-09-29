
#ifndef _COMPUTE_KERNELS_H_
#define _COMPUTE_KERNELS_H_

#include "helper.h"
#include "image.h"
#include "filter.h"


namespace keying {
    void apply_opencl(ImageRGB &fg, ImageRGB &output);
}

namespace conv {
    void apply_opencl(ImageRGB &input, Filter &filter, ImageRGB &output);
}
#endif  // _COMPUTE_KERNELS_H_