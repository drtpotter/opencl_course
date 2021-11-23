#include <iostream>

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.hpp"
#endif

#define OCL_EXIT -20

void h_errchk(cl_int errcode, std::string message) {
    if (errcode!=CL_SUCCESS) {
        printf("Error, OpenCL call, failed at %s with error code %d \n", message.c_str(), errcode);
        exit(OCL_EXIT);
    }
};

cl_command_queue* h_create_command_queues(
        // Create a list of command queues
        // with selectable properties
        // Assumes that contexts is as long as devices
        cl_device_id *devices,
        cl_context *contexts, 
        cl_uint num_devices, 
        cl_uint num_command_queues,
        cl_bool out_of_order_enable,
        cl_bool profiling_enable) {
    
    cl_int ret_code;   

    // Manage bit fields
    cl_command_queue_properties queue_properties = 0;
    if (out_of_order_enable == CL_TRUE) {
        queue_properties = queue_properties | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;    
    }
    if (profiling_enable == CL_TRUE) {
        queue_properties = queue_properties | CL_QUEUE_PROFILING_ENABLE;    
    }

    cl_command_queue *command_queues = (cl_command_queue*)malloc(num_command_queues*sizeof(cl_command_queue));

    for (cl_uint n=0; n<num_command_queues; n++) {
        command_queues[n] = clCreateCommandQueue(
            contexts[n % num_devices],
            devices[n % num_devices],
            queue_properties,
            &ret_code    
        );
        h_errchk(ret_code, "Creating a context");        
    }
            
    return command_queues;
}

void h_release_command_queues(cl_command_queue *command_queues, cl_uint num_command_queues) {
    // Release command queues
    for (cl_uint n = 0; n<num_command_queues; n++) {
        h_errchk(clFinish(command_queues[n]), "Finishing up command queues");
        h_errchk(clReleaseCommandQueue(command_queues[n]), "Releasing command queues");
    }

    // Now destroy the command queues
    free(command_queues);
}

void h_report_on_device(cl_device_id device) {
    using namespace std;

    // Report some information on the device
    size_t nbytes_name;
    h_errchk(clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &nbytes_name),"Device name bytes");
    char* name=new char[nbytes_name];
    h_errchk(clGetDeviceInfo(device, CL_DEVICE_NAME, nbytes_name, name, NULL),"Device name");
    int textwidth=16;

    printf("\t%20s %s \n","name:", name);

    cl_ulong mem_size;
    h_errchk(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &mem_size,
    NULL),"Global mem size");

    printf("\t%20s %d MB\n","global memory size:",mem_size/(1000000));

    h_errchk(clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &mem_size,
    NULL),"Max mem alloc size");
   
    printf("\t%20s %d MB\n","max buffer size:", mem_size/(1000000));
    delete [] name;
}


void h_acquire_devices(
        // Input parameter
        cl_device_type device_type,
        // Output parameters
        cl_platform_id **platform_ids_out,
        cl_uint *num_platforms_out,
        cl_device_id **device_ids_out,
        cl_uint *num_devices_out, 
        cl_context **contexts_out) {

    // Return code for running things
    cl_int ret_code = CL_SUCCESS;
    
    // Get platforms
    cl_uint num_platforms;
    cl_platform_id *platform_ids = NULL;
    h_errchk(clGetPlatformIDs(0, NULL, &num_platforms), "Fetching number of platforms");
    platform_ids = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id));
    h_errchk(clGetPlatformIDs(num_platforms, platform_ids, NULL), "Fetching platforms");
        
    // Fetch the total number of compatible devices
    cl_uint num_devices=0;
    
    // Get the total number of devices
    for (cl_uint n=0; n < num_platforms; n++) {
        cl_uint ndevices;
        h_errchk(clGetDeviceIDs(
            platform_ids[n],
            device_type,
            0,
            NULL,
            &ndevices), "Getting number of devices");
        num_devices += ndevices;
    }
    
    // Allocate memory for device ID's and contexts
    cl_device_id *device_ids = (cl_device_id*)malloc(num_devices*sizeof(cl_device_id));
    cl_context *contexts = (cl_context*)malloc(num_devices*sizeof(cl_context));
    
    cl_device_id *device_ids_ptr = device_ids;
    cl_context *contexts_ptr = contexts;
    
    // Fill device ID's array
    for (cl_uint n=0; n < num_platforms; n++) {
        cl_uint ndevices;
        h_errchk(clGetDeviceIDs(
            platform_ids[n],
            device_type,
            0,
            NULL,
            &ndevices), "Getting number of devices for the platform");
        
        // Fill devices
        h_errchk(clGetDeviceIDs(
            platform_ids[n],
            device_type,
            ndevices,
            device_ids_ptr,
            NULL), "Filling devices");
        
        // Create a context for every device found
        for (cl_uint c=0; c<ndevices; c++ ) {
            // Context properties
            const cl_context_properties prop[] = {CL_CONTEXT_PLATFORM, 
                                                  (cl_context_properties)platform_ids[n], 
                                                  0 };
            // Create a context with 1 device in it
            const cl_device_id dev_id = *(device_ids_ptr+c);
            cl_uint ndev = 1;
            
            *contexts_ptr = clCreateContext(
                prop, 
                ndev, 
                &dev_id,
                NULL,
                NULL,
                &ret_code
            );
            h_errchk(ret_code, "Creating a context");
            contexts_ptr++;
        }
        
        // Advance device_id's pointer
        device_ids_ptr += ndevices;
    }   

    // Fill in output information here to 
    // avoid problems with understanding
    *platform_ids_out = platform_ids;
    *num_platforms_out = num_platforms;
    *device_ids_out = device_ids;
    *num_devices_out = num_devices;
    *contexts_out = contexts;
}

void h_release_devices(
        cl_device_id *devices,
        cl_uint num_devices,
        cl_context* contexts,
        cl_platform_id *platforms) {
    
    // Free contexts
    for (cl_uint n = 0; n<num_devices; n++) {
        h_errchk(clReleaseContext(contexts[n]), "Releasing contexts");
    }

    free(contexts);
    free(devices);
    free(platforms);
}
