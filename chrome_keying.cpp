#include <iostream>
#include <chrono> 

#include "helper.h"
#include "image.h"


using namespace std::chrono; 

const float threshold = 0.25;  // TODO: must be set from the command line

// 24 frams per second; 60 number of seconds
const size_t repeats = 24 * 60;  // TODO: must be set from the command line

enum COLORS {RED=0, GREEN=1, BLUE=2};

ImageRGB apply_default_ck(ImageRGB &fg, ImageRGB &bg) {
    ImageRGB output(bg);
    
    float *output_channels[ImageRGB::num_channels] = {output.get_red(), output.get_green(), output.get_blue()};
    float *fg_channels[ImageRGB::num_channels] = {fg.get_red(), fg.get_green(), fg.get_blue()};

    auto start = system_clock::now();
    for (size_t count = 0; count < repeats; ++count) {
        for (size_t i = 0; i < output.get_total_area(); ++i) {
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

    return output;
}