#ifndef _DEVICE_SETUP_H_
#define _DEVICE_SETUP_H_

class DeviceSetup {
    public:
    
    ~DeviceSetup();
    static void init_device();
    static void close_device();
    static const void *get_device_ptr();
    static const void *get_context_ptr();
    static const void *get_program_ptr();


    private:
    DeviceSetup();
    
    static DeviceSetup *m_setup;
    void *m_device_ptr;  // <- don't expose openCL to main.cpp
    void *m_context_ptr;
    void *m_program_ptr;
};

#endif  //_DEVICE_SETUP_H_

