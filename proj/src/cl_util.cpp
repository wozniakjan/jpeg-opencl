#include "cl_util.h"

#define DEBUG


using namespace std;


// variables, settings & other stuff
cl_platform_id platform;
int dev_num = 1;
cl_device_id devices[1];
cl_uint dev_count;
cl_uint dev_limit = 1;
cl_context context;
cl_command_queue queue;
cl_program program;
cl_kernel kernel;


cl_int getPlatformID()
{
    cl_uint num_platforms;
    cl_platform_id *platform_ids;
    
    // Get OpenCL platform count
    cl_int error = clGetPlatformIDs (0, NULL, &num_platforms);
    checkClError(error, "platformID count");
    if(num_platforms == 0) {
        cout << "ERROR: 0 platforms found\n";
        exit(1);
    }
    // if there's a platform or more, make space for ID's
    if((platform_ids = (cl_platform_id*)malloc(num_platforms*sizeof(cl_platform_id))) == NULL) {
        cout << "Failed to allocate memory for cl_platform ID's!\n";
        exit(1);
    }

    // get platform info for each platform and trap the NVIDIA platform if found
    error = clGetPlatformIDs (num_platforms, platform_ids, NULL);
    checkClError(error, "platformIDs");
    
    //assign platform id to global variable
    platform = platform_ids[0];
    
    //clean resources
    free(platform_ids);

    return CL_SUCCESS;
}



int initOpenCL(){
    cl_int error = getPlatformID();
    checkClError(error, "getPlatformID");
    
    // Device
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, dev_limit, devices, &dev_count);
    checkClError(error, "clGetDeviceIDs");
    
    // Context
    cl_context_properties contextProperties[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0
    };
    context = clCreateContext(contextProperties, dev_num, devices, 
                                         &contextCallback, NULL, &error);
    checkClError(error, "clCreateContext");
    
    // Command-queue
    queue = clCreateCommandQueue(context, devices[0], 0, &error);
    checkClError(error, "clCreateCommandQueue");

    // Load kernel
    error = loadKernelFromFile("../src/jpeg.cl");
    checkClError(error, "loadKernelFromFile");
}


cl_int loadKernelFromFile(const char* fileName){
    cl_int error;
    ifstream src_file(fileName);
    if(!src_file.is_open()) return EXIT_FAILURE;

    string src_prog(istreambuf_iterator<char>(src_file), (istreambuf_iterator<char>()));
    const char *src = src_prog.c_str();
    size_t length = src_prog.length();

    program = clCreateProgramWithSource(context, 1, &src, &length, &error);
    checkClError(error, "clCreatePogramWithSource");

    error = clBuildProgram(program, dev_count, devices, NULL, NULL, NULL);
    checkClError(error, "clBuildProgrm");

    kernel = clCreateKernel(program, "jpeg_block", &error);
    checkClError(error, "clCreateKernel");

    return CL_SUCCESS;
}





/**
 * Vrati retezec pro opencl error kod
 */
const char *CLErrorString(cl_int _err) {
    switch (_err) {
    case CL_SUCCESS:
        return "Success!";
    case CL_DEVICE_NOT_FOUND:
        return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE:
        return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE:
        return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES:
        return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY:
        return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE:
        return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP:
        return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH:
        return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE:
        return "Program build failure";
    case CL_MAP_FAILURE:
        return "Map failure";
    case CL_INVALID_VALUE:
        return "Invalid value";
    case CL_INVALID_DEVICE_TYPE:
        return "Invalid device type";
    case CL_INVALID_PLATFORM:
        return "Invalid platform";
    case CL_INVALID_DEVICE:
        return "Invalid device";
    case CL_INVALID_CONTEXT:
        return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES:
        return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE:
        return "Invalid command queue";
    case CL_INVALID_HOST_PTR:
        return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT:
        return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE:
        return "Invalid image size";
    case CL_INVALID_SAMPLER:
        return "Invalid sampler";
    case CL_INVALID_BINARY:
        return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS:
        return "Invalid build options";
    case CL_INVALID_PROGRAM:
        return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE:
        return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME:
        return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION:
        return "Invalid kernel definition";
    case CL_INVALID_KERNEL:
        return "Invalid kernel";
    case CL_INVALID_ARG_INDEX:
        return "Invalid argument index";
    case CL_INVALID_ARG_VALUE:
        return "Invalid argument value";
    case CL_INVALID_ARG_SIZE:
        return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS:
        return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION:
        return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE:
        return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE:
        return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET:
        return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST:
        return "Invalid event wait list";
    case CL_INVALID_EVENT:
        return "Invalid event";
    case CL_INVALID_OPERATION:
        return "Invalid operation";
    case CL_INVALID_GL_OBJECT:
        return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE:
        return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL:
        return "Invalid mip-map level";
    default:
        return "Unknown";
    }
}

inline void checkClError(cl_int err, char* debug){
    if(err != CL_SUCCESS){
        cout << "ERROR: " << CLErrorString(err) << endl;
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    else {
        cout << "OK - " << debug << endl;
    }
#endif
}

void CL_CALLBACK contextCallback(const char *err_info, 
                                 const void *private_intfo,
                                 size_t cb,
                                 void *user_data){
    cout << "ERROR during context use: " << err_info << endl;
    exit(EXIT_FAILURE);
}
