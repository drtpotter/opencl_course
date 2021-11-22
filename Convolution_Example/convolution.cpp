// Main

#include <CL/cl.hpp>
#include "helper_functions.hpp"

int main(int argc, char** argv) {
    
    // Setting up memory for contextx and platforms
    cl_uint num_platforms, num_devices, num_contexts;
    
    cl_device_id *devices;
    cl_context *contexts;
    cl_platform_id *platforms;
    
    // Could be CL_DEVICE_TYPE_GPU or CL_DEVICE_TYPE_CPU
    cl_device_type device_type = CL_DEVICE_TYPE_ALL;
    
    // Get devices and contexts
    acquire_devices(device_type, 
                    &platforms, &num_platforms, 
                    &devices, &num_devices,
                    &platforms, &num_platforms);
    
    // Release devices and contexts
    release_devices(command_queues, num_command_queues
                   contexts, num_contexts,
                   devices, platforms);
    
    
}
