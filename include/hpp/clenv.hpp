#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <log.h>

using namespace std;

namespace clenv {
class CLENV {

public:
    CLENV() {
        context_ = NULL;
        deviceIdsSize_ = 0;
        deviceIds_ = NULL;
        defaultdeviceId_ = NULL;
        initial_ = false;
        init();
    }
    
    ~CLENV() {
        deinit();
        context_ = NULL;
        deviceIdsSize_ = 0;
        deviceIds_ = NULL;
        defaultdeviceId_ = NULL;
    }
    
    void init() {
        if (initial_) {
            LOG_WARNING("Already init, ignored");
            return;
        }
        initial_ = InitContext();
        initial_ = InitDeviceId();
    }
    
    void deinit() {
        if (context_) {
            clReleaseContext(context_);
        }
        if (deviceIdsSize_) {
            for (int i = 0 ; i < deviceIdsSize_; i++) {
                assert(deviceIds_[i] != NULL);
                clReleaseDevice(deviceIds_[i]);
            }
            delete [] deviceIds_;
        }
    }
    
    cl_command_queue CreateCommandQueue() {
        cl_command_queue commandQueue = clCreateCommandQueue(context_, defaultdeviceId_, NULL, NULL);
        CheckErrorAndReturn(commandQueue == NULL, NULL, "clCreateCommandQueue  called fail")
        return commandQueue;
    }
    
    cl_program CreateProgram(const char *src) {
        CheckErrorAndReturn(src == NULL || strlen(src) == 0, NULL, "input src is null")

        cl_int ret;
        cl_program program;
    
        program = clCreateProgramWithSource(context_, 1, (const char**)&src,  NULL, NULL);
        CheckErrorAndReturn(program == NULL, NULL, "clCreateProgramWithSource  called fail")

        ret = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
        CheckErrorAndReturn(ret != CL_SUCCESS, NULL, "clBuildProgram  called fail")

        return program;
    }
    
    cl_program CreateProgramWithFile(const char *fileName) {
        ifstream kernelFile(fileName, ios::in);
        CheckErrorAndReturn(!kernelFile.is_open(), NULL, "Failed to open file for reading: %s", fileName)

        std::ostringstream oss;
        oss << kernelFile.rdbuf();
        std::string srcStdStr = oss.str();

        return CreateProgram(srcStdStr.c_str());
    }

private:
    bool InitContext() {
        cl_int ret;
        cl_uint numPlatform;
        cl_platform_id platformId;

        ret = clGetPlatformIDs(1, &platformId, &numPlatform);
        CheckErrorAndReturn(ret != CL_SUCCESS || numPlatform <= 0, false, "clGetPlatformIDs  called fail")

        cl_context_properties contextProperties[] = {
            CL_CONTEXT_PLATFORM, (cl_context_properties)platformId,
            0
        };

        context_ = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &ret);
        if (ret != CL_SUCCESS || context_ == NULL) {
            LOG_WARNING("clCreateContextFromType with GPU called fail, try CPU");
            context_ = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU, NULL, NULL, &ret);
            CheckErrorAndReturn(ret != CL_SUCCESS || context_ == NULL, false, "clCreateContextFromType with CPU called fail")
        }
        return true;
    }
    
    bool InitDeviceId() {
        CheckErrorAndReturn(context_ == NULL, false, "context is null")

        cl_int ret;
        size_t deviceBufferSize = 0;
        
        ret = clGetContextInfo(context_, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
        CheckErrorAndReturn(ret != CL_SUCCESS || deviceBufferSize <= 0, false, "clGetContextInfo called fail")

        deviceIds_ = new cl_device_id[deviceBufferSize/sizeof(cl_device_id)];
        ret = clGetContextInfo(context_, CL_CONTEXT_DEVICES, deviceBufferSize, deviceIds_, NULL);
        CheckErrorAndReturn(ret != CL_SUCCESS, false, "clGetContextInfo called fail")

        deviceIdsSize_ = deviceBufferSize/sizeof(cl_device_id);
        defaultdeviceId_ = deviceIds_[0];
        return true;
    }
private:
    bool initial_;

public:
    cl_context context_;
    
    size_t deviceIdsSize_;
    cl_device_id *deviceIds_;
    cl_device_id defaultdeviceId_;
};

}
