#include <stdio.h>
#include <assert.h>

#define OCL_EXIT -20
#define MAXCHAR 100
#define NQUEUES_PER_DEVICE 2

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.hpp"
#endif

void errchk(cl_int errcode, std::string message) {
    if (errcode!=CL_SUCCESS) {
        printf("Error, OpenCL call, failed at %s with error code %d \n", message.c_str(), errcode);
        exit(OCL_EXIT);
    }
};

int main(int argc, char**argv) {
    // Useful for checking OpenCL errors
    cl_int errcode;

    // Number to hold the number of platforms
    cl_uint num_platforms;
    
    // First call this to get the number of compute platforms
    errchk(clGetPlatformIDs(0, NULL, &num_platforms),"Get number of platforms");
    
    // Allocate memory for the found number of platforms
    cl_platform_id* platformIDs_1d=(cl_platform_id*)calloc(num_platforms, sizeof(cl_platform_id));
    
    // Now fill the platform information in
    errchk(clGetPlatformIDs(num_platforms, platformIDs_1d, &num_platforms),"Filling platform ID's\n");  

    for (int i=0; i<num_platforms; i++) {
        char version[MAXCHAR];
        char vendor[MAXCHAR];
        char name[MAXCHAR];
        
        // Fill the string with platform information
        errchk(clGetPlatformInfo(   platformIDs_1d[i], \
                                    CL_PLATFORM_NAME, 
                                    MAXCHAR, 
                                    name, 
                                    NULL),"Getting name\n");
        // Getting vendor         
        errchk(clGetPlatformInfo(platformIDs_1d[i], 
                                    CL_PLATFORM_VENDOR, 
                                    MAXCHAR, 
                                    vendor, 
                                    NULL),"Getting vendor\n");
        
        // Getting version
        errchk(clGetPlatformInfo(   platformIDs_1d[i], 
                                    CL_PLATFORM_VERSION,
                                    MAXCHAR,
                                    version,
                                    NULL),"Getting version\n");
                                    
        printf("Platform %d: %s, vendor: %s, version %s\n", i, name, vendor, version);
    }

    // Now get the number of valid devices for each platform 
    // And the total number of valid devices
    cl_uint* ndevices_1d=(cl_uint*)calloc(num_platforms, sizeof(cl_uint));
    int ndevices=0;
    for (int i=0; i<num_platforms; i++) {
        cl_uint ndevice=0;
        errchk(clGetDeviceIDs(      platformIDs_1d[i],
                                    CL_DEVICE_TYPE_ALL,
                                    0,
                                    NULL,
                                    &ndevice),"Getting number of devices");
        
        if (ndevice>0) {
            ndevices_1d[i]=ndevice;
            ndevices+=ndevice;
        } 

        printf("Platform %d has %d devices\n", i,  ndevices_1d[i]);
    }
    
    // Make sure we have at least one valid platform
    assert(ndevices>=1);

    // Create a 2D array for devices and fill it with device information
    cl_device_id** devices_2d=(cl_device_id**)calloc(num_platforms, sizeof(cl_device_id*));
    for (int i=0; i<num_platforms; i++) {
        // Create a 1D array for devices in a platform
        devices_2d[i]=(cl_device_id*)calloc(ndevices_1d[i], sizeof(cl_device_id));
        
        // Fill the devices array
        errchk(clGetDeviceIDs(      platformIDs_1d[i],
                                    CL_DEVICE_TYPE_ALL,
                                    ndevices_1d[i],
                                    devices_2d[i],
                                    NULL),"Filling device arrays");
    }

    // Now make a context for each platform
    // allocate memory first
    cl_context* contexts_1d=(cl_context*)calloc(num_platforms, sizeof(cl_context));
    for (int i=0; i<num_platforms; i++) {
        if (ndevices_1d[i]>0) {
            // We have a valid platform, create a context.
            // Handling the context properties is tricky, here is how to do it
            const cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformIDs_1d[i], 0 };
            // Now set the context using the context properties
            contexts_1d[i]=clCreateContext(prop, ndevices_1d[i], devices_2d[i], NULL, NULL, &errcode);
            errchk(errcode, "Creating contexts");
        }
    }

    // Now create command queues for each valid device;
    int num_command_queues=ndevices*NQUEUES_PER_DEVICE;
    cl_command_queue* command_queues_1d=(cl_command_queue*)calloc(num_command_queues, sizeof(cl_command_queue));
    int counter=0;
    for (int i=0; i<num_platforms; i++) {
        for (int j=0; j<ndevices_1d[i]; j++) {
            for (int k=0; k<NQUEUES_PER_DEVICE; k++) {
                command_queues_1d[counter]=clCreateCommandQueue(contexts_1d[i], devices_2d[i][j], 0, &errcode);
                counter++;
            }
        }
    }

    // Allocate buffers for the memory transfer
    // Make a kernel
    // Build the kernel
    // Run the kernel


    // Wait for all command queues to finish
    // Release the command queues
    for (int i=0; i<num_command_queues; i++) {
        errchk(clFinish(command_queues_1d[i]),"Finishing up command queues");
        errchk(clReleaseCommandQueue(command_queues_1d[i]), "Releasing command queues");
    }

    // Release the contexts and free the devices associated with each context
    for (int i=0; i<num_platforms; i++) {
        if (ndevices_1d[i]>0) {
            errchk(clReleaseContext(contexts_1d[i]),"Releasing the context");
            free(devices_2d[i]);
        }
    }

    // Clean up memory    
    free(command_queues_1d);
    free(ndevices_1d);
    free(platformIDs_1d);
    free(contexts_1d);
    free(devices_2d);

}

