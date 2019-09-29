#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <CL/cl.hpp>

#include "helper.h"
#include "opencl_helper.h"
#include "device_setup.h"

extern cl_int cl_status;

DeviceSetup *DeviceSetup::m_setup = nullptr;

void DeviceSetup::init_device() {
    if (m_setup == nullptr) {
        m_setup = new DeviceSetup();
    }
}


DeviceSetup::DeviceSetup() : m_device_ptr(nullptr) {

    cl_status = 0;

    std::string platform_name = CommandLineSettings::get_settings()->get_platform_name();
    
    // find platforms
    std::vector<cl::Platform> platforms;   
    cl_status = cl::Platform::get(&platforms); CL_CHECK;

    //  find device the first platform with the requested name
    std::regex self_regex(platform_name , std::regex_constants::grep | std::regex_constants::icase);
    int platform_id = -1; unsigned counter = 0;
    for (auto platform: platforms) {
        if (std::regex_search(platform.getInfo<CL_PLATFORM_NAME>(), self_regex)) {
            platform_id = counter;
            break;
        }
        ++counter;
    }

    if (platform_id == -1) {
        std::cout << "ERROR: the requested platform has not been found" << std::endl;
        std::cout << std::string(20, '-') << "List of available platforms" << std::string(20, '-') << std::endl;
        for (auto platform: platforms)
            std::cout << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

        exit(EXIT_FAILURE); 
    }


    // get all devices of the target platform
    std::vector<cl::Device> devices;
    cl_status = platforms[platform_id].getDevices(CL_DEVICE_TYPE_GPU, &devices); CL_CHECK;
    if (devices.empty()) {
        std::cout << "ERROR: the requested platform doesn't contain an GPU instance" << std::endl;
        exit(EXIT_FAILURE); 
    }

    // store the first device of the target platform for the subsequent usage
    m_device_ptr = reinterpret_cast<void*>(new cl::Device(devices[0]));
    
    // create an OpenCL context for the first device
    cl::Context context(devices[0], NULL, NULL, NULL, &cl_status);  CL_CHECK;


    // open and read a text file with opencl kernels
    std::string source_code;
    {
        // open a text file
        std::string file_name("../opencl/opencl_kernels.cl");
        std::ifstream file(file_name);
        if (!file.is_open()) {
            std::cout << "ERROR: cannot open the opencl kernel-file: " << file_name << std::endl;
            exit(EXIT_FAILURE);
            abort();
        }

        // compute the file length
        file.seekg(0, file.end);
        size_t file_length = file.tellg();
        file.seekg(0, file.beg);

        // read file content
        char *content = new char[file_length + 1];
        file.read(content, file_length);
        content[file_length + 1] = '\0';
        file.close();

        // put the content to a string
        source_code = content;
    }

    // compile opencl kernels (JIT)
    cl::Program program(context, source_code, &cl_status); CL_CHECK;
    cl_status = program.build(devices, NULL, NULL, NULL); CL_CHECK;

    // check compilation log
    if (cl_status != CL_SUCCESS) { 
        std::cout << program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &cl_status) << std::endl; CL_CHECK;
    }

    // TODO: continue...
}


DeviceSetup::~DeviceSetup() {
    if (m_setup != nullptr)  {
        delete m_setup;
        delete reinterpret_cast<cl::Device*>(m_device_ptr);
    }
}

unsigned int DeviceSetup::getDevice() {
    return 0;
}
