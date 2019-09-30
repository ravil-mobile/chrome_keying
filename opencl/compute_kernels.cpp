#include <iostream>
#include <chrono>
#include <CL/cl.hpp>

#include "helper.h"
#include "image.h"
#include "filter.h"
#include "device_setup.h"
#include "opencl_helper.h"

using namespace std::chrono;

extern cl_int cl_status;
namespace keying {
    void apply_opencl(ImageRGB &fg, ImageRGB &out) {

        cl_status = 0;
        const cl::Program *program = reinterpret_cast<const cl::Program*>(DeviceSetup::get_program_ptr());
        const cl::Context *context = reinterpret_cast<const cl::Context*>(DeviceSetup::get_context_ptr());
        const cl::Device *device = reinterpret_cast<const cl::Device*>(DeviceSetup::get_device_ptr());


        cl::Kernel kernel(*program, "mult_vectors", &cl_status); CL_CHECK;
        cl::CommandQueue command(*context, *device, 0, &cl_status); CL_CHECK;

        const size_t area = fg.get_real_total_area();
        cl::Buffer d_fg_red(*context, CL_TRUE, area * sizeof(float), fg.get_red(), &cl_status); CL_CHECK;
        cl::Buffer d_fg_green(*context, CL_TRUE, area * sizeof(float), fg.get_green(), &cl_status); CL_CHECK;
        cl::Buffer d_fg_blue(*context, CL_TRUE, area * sizeof(float), fg.get_blue(), &cl_status); CL_CHECK;

        cl::Buffer d_out_red(*context, CL_TRUE, area * sizeof(float), out.get_red(), &cl_status); CL_CHECK;
        cl::Buffer d_out_green(*context, CL_TRUE, area * sizeof(float), out.get_green(), &cl_status); CL_CHECK;
        cl::Buffer d_out_blue(*context, CL_TRUE, area * sizeof(float), out.get_blue(), &cl_status); CL_CHECK;



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