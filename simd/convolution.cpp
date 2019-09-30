#include <iostream>
#include <chrono> 
#include <immintrin.h>

#include "compute_kernels.h"


using namespace std::chrono; 

namespace conv {
    //******************************************************************************
    //void apply_openmp(ImageRGB &input, Filter &filter, ImageRGB &output) {}


    //******************************************************************************
    void apply_simd_intrinsics(ImageRGB &input, 
                               Filter &filter,
                               ImageRGB &output) 
    {
        if (input != output) {
            std::cout << "ERROR: input and output dimensions do not match" << std::endl;
            REPORT_ERR;
            exit(EXIT_FAILURE);
        }
    
        float *i_channels[ImageRGB::num_channels] = {input.get_red(), input.get_green(), input.get_blue()};
        float *o_channels[ImageRGB::num_channels] = {output.get_red(), output.get_green(), output.get_blue()};
        const float *filter_data = filter.get_data(); 


        const int k_half_width = filter.get_half_width();
        const int k_half_height = filter.get_half_height();
        const int k_total_size = filter.get_total_size();


        const int register_length = 8;
      
        CommandLineSettings* settings = CommandLineSettings::get_settings();

        auto start = steady_clock::now();
        
        // allocate registers to hold filters
        __m256 kernels[k_total_size];
        
        // load kernels to registers
        for (unsigned i = 0; i < k_total_size; ++i) {
            kernels[i] = _mm256_broadcast_ss(filter_data + i);
        }

        __m256 tmp_output;

        for (size_t repeat = 0; repeat < settings->get_num_repeats(); ++repeat) {
            for (size_t channel = 0; channel < ImageRGB::num_channels; ++channel) {
            
                for (size_t y = 0; y < input.get_height(); ++y) {
                    for (size_t x = 0; x < input.get_width(); x += register_length) {

                        tmp_output = _mm256_set1_ps(0.0f);
                        unsigned int counter = 0;
                        for (int yy = -k_half_height; yy <= k_half_height; ++yy) {
                            for (int xx = -k_half_width; xx <= k_half_width; ++xx) {
                                __m256 stripe = _mm256_loadu_ps(&i_channels[channel][input.get_index(x + xx, y + yy)]);
                                tmp_output = _mm256_fmadd_ps(stripe, kernels[counter], tmp_output);
                                ++counter;
                            }
                        }
                        _mm256_storeu_ps(&o_channels[channel][output.get_index(x, y)], tmp_output);
                    }
                }
            }
        }
        
        auto end = steady_clock::now();
        duration<double> elapse_time = end - start;
        std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
    }
}