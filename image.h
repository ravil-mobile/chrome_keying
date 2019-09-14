#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdlib.h>
#include <cstring>

/** TODO: write documentation
 * 
 */
class ImageRGB{
public:
    ImageRGB(size_t height, size_t width, size_t padded_width = 0) : m_red(nullptr), 
                                                                     m_green(nullptr), 
                                                                     m_blue(nullptr),
                                                                     m_height(height),
                                                                     m_width(width),
                                                                     m_padded_width(padded_width) 
    {
        if (padded_width == 0)
            m_padded_width = width;
        
        m_red = (float*)aligned_alloc(this->alignment, sizeof(float) * this->get_total_area());
        m_green = (float*)aligned_alloc(this->alignment, sizeof(float) * this->get_total_area());
        m_blue = (float*)aligned_alloc(this->alignment, sizeof(float) * this->get_total_area());

    }

    ImageRGB(const ImageRGB &other)  : m_red(nullptr), 
                                       m_green(nullptr), 
                                       m_blue(nullptr) {
        *this = other;        
    }

    ImageRGB& operator=(const ImageRGB& other) {
        if (this != &other) {
            m_height = other.m_height;
            m_width = other.m_width;
            m_padded_width = other.m_padded_width;

            if (m_red != nullptr) free(m_red);
            if (m_green != nullptr) free(m_green);
            if (m_blue != nullptr) free(m_blue);

            m_red = (float*)aligned_alloc(this->alignment, sizeof(float) * this->get_total_area());
            m_green = (float*)aligned_alloc(this->alignment, sizeof(float) * this->get_total_area());
            m_blue = (float*)aligned_alloc(this->alignment, sizeof(float) * this->get_total_area());

            std::memcpy((void*)m_red, other.m_red, this->get_total_area() * sizeof(float));
            std::memcpy((void*)m_green, other.m_green, this->get_total_area() * sizeof(float));
            std::memcpy((void*)m_blue, other.m_blue, this->get_total_area() * sizeof(float));
        }
        return *this;
    }


    bool operator==(const ImageRGB& rhs) {
        if ((m_height == rhs.m_height) && (m_width == rhs.m_width) && (m_padded_width == rhs.m_padded_width)) {
            return true;
        }
        else {
            return false;
        }
    }

    bool operator!=(const ImageRGB& rhs) {
        return !((*this) == rhs);

    }

    ~ImageRGB() {
        if (m_red != nullptr) free(m_red);
        if (m_green != nullptr) free(m_green);
        if (m_blue != nullptr) free(m_blue);
    }

    size_t get_height() {return m_height;}
    size_t get_width() {return m_width;}
    size_t get_padded_width() {return m_padded_width;}

    float* get_red() {return m_red;}
    float* get_green() {return m_green;}
    float* get_blue() {return m_blue;}

    size_t get_index(size_t x, size_t y) {
        return x + y * m_padded_width;
    }

    size_t get_total_area() {
        return m_padded_width * m_height;
    }

    const static short num_channels = 3;
    const static size_t alignment = 64; // bytes

private:

    size_t m_height;
    size_t m_width;
    size_t m_padded_width;

    float* m_red;
    float* m_green;
    float* m_blue;
};

#endif  // _IMAGE_H_