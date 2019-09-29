#include <iostream>
#include <chrono>

#include "helper.h"
#include "image.h"
#include "filter.h"

using namespace std::chrono;

namespace keying {
    void apply_opencl(ImageRGB &fg, ImageRGB &output) {
        auto start = steady_clock::now();
        
        
        std::cout << "it has not been implemented" << std::endl;
        
        
        auto end = std::chrono::steady_clock::now();
        duration<double> elapse_time = end - start;
        std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
    }
}

namespace conv {
    void apply_opencl(ImageRGB &input, Filter &filter, ImageRGB &output) {
        auto start = steady_clock::now();
        
        
        std::cout << "it has not been implemented" << std::endl;
        
        
        auto end = std::chrono::steady_clock::now();
        duration<double> elapse_time = end - start;
        std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
    }
}