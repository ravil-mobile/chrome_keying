#include <string>
#include <assert.h>
#include "image.h"

#ifndef _HELPER_H_
#define _HELPER_H_

#define REPORT_ERR report_error(__FILE__, __LINE__)

void report_error(const char* file, size_t string_number);
ImageRGB read_bmp_image(std::string path, size_t pad_x = 0, size_t pad_y = 0);
void display_window(ImageRGB &image);
enum COLORS {RED=0, GREEN=1, BLUE=2};


/** TODO: add description
 * NOTE: the class is a singelton
 */
class CommandLineSettings {
public:

    ~CommandLineSettings() {
        if (m_isntance != nullptr) {delete m_isntance;}
    }

    size_t get_num_repeats() {return m_num_repeats;}
    float get_ck_threshold() {return m_ck_threshold;}

    static CommandLineSettings* get_settings() {
        assert((m_isntance != nullptr) && ("the has not been initialized"));
        return m_isntance;
    }

    static void init(size_t repeats, float threshold) {
        if (m_isntance == nullptr) {
            m_isntance = new CommandLineSettings(repeats, threshold);
        }
    }

private:
    CommandLineSettings(size_t repeats, float threshold) : m_num_repeats(repeats), 
                                                           m_ck_threshold(threshold) {}

    CommandLineSettings(const CommandLineSettings& other) {}
    CommandLineSettings& operator=(const CommandLineSettings& other) {return *this;}

    size_t m_num_repeats;
    float m_ck_threshold;
    static CommandLineSettings* m_isntance;
};

#endif  // _HELPER_H_