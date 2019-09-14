#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <errno.h>
#include <cstring>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "helper.h"

typedef unsigned char byte; 
const unsigned bmp_header_size = 54;

void report_error(const char* file, size_t string_number) {
    std::cout << "REASON: " <<  strerror(errno) << std::endl;
    std::cout << "OCCURED at: " << file << "::" << string_number << std::endl;
}

/** Read a bmp file and extracts an image
 * 
 * @param path a relative location of an image
 */
ImageRGB read_bmp_image(std::string path) {

    // open a file
    std::fstream file(path.c_str(), std::ios_base::in | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "ERROR: cannot open a file: " << path << std::endl; REPORT_ERR;
        exit(EXIT_FAILURE);
    } 

    // read header
    byte header[bmp_header_size];
    file.read((char*)header, bmp_header_size);

    // check whether a file is bmp or not
    if ((header[0] != 'B') && (header[1] != 'M')) {
        std::cout << "ERROR: file doesn't belong to the BMP format" << std::endl; REPORT_ERR;
        file.close();
        exit(EXIT_FAILURE);
    }

    // check whether a bmp file has been compressed or not
    const unsigned compression = *reinterpret_cast<unsigned*>(header + 30);
    if (compression) {
        std::cout << "ERROR: file is compressed: " << "cannot work with compressed bmp files"<< std::endl; REPORT_ERR;
        file.close();
        exit(EXIT_FAILURE);
    }

    // check whether an image is encoded according to 8-bit color format
    const unsigned bits_per_pixel = *reinterpret_cast<short*>(header + 28);
    if (bits_per_pixel != (8 * ImageRGB::num_channels)) {
        std::cout << "ERROR: an image is not encoded as 8-bit color format" << std::endl; REPORT_ERR;
        file.close();
        exit(EXIT_FAILURE);
    }

    // retrieve info from the header 

    const unsigned pixel_array_offset = *reinterpret_cast<unsigned*>(header + 10);
    const unsigned image_size = *reinterpret_cast<unsigned*>(header + 34);
    const unsigned height = *reinterpret_cast<unsigned*>(header + 22);
    const unsigned width = *reinterpret_cast<unsigned*>(header + 18);
    const unsigned padded_width = image_size / (3 * height);
    ImageRGB image(height, width, padded_width);

    // allocate arrays for each channel 
    float *red = image.get_red();
    float *green = image.get_green();
    float *blue = image.get_blue();
      

    // change the current position to the pixel array within a file
    file.seekg(pixel_array_offset, file.beg);

    // read pixel array
    const size_t pixel_array_size = ImageRGB::num_channels * image.get_total_area();
    byte pixel_array[pixel_array_size];
    file.read((char*)pixel_array, pixel_array_size);

    // close file
    file.close(); 

    // read data from each channel
    for (size_t bmp_index = 0, rgb_index = 0; bmp_index < pixel_array_size; bmp_index += 3, ++rgb_index) {
        blue[rgb_index] = float(pixel_array[bmp_index]) / 255.0;
        green[rgb_index] = float(pixel_array[bmp_index + 1]) / 255.0;
        red[rgb_index] = float(pixel_array[bmp_index + 2]) / 255.0;
    }


#ifdef DEBUG
    std::cout << "offset to PixelArray: " << pixel_array_offset << std::endl;
    std::cout << "image width: " << image.get_width() << std::endl;
    std::cout << "padded width: " << image.get_padded_width() << std::endl;
    std::cout << "image height: " << image.get_height() << std::endl;
    std::cout << "image size: " << image_size << std::endl;
    std::cout << "color planes: " << *reinterpret_cast<short*>(header + 26) << std::endl;
    std::cout << "bits per pixel: " << bits_per_pixel << std::endl;
    std::cout << "image size: " << *reinterpret_cast<unsigned*>(header + 34) << std::endl;
    std::cout << "compression: " << *reinterpret_cast<unsigned*>(header + 30) << std::endl;
#endif

    return image;
}




/** displays a given image
 * TODO: complete
 * 
 */
void display_window(ImageRGB &image) {
    GLFWwindow* window;
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(image.get_width(), image.get_height(), "image", NULL, NULL );
	if (!window)
	{
		std::cout << "Failed to open GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// set up view
	glViewport(0, 0, image.get_width(), image.get_height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, image.get_width(), 0.0, image.get_height(), 0.0, 1.0); // this creates a canvas you can do 2D drawing on


	// Main loop
	while(!glfwWindowShouldClose(window))
	{
		// Draw gears
		//render_loop();
        float *channels[ImageRGB::num_channels] = {image.get_red(), image.get_green(), image.get_blue()};
        glBegin(GL_POINTS);
        for (int y = 0; y < image.get_height(); ++y) {
            for (int x = 0; x < image.get_width(); ++x) {
                size_t index = image.get_index(x, y);
                glColor3f(channels[0][index], channels[1][index], channels[2][index]);
                //glColor3f(0.0, 0.0, image.blue[index]);
                glVertex2f(x, y);
            }
        }
            
            glEnd();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    //DEBUGGING
    //glfwDestroyWindow(window);

}