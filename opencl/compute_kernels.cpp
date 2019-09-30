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
        const cl::Device *device = reinterpret_cast<const cl::Device*>(DeviceSetup::get_device_ptr());
        

        // allocate buffers in order to transfer data
        const cl::Context *context = reinterpret_cast<const cl::Context*>(DeviceSetup::get_context_ptr());
        const unsigned area = static_cast<unsigned>(fg.get_real_total_area());
        cl::Buffer d_fg_red(*context, CL_MEM_READ_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;
        cl::Buffer d_fg_green(*context, CL_MEM_READ_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;
        cl::Buffer d_fg_blue(*context, CL_MEM_READ_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;

        cl::Buffer d_out_red(*context, CL_MEM_READ_WRITE, area * sizeof(float), NULL, &cl_status); CL_CHECK;
        cl::Buffer d_out_green(*context, CL_MEM_READ_WRITE, area * sizeof(float), NULL, &cl_status); CL_CHECK;
        cl::Buffer d_out_blue(*context, CL_MEM_READ_WRITE, area * sizeof(float), NULL, &cl_status); CL_CHECK;


        // init kernel
        const cl::Program *program = reinterpret_cast<const cl::Program*>(DeviceSetup::get_program_ptr());
        CommandLineSettings* settings = CommandLineSettings::get_settings();
        const float threshold = settings->get_ck_threshold();

        cl::Kernel kernel(*program, "kernel_apply_keying", &cl_status); CL_CHECK;
        cl_status = kernel.setArg(0, d_fg_red); CL_CHECK;
        cl_status = kernel.setArg(1, d_fg_green); CL_CHECK;
        cl_status = kernel.setArg(2, d_fg_blue); CL_CHECK;
        cl_status = kernel.setArg(3, d_out_red); CL_CHECK;
        cl_status = kernel.setArg(4, d_out_green); CL_CHECK;
        cl_status = kernel.setArg(5, d_out_blue); CL_CHECK;

        cl_status = kernel.setArg(6, sizeof(float), &threshold); CL_CHECK;
        cl_status = kernel.setArg(7, sizeof(unsigned), &area); CL_CHECK;


        auto start = steady_clock::now();

        cl::CommandQueue queue(*context, *device, 0, &cl_status); CL_CHECK;

        // transfer data to a device
        cl_status = queue.enqueueWriteBuffer(d_fg_red, CL_FALSE, 0, area * sizeof(float), fg.get_red()); CL_CHECK;
        cl_status = queue.enqueueWriteBuffer(d_fg_green, CL_FALSE, 0, area * sizeof(float), fg.get_green()); CL_CHECK;
        cl_status = queue.enqueueWriteBuffer(d_fg_blue, CL_FALSE, 0, area * sizeof(float), fg.get_blue()); CL_CHECK;

        cl_status = queue.enqueueWriteBuffer(d_out_red, CL_FALSE, 0, area * sizeof(float), out.get_red()); CL_CHECK;
        cl_status = queue.enqueueWriteBuffer(d_out_green, CL_FALSE, 0, area * sizeof(float), out.get_green()); CL_CHECK;
        cl_status = queue.enqueueWriteBuffer(d_out_blue, CL_FALSE, 0, area * sizeof(float), out.get_blue()); CL_CHECK;
        
        // wait until the data is transferred
        cl_status = queue.finish(); CL_CHECK;

        for (size_t count = 0; count < settings->get_num_repeats(); ++count) {
            cl_status = queue.enqueueNDRangeKernel(kernel, 
                                                cl::NDRange(), 
                                                cl::NDRange(cl_norm_size(fg.get_pad_width() * fg.get_pad_height(), 32)), 
                                                cl::NDRange(32), 
                                                NULL, 
                                                NULL); CL_CHECK;
            cl_status = queue.finish(); CL_CHECK;
        }      

        // fet data from the device
        cl_status = queue.enqueueReadBuffer(d_out_red, CL_FALSE, 0, area * sizeof(float), out.get_red()); CL_CHECK;
        cl_status = queue.enqueueReadBuffer(d_out_green, CL_FALSE, 0, area * sizeof(float), out.get_green()); CL_CHECK;
        cl_status = queue.enqueueReadBuffer(d_out_blue, CL_FALSE, 0, area * sizeof(float), out.get_blue()); CL_CHECK;
        
        // wait until the queue is empty
        cl_status = queue.finish(); CL_CHECK;

        auto end = std::chrono::steady_clock::now();
        duration<double> elapse_time = end - start;
        std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
    }
}




namespace conv {
    void apply_opencl(ImageRGB &in, Filter &filter, ImageRGB &out) {

        cl_status = 0;
        const cl::Device *device = reinterpret_cast<const cl::Device*>(DeviceSetup::get_device_ptr());
        
        
        // allocate buffers in order to transfer data
        const cl::Context *context = reinterpret_cast<const cl::Context*>(DeviceSetup::get_context_ptr());
        const unsigned area = static_cast<unsigned>(in.get_real_total_area());
        cl::Buffer d_in_red(*context, CL_MEM_READ_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;
        cl::Buffer d_in_green(*context, CL_MEM_READ_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;
        cl::Buffer d_in_blue(*context, CL_MEM_READ_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;


        cl::Buffer d_out_red(*context, CL_MEM_WRITE_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;
        cl::Buffer d_out_green(*context, CL_MEM_WRITE_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;
        cl::Buffer d_out_blue(*context, CL_MEM_WRITE_ONLY, area * sizeof(float), NULL, &cl_status); CL_CHECK;


        const unsigned filter_size = static_cast<unsigned>(filter.get_total_size());
        cl::Buffer d_filter(*context, CL_MEM_READ_ONLY, filter_size * sizeof(float), NULL, &cl_status); CL_CHECK;

        
        // init kernel
        const cl::Program *program = reinterpret_cast<const cl::Program*>(DeviceSetup::get_program_ptr());
        CommandLineSettings* settings = CommandLineSettings::get_settings();
        const float threshold = settings->get_ck_threshold();

#ifndef SR_MEM 
    cl::Kernel kernel(*program, "kernel_apply_conv", &cl_status); CL_CHECK;
#else
    cl::Kernel kernel(*program, "kernel_apply_conv_shmem", &cl_status); CL_CHECK;    
#endif
        // load parameters to the kernel
        cl_status = kernel.setArg(0, d_in_red); CL_CHECK;
        cl_status = kernel.setArg(1, d_in_green); CL_CHECK;
        cl_status = kernel.setArg(2, d_in_blue); CL_CHECK;
        cl_status = kernel.setArg(3, d_out_red); CL_CHECK;
        cl_status = kernel.setArg(4, d_out_green); CL_CHECK;
        cl_status = kernel.setArg(5, d_out_blue); CL_CHECK;

        unsigned image_width = static_cast<unsigned>(in.get_width());
        unsigned image_height = static_cast<unsigned>(in.get_height());
        cl_status = kernel.setArg(6, sizeof(unsigned), &image_width); CL_CHECK;
        cl_status = kernel.setArg(7, sizeof(unsigned), &image_height); CL_CHECK;

        cl_status = kernel.setArg(8, d_filter); CL_CHECK;
        
        unsigned filter_width = static_cast<unsigned>(filter.get_width());
        unsigned filter_height = static_cast<unsigned>(filter.get_height());
        cl_status = kernel.setArg(9, sizeof(unsigned), &filter_width); CL_CHECK;
        cl_status = kernel.setArg(10, sizeof(unsigned), &filter_height); CL_CHECK;
       


        auto start = steady_clock::now();

        cl::CommandQueue queue(*context, *device, 0, &cl_status); CL_CHECK;

        // transfer data to a device
        cl_status = queue.enqueueWriteBuffer(d_in_red, CL_FALSE, 0, area * sizeof(float), in.get_red()); CL_CHECK;
        cl_status = queue.enqueueWriteBuffer(d_in_green, CL_FALSE, 0, area * sizeof(float), in.get_green()); CL_CHECK;
        cl_status = queue.enqueueWriteBuffer(d_in_blue, CL_FALSE, 0, area * sizeof(float), in.get_blue()); CL_CHECK;

        cl_status = queue.enqueueWriteBuffer(d_out_red, CL_FALSE, 0, area * sizeof(float), out.get_red()); CL_CHECK;
        cl_status = queue.enqueueWriteBuffer(d_out_green, CL_FALSE, 0, area * sizeof(float), out.get_green()); CL_CHECK;
        cl_status = queue.enqueueWriteBuffer(d_out_blue, CL_FALSE, 0, area * sizeof(float), out.get_blue()); CL_CHECK;

        cl_status = queue.enqueueWriteBuffer(d_filter, CL_FALSE, 0, filter_size * sizeof(float), filter.get_data()); CL_CHECK;
        
        // wait until the data is transferred
        cl_status = queue.finish(); CL_CHECK;

        // prepare local and global dimensions
        const unsigned local_x = 32;
        const unsigned local_y = 4;
        const unsigned global_x = cl_norm_size(in.get_width(), local_x);
        const unsigned global_y = cl_norm_size(in.get_height(), local_y);
        
#ifdef SR_MEM 
        // allocate share memory for a patch
        const unsigned patch_size = (local_x + filter.get_width()) * (local_y + filter.get_height());
        cl_status = kernel.setArg(11, patch_size * sizeof(float), NULL); CL_CHECK;
#endif
        for (size_t count = 0; count < settings->get_num_repeats(); ++count) {
            
            // execute kernel
            cl_status = queue.enqueueNDRangeKernel(kernel, 
                                                   cl::NDRange(), 
                                                   cl::NDRange(global_x, global_y), 
                                                   cl::NDRange(local_x, local_y), 
                                                   NULL, 
                                                   NULL);
            cl_status = queue.finish(); CL_CHECK;
        }

        // fet data from the device
        cl_status = queue.enqueueReadBuffer(d_out_red, CL_FALSE, 0, area * sizeof(float), out.get_red()); CL_CHECK;
        cl_status = queue.enqueueReadBuffer(d_out_green, CL_FALSE, 0, area * sizeof(float), out.get_green()); CL_CHECK;
        cl_status = queue.enqueueReadBuffer(d_out_blue, CL_FALSE, 0, area * sizeof(float), out.get_blue()); CL_CHECK;
        
        // wait until the queue is empty
        cl_status = queue.finish(); CL_CHECK;

        auto end = std::chrono::steady_clock::now();
        duration<double> elapse_time = end - start;
        std::cout << "time spent: " << elapse_time.count() << ", sec" << std::endl;
    }
}