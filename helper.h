#include <string>
#include "image.h"

#ifndef _HELPER_H_
#define _HELPER_H_

#define REPORT_ERR report_error(__FILE__, __LINE__)

void report_error(const char* file, size_t string_number);
ImageRGB read_bmp_image(std::string path);
void display_window(ImageRGB &image);

#endif  // _HELPER_H_