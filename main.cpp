#include <iostream>
#include <string>

#include <GLFW/glfw3.h>
#include <GL/gl.h>


#include "helper.h"
#include "image.h"
#include "chrome_keying.h"

/* program entry */
int main(int argc, char *argv[])
{
	if( !glfwInit() )
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

    // load images
    ImageRGB foreground = read_bmp_image("../images/input-2.bmp");    // TODO: must be set from the command line
    ImageRGB background = read_bmp_image("../images/background.bmp");    // TODO: must be set from the command line


    // check whether images have the same size
    if (foreground != background) {
        std::cout << "ERROR: foreground and bacjground have different sizes" << std::endl;
        std::cout << "widht (fg/bg): " << foreground.get_width() << "\\" << background.get_width() << std::endl;
        std::cout << "height (fg/bg): " << foreground.get_height() << "\\" << background.get_height() << std::endl;
        REPORT_ERR;
        exit(EXIT_FAILURE);
    }

    // display images
    display_window(foreground);
    display_window(background);

    ImageRGB output = apply_default_ck(foreground, background);
    display_window(output);

	// Terminate GLFW
	glfwTerminate();

	return 0;
}