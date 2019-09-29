#include <iostream>
#include <string>
#include <CL/cl.hpp>

#ifndef OPENCL_HELPER_H_
#define OPENCL_HELPER_H_


#define CL_CHECK check_error_code(cl_status, __FILE__, __LINE__)

void check_error_code(cl_int status, std::string file_name, size_t string_number);
const char *getErrorString(cl_int error);

#endif  // OPENCL_HELPER_H_