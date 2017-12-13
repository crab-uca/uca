#ifndef _INFODEVICE_H_
#define _INFODEVICE_H_

#include <string>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

template <typename T>
void appendBitfield(T info, T value, std::string name, std::string & str)
{
	if (info & value) {
        if (str.length() > 0) {
            str.append(" | ");
        }
        str.append(name);
	}
}

template <typename T>
class InfoDevice
{
public:
    static void display(cl_device_id id, cl_device_info name, std::string str);
    static void display_arr(cl_device_id id, cl_device_info name, std::string str);
    static void display_str(cl_device_id id, cl_device_info name, std::string str);
};

template <typename T>
void InfoDevice<T>::display(cl_device_id id, cl_device_info name, std::string str)
{
    cl_int errNum;
    std::size_t paramValueSize;

    errNum = clGetDeviceInfo(id, name, 0, NULL, &paramValueSize);
    if (errNum != CL_SUCCESS) {
        std::cerr << "Failed to find OpenCL device info "
                 << str << "." << std::endl;
        return;
    }

    T *info = (T *)alloca(sizeof(T) * paramValueSize);
    errNum = clGetDeviceInfo(id, name, paramValueSize, info, NULL);
    if (errNum != CL_SUCCESS) {
        std::cerr << "Failed to find OpenCL device info "
                 << str << "." << std::endl;
        return;
    }

    switch (name) {
    case CL_DEVICE_TYPE:
    {
        std::string deviceType;

        appendBitfield<cl_device_type>(
                *(reinterpret_cast<cl_device_type *>(info)),
                CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU", deviceType);

        appendBitfield<cl_device_type>(
                *(reinterpret_cast<cl_device_type *>(info)),
                CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU", deviceType);

        appendBitfield<cl_device_type>(
                *(reinterpret_cast<cl_device_type *>(info)),
                CL_DEVICE_TYPE_ACCELERATOR, "CL_DEVICE_TYPE_ACCELERATOR", deviceType);

        appendBitfield<cl_device_type>(
                *(reinterpret_cast<cl_device_type *>(info)),
                CL_DEVICE_TYPE_DEFAULT, "CL_DEVICE_TYPE_DEFAULT", deviceType);

        std::cout << "\t\t" << str << ":\t" << deviceType << std::endl;
    }
    break;
    case CL_DEVICE_SINGLE_FP_CONFIG:
    {
    }
    break;
    case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:
    {
    }
    break;
    case CL_DEVICE_LOCAL_MEM_TYPE:
    {
    }
    break;
    case CL_DEVICE_EXECUTION_CAPABILITIES:
    {
    }
    break;
    case CL_DEVICE_QUEUE_PROPERTIES:
    {
    }
    break;
    default:
        std::cout << "\t\t" << str << ":\t" << *info << std::endl;
        break;
    }
}

template <typename T>
void InfoDevice<T>::display_arr(cl_device_id id, cl_device_info name, std::string str)
{
    cl_int errNum;
    std::size_t paramValueSize;

    errNum = clGetDeviceInfo(id, name, 0, NULL, &paramValueSize);
    if (errNum != CL_SUCCESS) {
        std::cerr << "Failed to find OpenCL device info "
                 << str << "." << std::endl;
        return;
    }

    T *info = (T *)alloca(sizeof(T) * paramValueSize);
    errNum = clGetDeviceInfo(id, name, paramValueSize, info, NULL);
    if (errNum != CL_SUCCESS) {
        std::cerr << "Failed to find OpenCL device info "
                 << str << "." << std::endl;
        return;
    }

    std::cout << "\t\t" << str << ":\t";
    for (size_t i = 0; i < paramValueSize / sizeof(T); i++)
        std::cout << info[i] << " ";
    std::cout << std::endl;
}

template <typename T>
void InfoDevice<T>::display_str(cl_device_id id, cl_device_info name, std::string str)
{
    cl_int errNum;
    std::size_t paramValueSize;

    errNum = clGetDeviceInfo(id, name, 0, NULL, &paramValueSize);
    if (errNum != CL_SUCCESS) {
        std::cerr << "Failed to find OpenCL device info "
                 << str << "." << std::endl;
        return;
    }

    T *info = (T *)alloca(sizeof(T) * paramValueSize);
    errNum = clGetDeviceInfo(id, name, paramValueSize, info, NULL);
    if (errNum != CL_SUCCESS) {
        std::cerr << "Failed to find OpenCL device info "
                 << str << "." << std::endl;
        return;
    }

    std::cout << "\t\t" << str << ":\t";
    for (size_t i = 0; i < paramValueSize / sizeof(T); i++)
        std::cout << info[i];
    std::cout << std::endl;
}
#endif
