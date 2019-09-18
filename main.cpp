#include <iostream>
#include <string>

#include <GLFW/glfw3.h>
#include <GL/gl.h>


#include "helper.h"
#include "image.h"
#include "filter.h"
#include "compute_kernels.h"

enum MODE {DEFAULT, OPENMP, INTRINSICS};

int main(int argc, char *argv[])
{
    // reading the command line
    MODE mode = DEFAULT;
    Filter filter(3, 3);
    filter.init_with_gaussian();
    std::string background_image_path("../images/input-2.bmp");
    std::string foreground_image_path("../images/background.bmp");
    const float threshold = 0.25;
    const size_t repeats = 24 * 30;


    CommandLineSettings::init(repeats, threshold);

    // load images
    ImageRGB foreground = read_bmp_image(background_image_path, 
                                        filter.get_half_width(), 
                                        filter.get_half_height()); // TODO: must be set from the command line
    
    ImageRGB background = read_bmp_image(foreground_image_path, 
                                         filter.get_half_width(), 
                                         filter.get_half_height()); // TODO: must be set from the command line


    // check whether images have the same size
    if (foreground != background) {
        std::cout << "ERROR: foreground and bacjground have different sizes" << std::endl;
        std::cout << "widht (fg/bg): " << foreground.get_width() << "\\" << background.get_width() << std::endl;
        std::cout << "height (fg/bg): " << foreground.get_height() << "\\" << background.get_height() << std::endl;
        REPORT_ERR;
        exit(EXIT_FAILURE);
    }


    // init OpenGL to render graphics 
	if(!glfwInit()){
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}


    // display images
    display_window(foreground);
    display_window(background);


    ImageRGB chrome_keying_output(background);
    ImageRGB convolution_output(background);
    switch(mode) {
        case DEFAULT: {std::cout << "MODE: default" << std::endl;
                       apply_default_ck(foreground, chrome_keying_output);
                       convolve_default(chrome_keying_output, filter, convolution_output);
                       break;}

        case OPENMP: {std::cout << "MODE: openmp" << std::endl;
                      apply_openmp_ck(foreground, chrome_keying_output);
                      // TODO: convolve_openmp_ck(chrome_keying_output, filter, convolution_output);
                      break;}

        case INTRINSICS: {std::cout << "MODE: intrinsics" << std::endl;
                          apply_simd_intrinsics_ck(foreground, chrome_keying_output);
                          convolve_simd_intrinsics(chrome_keying_output, filter, convolution_output);
                          break;}

        default: {std::cout << "ERROR: unknown running mode" << std::endl; 
                  exit(EXIT_FAILURE);}
    }

    display_window(chrome_keying_output);
    display_window(convolution_output);

	// Terminate OpenGL
	glfwTerminate();

	return 0;
}