#ifndef _DEVICE_SETUP_H_
#define _DEVICE_SETUP_H_

class DeviceSetup {
    public:
    
    ~DeviceSetup();
    static void init_device();

    unsigned int getDevice();

    private:
    DeviceSetup();
    
    static DeviceSetup *m_setup;
    void *m_device_ptr;  // <- don't expose openCL to main.cpp
};

#endif  //_DEVICE_SETUP_H_

