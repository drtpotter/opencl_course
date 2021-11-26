// Main

#include <assert.h>
#include "cl_helper.hpp"
#include <cstdio>
#include <omp.h>

#define N0 1024
#define N1 1024
#define NIMAGES 64
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

    // Create memory for images out
    float* images_out = (float*)calloc(NIMAGES*N0*N1, sizeof(float));
    
    // Image_in will have dimensions (NIMAGES, N0, N1) and will have row-major ordering

    // Read in images
    float* images_in = (float*)h_read_file("images_in.dat", "rb", &nbytes);
    assert(nbytes == NIMAGES*N0*N1*sizeof(float));

    // Read in image Kernel
    size_t nelements_image_kernel = (L0+R0+1)*(L1+R1+1);
    float* image_kernel = (float*)h_read_file("image_kernel.dat", "rb", &nbytes);
    assert(nbytes == nelements_image_kernel*sizeof(float));

    // Read kernel sources 
    size_t nbytes;
    const char* filename = "kernels.cl";
    char* source = (char*)h_read_file(filename, "r", &nbytes);

    // Create Programs and kernels using this source
    cl_program *programs = (cl_program*)calloc(num_devices, sizeof(cl_program));
    cl_kernel *kernels = (cl_kernel*)calloc(num_devices, sizeof(cl_kernel));
    
    for (cl_uint n=0; n<num_devices; n++) {
        // Make the program from source
        programs[n] = h_build_program(source, contexts[n], devices[n]);
        // And make the kernel
        kernels[n] = clCreateKernel(programs[n], "xcorr", &ret_code);
        h_errchk(ret_code, "Making a kernel");
    }

    // Create memory for images in and images out for each device
    cl_mem *buffer_srces = (cl_mem*)calloc(num_devices, sizeof(cl_mem));
    cl_mem *buffer_dests = (cl_mem*)calloc(num_devices, sizeof(cl_mem));
    cl_mem *buffer_kerns = (cl_mem*)calloc(num_devices, sizeof(cl_mem));
   
    // Create buffers
    for (cl_uint n=0; n<num_devices; n++) {
        // Create buffers for sources
        buffer_srces[n] = clCreateBuffer(
                contexts[n],
                CL_MEM_READ_WRITE,
                N0*N1*sizeof(float),
                NULL,
                &ret_code);
        h_errck(ret_code, "Creating buffers for sources");

        // Create buffers for destination
        buffer_dests[n] = clCreateBuffer(
                contexts[n],
                CL_MEM_READ_WRITE,
                N0*N1*sizeof(float),
                NULL,
                &ret_code);
        h_errck(ret_code, "Creating buffers for destinations");

        // Copy host memory for the image kernel
        buffer_kerns[n] = clCreateBuffer(
                contexts[n],
                CL_MEM_COPY_HOST_PTR,
                nelements_image_kernel*sizeof(float),
                (void*)image_kernel,
                &ret_code);
        h_errck(ret_code, "Creating buffers for image kernel");
    }

    // Use OpenMP to dynamically distribute threads across the available workflow of images
    //omp_set_dynamic(0);
    //omp_set_num_threads(num_devices);

    #pragma omp parallel for default(none) schedule(dynamic, 1) num_threads(num_devices) 
    for (cl_uint n=0; n<NIMAGES; n++) {
        // Get the thread_id
        int tid = omp_get_thread_num();

        
    
    }

    // Loop over buffers using openmp
    // For each loop
    // read into buffer
    // run kernel
    // read out into results

    // Write output data to output file
    FILE* fp = fopen("images_out.dat", "w+b");
    fwrite(images_out, sizeof(float), NIMAGES*N0*N1, fp);
    fclose(fp);

    // Free memory
    free(source);
    free(image_kernel);
    free(images_in);
    free(images_out);

    // Release command queues
    h_release_command_queues(command_queues, num_command_queues);

    // Release programs and kernels
    for (cl_uint n=0; n<num_devices; n++) {
        h_errchk(clReleaseKernel(kernels[n]), "Releasing kernel");
        h_errchk(clReleaseProgram(programs[n]), "Releasing program");
        h_errchk(clReleaseMemObject(buffer_srces[n]),"Releasing sources buffer");
        h_errchk(clReleaseMemObject(buffer_dests[n]),"Releasing dests buffer");
        h_errchk(clReleaseMemObject(buffer_kerns[n]),"Releasing image kernels buffer");
    }

    // Free memory
    free(buffer_srces);
    free(buffer_dests);
    free(buffer_kerns);
    free(programs);
    free(kernels);

    // Release devices and contexts
    h_release_devices(devices, num_devices, contexts, platforms);
}
