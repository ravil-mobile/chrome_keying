#include <iostream>
#include <string>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "helper.h"
#include "image.h"
#include "filter.h"
#include "compute_kernels.h"


int main(int argc, char *argv[])
{
    po::variables_map map;
    try
    {

        po::options_description desc("Program options");
        
        // define command line options
        desc.add_options()
            ("help,h", "print program description")
            ("input,i", po::value<std::string>()->default_value("../images/input-2.bmp"), "path to an input image")
            ("backgound,b", po::value<std::string>()->default_value("../images/background.bmp"), "path to an background image")
            ("threshold,t", po::value<float>()->default_value(0.25), "chrome keying sensetivity")
            ("width,m", po::value<unsigned>()->default_value(3), "gaussian filter width")
            ("height,k", po::value<unsigned>()->default_value(3), "gaussian filter height")
            ("repeats,r", po::value<unsigned>()->default_value(720), "number of repeats to measure performance")
            ("mode", po::value<std::string>()->default_value("default"), "modes: default, openmp, intrinsics")
        ;

        // parse command line
        po::store(po::parse_command_line(argc, argv, desc), map);
        
        if (map.count("help")) {
            std::cout << desc << std::endl;
            return 1;
        }

        // print parameter settings and check whether the user provide correct data type for each parameter
        std::cout << std::string(30, '-') << "PARAMETERS" << std::string(30, '-') << std::endl;
        std::cout << "input image: " << map["input"].as<std::string>() << std::endl;
        std::cout << "backgound image: " << map["backgound"].as<std::string>() << std::endl;
        std::cout << "chrome-keying threshold: " << map["threshold"].as<float>() << std::endl;
        std::cout << "kernel width: " << map["width"].as<unsigned>() << std::endl;
        std::cout << "kernel height: " << map["height"].as<unsigned>() << std::endl;
        std::cout << "num repeats: " << map["repeats"].as<unsigned>() << std::endl;
        std::cout << "mode: " << map["mode"].as<std::string>() << std::endl;
        std::cout << std::string(70, '-') << std::endl << std::endl;

    } catch(const po::error err) {
        std::cout << err.what() << std::endl;
        exit(EXIT_FAILURE); 
    }
    
    Filter filter(map["width"].as<unsigned>(), map["height"].as<unsigned>());
    filter.init_with_gaussian();


    CommandLineSettings::init(map["repeats"].as<unsigned>(), map["threshold"].as<float>());

    // load images
    ImageRGB foreground = read_bmp_image(map["input"].as<std::string>(), 
                                        filter.get_half_width(), 
                                        filter.get_half_height());
    
    ImageRGB background = read_bmp_image(map["backgound"].as<std::string>(), 
                                         filter.get_half_width(), 
                                         filter.get_half_height());

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
#ifndef DEBUG
    display_window(foreground);
    display_window(background);
#endif

    ImageRGB chrome_keying_output(background);
    ImageRGB convolution_output(background);

    std::string mode(map["mode"].as<std::string>());
    if(!mode.compare(std::string("default"))) {
        keying::apply_default(foreground, chrome_keying_output);
        conv::apply_default(chrome_keying_output, filter, convolution_output);
    }
    else if(!mode.compare(std::string("openmp"))) {
        keying::apply_openmp(foreground, chrome_keying_output);
        // TODO: conv::apply_openmp(chrome_keying_output, filter, convolution_output);
    }
    else if (!mode.compare(std::string("intrinsics"))) {
        keying::apply_simd_intrinsics(foreground, chrome_keying_output);
        conv::apply_simd_intrinsics(chrome_keying_output, filter, convolution_output);
    }
    else {
        std::cout << "ERROR: unknwon mode. Please, see the documentation (--help)" << std::endl;
        exit(EXIT_FAILURE); 
    }


    display_window(chrome_keying_output);
    display_window(convolution_output);

	// Terminate OpenGL
	glfwTerminate();

	return 0;
}