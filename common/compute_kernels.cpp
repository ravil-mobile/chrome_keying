#include <iostream>
#include <chrono> 
#include <immintrin.h>

#include "compute_kernels_default.h"


using namespace std::chrono; 

namespace keying {
    void apply_default(ImageRGB &fg, ImageRGB &output) {
        
        float *output_channels[ImageRGB::num_channels] = {output.get_red(), output.get_green(), output.get_blue()};
        float *fg_channels[ImageRGB::num_channels] = {fg.get_red(), fg.get_green(), fg.get_blue()};

        CommandLineSettings* settings = CommandLineSettings::get_settings();
        const float threshold = settings->get_ck_threshold();

        auto start = steady_clock::now();
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

        auto end = steady_clock::now();
        duration<double> elapse_time = end - start;
        std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
    }
}


namespace conv {
    void apply_default(ImageRGB &input, 
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

        CommandLineSettings* settings = CommandLineSettings::get_settings();
        
        auto start = steady_clock::now();
        for (size_t repeat = 0; repeat < settings->get_num_repeats(); ++repeat) {
            for (size_t channel = 0; channel < ImageRGB::num_channels; ++channel) {
                for (size_t y = 0; y < input.get_height(); ++y ) {
                    for (size_t x = 0; x < input.get_width(); ++x) {

                        float value = 0.0f;
                        for (int yy = -k_half_height; yy <= k_half_height; ++yy) {
                            for (int xx = -k_half_width; xx <= k_half_width; ++xx) {
                                
                                value += (i_channels[channel][input.get_index(x + xx, y + yy)] \
                                        * filter_data[filter.get_rel_index(xx, yy)]);
                            }
                        }
                        o_channels[channel][output.get_index(x, y)] = value;
                        
                    }
                }
            }
        }
        
        auto end = steady_clock::now();
        duration<double> elapse_time = end - start;
        std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
    }
}