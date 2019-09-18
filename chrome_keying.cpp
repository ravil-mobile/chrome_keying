#include <iostream>
#include <chrono> 
#include <immintrin.h>

#include "compute_kernels.h"


using namespace std::chrono; 


//******************************************************************************
void apply_default_ck(ImageRGB &fg, ImageRGB &output) {
    
    float *output_channels[ImageRGB::num_channels] = {output.get_red(), output.get_green(), output.get_blue()};
    float *fg_channels[ImageRGB::num_channels] = {fg.get_red(), fg.get_green(), fg.get_blue()};

    CommandLineSettings* settings = CommandLineSettings::get_settings();
    const float threshold = settings->get_ck_threshold();

    auto start = system_clock::now();
    for (size_t count = 0; count < settings->get_num_repeats(); ++count) {
        for (size_t i = 0; i < output.get_real_total_area(); ++i) {
            const bool is_green = (fg_channels[GREEN][i] > fg_channels[RED][i])
                                   && (fg_channels[GREEN][i] > fg_channels[BLUE][i]) 
                                   && (fg_channels[GREEN][i] > threshold);
            if (!is_green) {
                output_channels[RED][i] = fg_channels[RED][i];
                output_channels[GREEN][i] = fg_channels[GREEN][i];
                output_channels[BLUE][i] = fg_channels[BLUE][i];
            }
        }
    }

    auto end = std::chrono::system_clock::now();
    duration<double> elapse_time = end - start;
    std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
}


//******************************************************************************
void apply_openmp_ck(ImageRGB &fg, ImageRGB &output) {
    
    float *output_channels[ImageRGB::num_channels] = {output.get_red(), output.get_green(), output.get_blue()};
    float *fg_channels[ImageRGB::num_channels] = {fg.get_red(), fg.get_green(), fg.get_blue()};

    CommandLineSettings* settings = CommandLineSettings::get_settings();
    const float threshold = settings->get_ck_threshold();

    auto start = system_clock::now();
    for (size_t count = 0; count < settings->get_num_repeats(); ++count) {

        #pragma omp simd 
        for (size_t i = 0; i < output.get_real_total_area(); ++i) {
            const bool is_green = (fg_channels[GREEN][i] > fg_channels[RED][i])
                                && (fg_channels[GREEN][i] > fg_channels[BLUE][i]) 
                                && (fg_channels[GREEN][i] > threshold);
            if (!is_green) {

                output_channels[RED][i] = fg_channels[RED][i];
                output_channels[GREEN][i] = fg_channels[GREEN][i];
                output_channels[BLUE][i] = fg_channels[BLUE][i];
            }
        }
    }

    auto end = std::chrono::system_clock::now();
    duration<double> elapse_time = end - start;
    std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
}




//******************************************************************************
void apply_simd_intrinsics_ck(ImageRGB &fg, ImageRGB &output) {
    
    float *output_channels[ImageRGB::num_channels] = {output.get_red(), output.get_green(), output.get_blue()};
    float *fg_channels[ImageRGB::num_channels] = {fg.get_red(), fg.get_green(), fg.get_blue()};

    CommandLineSettings* settings = CommandLineSettings::get_settings();
    const float threshold = settings->get_ck_threshold();

    const unsigned stride = 32 / sizeof(float);

    auto start = system_clock::now();
    __m256 level = _mm256_broadcast_ss(&threshold);
    for (size_t count = 0; count < settings->get_num_repeats(); ++count) {
        for (size_t i = 0; i < output.get_real_total_area(); i += stride) {


            // load data to registers
            __m256 green = _mm256_loadu_ps(fg_channels[GREEN] + i);
            __m256 red = _mm256_loadu_ps(fg_channels[RED] + i);
            __m256 blue = _mm256_loadu_ps(fg_channels[BLUE] + i);

            //compare colors
            __m256 tmp0 = _mm256_cmp_ps(red, green, _CMP_GT_OQ);
            __m256 tmp1 = _mm256_cmp_ps(blue, green, _CMP_GT_OQ);
            __m256 tmp2 = _mm256_cmp_ps(level, green, _CMP_GT_OQ);

            __m256 tmp3 = _mm256_or_ps(tmp0, tmp1);
            __m256 tmp4 = _mm256_or_ps(tmp2, tmp3);

            // compute store mask
            __m256i mask = _mm256_cvtps_epi32(tmp4);

            // write data back
            _mm256_maskstore_ps(output_channels[RED] + i, mask, red);
            _mm256_maskstore_ps(output_channels[GREEN] + i, mask, green);
            _mm256_maskstore_ps(output_channels[BLUE] + i, mask, blue);
        }
    }

    auto end = std::chrono::system_clock::now();
    duration<double> elapse_time = end - start;
    std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
}