__kernel void kernel_apply_keying(__global float *fg_red, __global float *fg_green, __global float *fg_blue,
                                  __global float *out_red, __global float *out_green, __global float *out_blue,
                                  float threshold, unsigned num_elements) {
    unsigned idx = get_global_id(0);

    if (idx < num_elements) {
        bool is_green = (fg_green[idx] > fg_red[idx])
                         && (fg_green[idx] > fg_blue[idx]) 
                         && (fg_green[idx] > threshold);
        if (!is_green) {
            out_red[idx] = fg_red[idx];
            out_green[idx] = fg_green[idx];
            out_blue[idx] = fg_blue[idx];
        }
    }
}


const unsigned get_address(const unsigned x, const unsigned y, const unsigned width) {
    return (x + y * width);
}

__kernel void kernel_apply_conv(__global float *in_red, __global float *in_green, __global float *in_blue,
                                __global float *out_red, __global float *out_green, __global float *out_blue,
                                unsigned width, unsigned height,
                                __global float *filter, unsigned m, unsigned n) {


    const unsigned g_idx_x = get_global_id(0);
    const unsigned g_idx_y = get_global_id(1);
    

    const unsigned hm = m / 2;
    const unsigned hn = n / 2;
    const unsigned offset = width + 2 * hm;


    __global float *in_channels[3] = {in_red, in_green, in_blue};
    __global float *out_channels[3] = {out_red, out_green, out_blue};

    
    if ((g_idx_x < width) && (g_idx_y < height))  {

        for (unsigned channel = 0; channel < 3; ++channel) {

            float accumulator = 0.0f;
            for (unsigned yy = 0; yy < n; ++yy) {
                for (unsigned xx = 0; xx < m; ++xx) {
                    accumulator += filter[get_address(xx, yy, m)] 
                                * in_channels[channel][get_address(g_idx_x + xx, g_idx_y + yy, offset)];
                }
            }

            out_channels[channel][get_address(g_idx_x + hm, g_idx_y + hn, offset)] = accumulator; 
        }
    }
}


__kernel void kernel_apply_conv_shmem(__global float *in_red, __global float *in_green, __global float *in_blue,
                                      __global float *out_red, __global float *out_green, __global float *out_blue,
                                      unsigned width, unsigned height,
                                      __constant float *filter, unsigned m, unsigned n,
                                      __local float *patch_srm) {


    const unsigned g_idx_x = get_global_id(0);
    const unsigned g_idx_y = get_global_id(1);
    
    const unsigned l_idx_x = get_local_id(0);
    const unsigned l_idx_y = get_local_id(1);

    const unsigned lin_idx = l_idx_x + l_idx_y * get_local_size(0);

    const unsigned patch_width = get_local_size(0) + m;
    const unsigned patch_height = get_local_size(1) + n;

    const unsigned hm = m / 2;
    const unsigned hn = n / 2;
    const offset = width + 2 * hm;

    
    __global float *in_channels[3] = {in_red, in_green, in_blue};
    __global float *out_channels[3] = {out_red, out_green, out_blue};


    for (unsigned channel = 0; channel < 3; ++channel) {

        if ((g_idx_x < width) && (g_idx_y < height))  {
            // load patch to its shared memory
            const unsigned base_address_x = get_group_id(0) * get_local_size(0);
            const unsigned base_address_y = get_group_id(1) * get_local_size(1);
            for (unsigned y = l_idx_y; y < patch_height; y += get_local_size(1)) {
                for (unsigned x = l_idx_x; x < patch_width; x += get_local_size(0)) {
                        
                    patch_srm[get_address(x, y, patch_width)] = 
                                in_channels[channel][get_address(base_address_x + x, base_address_y + y, offset)];
                }
            }

            barrier(CLK_LOCAL_MEM_FENCE);

            // convolve a channel
            float accumulator = 0.0f;
            for(unsigned yy = 0; yy < n; ++yy) {
                for (unsigned xx = 0; xx < m; ++xx) {
                        accumulator += filter[get_address(xx, yy, m)] 
                                    * patch_srm[get_address(l_idx_x + xx, l_idx_y + yy, patch_width)];
                    }
                }

            // write results back to the global mem
            out_channels[channel][get_address(g_idx_x + hm, g_idx_y + hn, offset)] = accumulator;
        }
    }
}