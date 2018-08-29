//
//  main.cpp
//  opencl-train
//
//  Created by andy on 2018/6/6.
//  Copyright © 2018 andy. All rights reserved.
//

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>

int main(int argc, const char * argv[])
{
    cl_int errNum;
    cl_uint numPlatforms;
    cl_platform_id *platformIds;
    
    errNum = clGetPlatformIDs(0, NULL, &numPlatforms);
    if(errNum != CL_SUCCESS || numPlatforms <= 0)
    {
        std::cerr << "Failed to find any OpenCL platform." << std::endl;
        return 0;
    }

    platformIds = (cl_platform_id *)alloca(sizeof(cl_platform_id) * numPlatforms);
    errNum = clGetPlatformIDs(numPlatforms, platformIds, NULL);
    if (errNum != CL_SUCCESS)
    {
        std::cerr << "Failed to find any OpenCL platforms." << std::endl;
        return 0;
    }
    std::cout << "Number of platforms: \t" << numPlatforms << std::endl;

    auto displayPlatformInfo = [&errNum](cl_platform_id platformId, cl_platform_info paramName, std::string paramNameStr) -> cl_int {
        size_t paramValueSize;
        errNum = clGetPlatformInfo(platformId, paramName, 0, NULL, &paramValueSize);
        if(errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to find OpenCL platform " << paramNameStr << "." << std::endl;
            return 0;
        }
        
        char *info = (char *)alloca(sizeof(char) * paramValueSize);
        errNum = clGetPlatformInfo(platformId, paramName, paramValueSize, info, NULL);
        if(errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to find OpenCL platform " << paramNameStr << "." << std::endl;
            return 0;
        }
        std::cout << "\t" << paramNameStr << ": " << info << std::endl;
        return 0;
    };
    
    auto displayPlatformDeviceInfo = [&errNum](cl_device_id deviceId, cl_device_info paramName, std::string paramNameStr, int infotype) -> cl_int {
        size_t paramValueSize;
        errNum = clGetDeviceInfo(deviceId, paramName, 0, NULL, &paramValueSize);
        if (errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to find OpenCL device info " << paramNameStr << "." << std::endl;
            return 0;
        }

        char *info = (char *)alloca(sizeof(char) * paramValueSize);
        errNum = clGetDeviceInfo(deviceId, paramName, paramValueSize, info, NULL);

        if (errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to find OpenCL device info " << paramNameStr << "." << std::endl;
            return 0;
        }
        
        if (infotype == 0) {
            std::cout << "\t" << paramNameStr << ": " << info << std::endl;
        } else if (infotype == 1) {
            std::cout << "\t" << paramNameStr << ": " << *((cl_device_type *)info) << std::endl;
        } else if (infotype == 2) {
            std::cout << "\t" << paramNameStr << ": " << *((cl_uint *)info) << std::endl;
        } else if (infotype == 3) {
            std::cout << "\t" << paramNameStr << ": " << "[";
            size_t *temp = (size_t *)info;
            for (int i = 0 ; i < paramValueSize/sizeof(size_t); i++) {
                std::cout << temp[i]  << ", ";
            }
            std::cout << "]" << std::endl;
        } else if (infotype == 4) {
            std::cout << "\t" << paramNameStr << ": " << *((size_t *)info) << std::endl;
        } else if (infotype == 5) {
            std::cout << "\t" << paramNameStr << ": " << *((cl_bool *)info) << std::endl;
        }
        return 0;
    };

    
    for(cl_uint i = 0; i < numPlatforms; i++)
    {
        std::cout << "Platform " << i << " : " << std::endl;

        std::cout << "Platform Info: " << std::endl;
        displayPlatformInfo(platformIds[i], CL_PLATFORM_PROFILE, "CL_PLATFORM_PROFILE");
        displayPlatformInfo(platformIds[i], CL_PLATFORM_NAME, "CL_PLATFORM_NAME");
        displayPlatformInfo(platformIds[i], CL_PLATFORM_VENDOR, "CL_PLATFORM_VENDOR");
        displayPlatformInfo(platformIds[i], CL_PLATFORM_VERSION, "CL_PLATFORM_VERSION");
        displayPlatformInfo(platformIds[i], CL_PLATFORM_EXTENSIONS, "CL_PLATFORM_EXTENSIONS");

        std::cout << std::endl;
        std::cout << "Platform Device: " << std::endl;

        cl_uint numDevices;
        errNum = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
        if (errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to find OpenCL devices." << std::endl;
            return 0;
        }
        cl_device_id * devices = (cl_device_id *)alloca(sizeof(cl_device_id) * numDevices);
        errNum = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
        if (errNum != CL_SUCCESS)
        {
            std::cerr << "Failed to find OpenCL devices." << std::endl;
            return 0;
        }
        std::cout << "\tNumber of devices: \t" << numDevices << std::endl;
        for (cl_uint j = 0; j < numDevices; j++) {
            std::cout << "Device " << j << " : " << std::endl;
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_TYPE, "CL_DEVICE_TYPE", 1);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_AVAILABLE, "CL_DEVICE_AVAILABLE", 5);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, "CL_DEVICE_MAX_COMPUTE_UNITS", 2);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS", 2);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, "CL_DEVICE_MAX_WORK_ITEM_SIZES", 3);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, "CL_DEVICE_MAX_WORK_GROUP_SIZE", 4);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_NAME, "CL_DEVICE_NAME", 0);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_VENDOR, "CL_DEVICE_VENDOR", 0);
            displayPlatformDeviceInfo(devices[j], CL_DRIVER_VERSION, "CL_DRIVER_VERSION", 0);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_PROFILE, "CL_DEVICE_PROFILE", 0);
            displayPlatformDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, "CL_DEVICE_EXTENSIONS", 0);
        }
    }
    return 0;
}


