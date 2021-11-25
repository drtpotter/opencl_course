// Main

#include "cl_helper.hpp"
#include <cstdio>

#define N0 1024
#define N1 1024
#define N2 64
#define L0 1 
#define R0 1
#define L1 1
#define R1 1


int main(int argc, char** argv) {
    
    // Setting up memory for contextx and platforms
    cl_uint num_platforms, num_devices;
    
    cl_device_id *devices;
    cl_context *contexts;
    cl_platform_id *platforms;
    cl_int ret_code = CL_SUCCESS;
    
    // Could be CL_DEVICE_TYPE_GPU or CL_DEVICE_TYPE_CPU
    cl_device_type device_type = CL_DEVICE_TYPE_GPU;
    
    // Get devices and contexts
    h_acquire_devices(device_type, 
                    &platforms, &num_platforms, 
                    &devices, &num_devices,
                    &contexts);
    
    // Make a command queue and report on devices
    for (cl_uint n=0; n<num_devices; n++) {
        h_report_on_device(devices[n]);
    }
   
    // Create command queues 
    cl_uint num_command_queues = num_devices;
    cl_command_queue* command_queues = h_create_command_queues(
            devices,
            contexts,
            num_devices,
            num_command_queues,
            CL_FALSE,
            CL_FALSE);

    // Create memory for images in and images out
    float* images_out = (float*)calloc(N0*N1*N2, sizeof(float));
    
    // Create buffers
    
    // Read 32-bit kernel into file
    size_t nelements_image_kernel = (L0+R0+1)*(L1+R1+1);
    float* image_kernel = (float*)calloc(nelements_image_kernel, sizeof(float));
    FILE* fp = fopen("image_kernel.dat", "r+b");
    fread(image_kernel, sizeof(float), nelements_image_kernel, fp);
    fclose(fp);

    // Read 32-bit data into file
    float* images_in = (float*)calloc(N0*N1*N2, sizeof(float));
    fp = fopen("images_in.dat", "r+b");
    fread(images_in, sizeof(float), N0*N1*N2, fp);
    fclose(fp);

    // Create output data

    // Loop over buffers
    // For each loop
    // read into buffer
    // run kernel
    // read out into results

    // Write output data to output file
    fp = fopen("images_out.dat", "w+b");
    fwrite(images_out, sizeof(float), N0*N1*N2, fp);
    fclose(fp);

    // Free memory
    free(image_kernel);
    free(images_in);
    free(images_out);

    // Release command queues
    h_release_command_queues(command_queues, num_command_queues);

    // Release devices and contexts
    h_release_devices(devices, num_devices, contexts, platforms);
}
