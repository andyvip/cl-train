#include <log.h>
#include <clenv.hpp>

using namespace std;
using namespace clenv;

#define ARRAY_SIZE 1000

int main(int argc, char **argv)
{
    const char *kernelSrc =
    "__kernel void hello_kernel(__global const float *a, __global const float *b, __global float *result) \n"
    "{ \n"
    "    int gid = get_global_id(0); \n"
    "    result[gid] = a[gid] + b[gid]; \n"
    "}; \n";

    CLENV clenv;
    cl_int ret = -1;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_mem memObjects[3] = { 0, 0, 0};
    cl_command_queue commandQueue = NULL;
    
    auto cleanFunction = [&] {
        for (int i = 0; i < 3; i++) {
            if (memObjects[i] != 0) {
                clReleaseMemObject(memObjects[i]);
            }
        }
        
        if (commandQueue != 0) {
            clReleaseCommandQueue(commandQueue);
        }
        
        if (kernel != 0) {
            clReleaseKernel(kernel);
        }
        
        if (program != 0) {
            clReleaseProgram(program);
        }
        
        clenv.deinit();
        exit(1);
    };

    program = clenv.CreateProgram(kernelSrc);
    if (program == NULL) {
        return 1;
    }
    
    commandQueue = clenv.CreateCommandQueue();
    if (commandQueue == NULL) {
        return 1;
    }
    
    kernel = clCreateKernel(program, "hello_kernel", NULL);
    if (kernel == NULL) {
        LOG_ERROR("clCreateKernel called fail");
        return 1;
    }
    
    float a[ARRAY_SIZE] = {0};
    float b[ARRAY_SIZE] = {0};
    float result[ARRAY_SIZE] = {0};
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        a[i] = (float)i;
        b[i] = (float)(i * 2);
    }
    
    memObjects[0] = clCreateBuffer(clenv.context_, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * ARRAY_SIZE, a, &ret);
    memObjects[1] = clCreateBuffer(clenv.context_, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * ARRAY_SIZE, b, &ret);
    memObjects[2] = clCreateBuffer(clenv.context_, CL_MEM_READ_WRITE, sizeof(float) * ARRAY_SIZE, NULL, &ret);
    CheckErrorAndHandle(memObjects[0] == NULL || memObjects[1] == NULL || memObjects[2] == NULL, cleanFunction, "clEnqueueNDRangeKernel called fail");

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects[0]);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[1]);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[2]);
    CheckErrorAndHandle(ret != CL_SUCCESS, cleanFunction, "clSetKernelArg called fail");
    
    size_t globalWorkSize[1] = { ARRAY_SIZE };
    size_t localWorkSize[1] = { 1 };
    ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    CheckErrorAndHandle(ret != CL_SUCCESS, cleanFunction, "clEnqueueNDRangeKernel called fail");

    ret = clEnqueueReadBuffer(commandQueue, memObjects[2], CL_TRUE, 0, ARRAY_SIZE * sizeof(float), result, 0, NULL, NULL);
    CheckErrorAndHandle(ret != CL_SUCCESS, cleanFunction, "clEnqueueReadBuffer called fail");
    
    LOG_NOTE("result:");
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        cout << result[i] << " ";
    }
    cout << std::endl;
    cleanFunction();
}

