#include <iostream>
#include <string>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "helper.h"
#include "image.h"
#include "filter.h"
#include "compute_kernels.h"
#include "compute_kernels_default.h"

#include "program_title.h"

int main(int argc, char *argv[])
{
    po::variables_map map;
    try
    {

        po::options_description desc(TITLE);
        
        // define command line options
        desc.add_options()
            ("help,h", "print program description")
            ("input,i", po::value<std::string>()->default_value("../images/input-2.bmp"), "path to an input image")
            ("backgound,b", po::value<std::string>()->default_value("../images/background.bmp"), "path to n background image")
            ("threshold,t", po::value<float>()->default_value(0.25), "chrome keying sensetivity")
            ("width,m", po::value<unsigned>()->default_value(3), "width of gaussian filter")
            ("height,k", po::value<unsigned>()->default_value(3), "height of gaussian filter")
            ("repeats,r", po::value<unsigned>()->default_value(720), "number of repeats to measure performance")
            ("platform,p", po::value<std::string>()->default_value("intel gpu"), "platform name: intel gpu, nvidia")
            ("mode", po::value<std::string>()->default_value("default"), "modes: default, opencl")
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
        std::cout << "target platform: " << map["platform"].as<std::string>() << std::endl;
        std::cout << "mode: " << map["mode"].as<std::string>() << std::endl;
        std::cout << std::string(70, '-') << std::endl << std::endl;

    } catch(const po::error err) {
        std::cout << "ERROR: during command line parsing. Please, read the documentation (--help)" << std::endl;
        std::cout << err.what() << std::endl;
        exit(EXIT_FAILURE); 
    }
    
    Filter filter(map["width"].as<unsigned>(), map["height"].as<unsigned>());
    filter.init_with_gaussian();


    CommandLineSettings::init(map["repeats"].as<unsigned>(), 
                              map["threshold"].as<float>(),
                              map["platform"].as<std::string>());

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


    Graphics *graphics = Graphics::init_graphics();

    // display images
#ifdef DEBUG  //DEBUGGING
    graphics->display_window(foreground);
    graphics->display_window(background);
#endif

    ImageRGB chrome_keying_output(background);
    ImageRGB convolution_output(background);

    std::string mode(map["mode"].as<std::string>());
    mode = "opencl"; // DEBUGGING
    if(!mode.compare(std::string("default"))) {
        keying::apply_default(foreground, chrome_keying_output);
        conv::apply_default(chrome_keying_output, filter, convolution_output);
    }
    else if(!mode.compare(std::string("opencl"))) {
        keying::apply_opencl(foreground, chrome_keying_output);
        conv::apply_opencl(chrome_keying_output, filter, convolution_output);
    }
    else {
        std::cout << "ERROR: unknwon mode. Please, read the documentation (--help)" << std::endl;
        exit(EXIT_FAILURE); 
    }

    graphics->display_window(chrome_keying_output);
    // graphics->display_window(convolution_output); //DEBUGGING
	return 0;
}