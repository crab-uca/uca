#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Force to use OpenCL 1.2 APIs
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

const int ARRAY_SIZE = 16;
const int NUM_MEM_OBJ = 2;

inline int checkErr(cl_int err, const char *msg)
{
	if (err != CL_SUCCESS) {
		std::cout << "Error(" << err << "): " << msg << std::endl;
		return err;
	}
	return CL_SUCCESS;
}

void Cleanup(cl_context context,  cl_command_queue commandQueue,
		cl_program program, cl_kernel kernel,
		cl_mem memObjects[NUM_MEM_OBJ])
{
	if (kernel != NULL)
		clReleaseKernel(kernel);

	if (program != NULL)
		clReleaseProgram(program);

	for (int i = 0; i < NUM_MEM_OBJ; i++) {
		if (memObjects[i] != NULL)
			clReleaseMemObject(memObjects[i]);
	}

	if (commandQueue != NULL)
		clReleaseCommandQueue(commandQueue);

	if (context != NULL)
		clReleaseContext(context);
}

int main(int argc, char *argv[])
{
	cl_int err;
	cl_uint plat_num;
	cl_platform_id *plat_ids; // alloca()

	cl_uint dev_num;
	cl_device_id *dev_ids; // alloca()

	cl_context ctx = NULL;
	cl_program prog = NULL;
	cl_kernel kernel = NULL;

	cl_mem mem_obj[NUM_MEM_OBJ] = {0, 0};
	cl_command_queue cmdq = NULL;

	err = clGetPlatformIDs(0, NULL, &plat_num);
	err = checkErr((err != CL_SUCCESS) ? err : (plat_num < 1 ? -1 : CL_SUCCESS),
			"failed to get valid platform number!");
	if (err != CL_SUCCESS)
		return err;

	plat_ids = (cl_platform_id *)alloca(sizeof(cl_platform_id) * plat_num);
	err = clGetPlatformIDs(plat_num, plat_ids, NULL);
	err = checkErr(err, "failed to get platform IDs!");
	if (err != CL_SUCCESS)
		return err;

	std::cout << "Platform Number: " << plat_num << std::endl;

	/* Use the first available platform */
	err = clGetDeviceIDs(plat_ids[0], CL_DEVICE_TYPE_ALL, 0, NULL, &dev_num);
	err = checkErr((err != CL_SUCCESS) ? err : (dev_num < 1 ? -1 : CL_SUCCESS),
			"failed to get valid ALL device number!");
	if (err != CL_SUCCESS)
		return err;
	std::cout << "Device Number: " << dev_num << std::endl;

	err = clGetDeviceIDs(plat_ids[0], CL_DEVICE_TYPE_GPU, 0, NULL, &dev_num);
	err = checkErr((err != CL_SUCCESS) ? err : (dev_num < 1 ? -1 : CL_SUCCESS),
			"failed to get valid GPU device number!");
	if (err != CL_SUCCESS)
		return err;
	std::cout << "Device(GPU) Number: " << dev_num << std::endl;

	/* Get the first GPU device */
	dev_ids = (cl_device_id *)alloca(sizeof(cl_device_id) * dev_num);
	err = clGetDeviceIDs(plat_ids[0], CL_DEVICE_TYPE_GPU, 1, dev_ids, NULL);
	if (checkErr(err, "failed to get device IDs!") != CL_SUCCESS)
		return err;

	cl_context_properties ctx_pro[] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)plat_ids[0],
		0
	};
	ctx = clCreateContext(ctx_pro, dev_num, dev_ids, NULL, NULL, &err);
	if (checkErr(err, "failed to create context!") != CL_SUCCESS)
		return err;

	/* Deprecated in OCL 2.0 */
	cmdq = clCreateCommandQueue(ctx, dev_ids[0], 0, &err);
	if (checkErr(err, "failed to create command queue!") != CL_SUCCESS) {
		Cleanup(ctx, cmdq, prog, kernel, mem_obj);
		return err;
	}

	int input_1[ARRAY_SIZE] =
	{
		0, 1, 2,   3,  4,  5,  6,  7,
		8, 9, 10, 11, 12, 13, 14, 15
	};
	int ptr_rd[4] = {-1, -1, -1, -1};

	int input_2[4][4] =
	{
		{0, 1, 2, 3},
		{4, 5, 6, 7},
		{8, 9, 10, 11},
		{12, 13, 14, 15}
	};
	int ptr_wr[2][2] = { {-1, -1}, {-1, -1}};

	mem_obj[0] = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(int) * ARRAY_SIZE, input_1, &err);
	if (checkErr(err, "failed to create buffer 0!") != CL_SUCCESS) {
		Cleanup(ctx, cmdq, prog, kernel, mem_obj);
		std::cout << "error value: " << err << std::endl;
		return err;
	}
	mem_obj[1] = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			sizeof(int) * ARRAY_SIZE, input_2, &err);
	if (checkErr(err, "failed to create buffer 1!") != CL_SUCCESS) {
		Cleanup(ctx, cmdq, prog, kernel, mem_obj);
		return err;
	}

	const size_t buf_origin[3] = {1 * sizeof(int), 1, 0 };
	const size_t host_origin[3] = {0, 0, 0};
	const size_t region[3] = { 2 * sizeof(int), 2, 1};

	err = clEnqueueReadBufferRect(cmdq, mem_obj[0], CL_TRUE, buf_origin,
			host_origin, region, 4 * sizeof(int), 0, 2 * sizeof(int), 0,
			ptr_rd, 0, NULL, NULL);
	if (checkErr(err, "failed to read buffer rectangle!") != CL_SUCCESS) {
		Cleanup(ctx, cmdq, prog, kernel, mem_obj);
		return err;
	}

	err = clEnqueueWriteBufferRect(cmdq, mem_obj[1], CL_TRUE, buf_origin,
			host_origin, region, 4 * sizeof(int), 0, 2 * sizeof(int), 0,
			ptr_wr, 0, NULL, NULL);
	if (checkErr(err, "failed to read buffer rectangle!") != CL_SUCCESS) {
		Cleanup(ctx, cmdq, prog, kernel, mem_obj);
		return err;
	}
	int *mapPtr = (int*)clEnqueueMapBuffer(cmdq, mem_obj[1], CL_TRUE, CL_MAP_READ,
				0, sizeof(int) * ARRAY_SIZE, 0, NULL, NULL, &err);

	std::cout << "input_1:" << std::endl;
	for (int i = 0; i < ARRAY_SIZE; i++) {
		std::cout << input_1[i] << " ";
		if (!((i + 1) % 4))
			std::cout<<std::endl;
	}
	std::cout << std::endl;

	std::cout << "ptr_rd:" << std::endl;
	std::cout << ptr_rd[0] << " ";
	std::cout << ptr_rd[1] << " " << std::endl;
	std::cout << ptr_rd[2] << " ";
	std::cout << ptr_rd[3] << " " << std::endl;

	std::cout << "input_2:" << std::endl;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			std::cout << input_2[i][j] << " ";
		std::cout<<std::endl;
	}
	std::cout << std::endl;
	std::cout << "write buffer:" << std::endl;
	for (int i = 0; i < ARRAY_SIZE; i++) {
		std::cout << mapPtr[i] << " ";
		if (!((i + 1) % 4))
			std::cout<<std::endl;
	}
	std::cout << "Execution is sucessful." << std::endl;

	clEnqueueUnmapMemObject(cmdq, mem_obj[1], mapPtr, 0, NULL, NULL);
	Cleanup(ctx, cmdq, prog, kernel, mem_obj);
	return 0;
}
