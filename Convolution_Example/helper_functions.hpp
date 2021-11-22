#include <iostream>

void errchk(cl_int errcode, std::string message) {
    if (errcode!=CL_SUCCESS) {
        printf("Error, OpenCL call, failed at %s with error code %d \n", message.c_str(), errcode);
        exit(OCL_EXIT);
    }
};

void report_on_device(cl_device_id device) {
    using namespace std;

    // Report some information on the device
    size_t nbytes_name;
    errchk(clGetDeviceInfo(device, CL_DEVICE_NAME, NULL, NULL, &nbytes_name),"Device name bytes");
    char* name=new char[nbytes_name];
    errchk(clGetDeviceInfo(device, CL_DEVICE_NAME, nbytes_name, name, NULL),"Device name");
    int textwidth=16;

    printf("\t%20s %s \n","name:", name);


    cl_ulong mem_size;
    errchk(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &mem_size,
    NULL),"Global mem size");

    printf("\t%20s %d MB\n","global memory size:",mem_size/(1000000));

    errchk(clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &mem_size,
    NULL),"Max mem alloc size");
   
    printf("\t%20s %d MB\n","max buffer size:", mem_size/(1000000));
    delete [] name;
}


void acquire_devices(
        // Input parameter
        cl_device_type device_type,
        // Output parameters
        cl_platform_id **platform_ids_out,
        cl_uint *num_platforms_out,
        cl_device_id **device_ids_out,
        cl_uint *num_devices_out, 
        cl_context **contexts_out,
        cl_uint *num_contexts_out) {

    // Return code for running things
    cl_int ret_code = CL_SUCCESS;
    
    // Get platforms
    cl_uint num_platforms;
    cl_platform_id *platform_ids = NULL;
    errchk(clGetPlatformIDs(0, NULL, &num_platforms), "Fetching number of platforms");
    platform_ids = malloc(num_platforms * sizeof(cl_platform_id));
    errchk(clGetPlatformIDs(num_platforms, platform_ids, NULL), "Fetching platforms");
        
    // Fetch the total number of compatible devices
    cl_uint ndevices=0;
    
    // Get the total number of devices
    for (cl_uint n=0; n < num_platforms; n++) {
        cl_uint ndevices;
        errchk(clGetDeviceIDs(
            platform_ids[n],
            device_type,
            0,
            NULL,
            &ndevices), "Getting number of devices");
        num_devices += ndevices;
    }
    
    // Allocate memory for device ID's and contexts
    cl_device_id *device_ids = malloc(num_devices*sizeof(cl_device_id));
    cl_context *contexts = malloc(num_devices*sizeof(cl_context));
    
    cl_device_id *device_ids_ptr = device_ids;
    cl_context *contexts_ptr = contexts;
    
    // Fill device ID's array
    for (cl_uint n=0; n < num_platforms; n++) {
        cl_uint ndevices;
        errchk(clGetDeviceIDs(
            platform_ids[n],
            device_type,
            0,
            NULL,
            &ndevices), "Getting number of devices for the platform");
        
        // Fill devices
        errchk(clGetDeviceIDs(
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
            errchk(ret_code, "Creating a context");
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
    *num_contexts_out = num_devices;
}

void release_devices(
        cl_command_queue *command_queues,
        cl_uint num_command_queues,
        cl_context* contexts,
        cl_uint num_contexts,
        cl_platform_id *platforms,
        cl_device_id *devices) {
    
    for (cl_uint n = 0; n<num_command_queues; n++) {
        errchk(clFinish(command_queues[n]), "Finishing up command queues");
        errchk(clReleaseCommandQueue(command_queues[n]), "Releasing command queues");
    }

    for (cl_uint n = 0; n<num_contexts; n++) {
        errchk(clReleaseContext(contexts[n]), "Releasing contexts");
    }

    free(command_queues);
    free(contexts);
    free(devices);
    free(platforms);
}
