#include "cl_util.h"
#include "jpeg_util.h"
#include "color_transf.h"

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
cl_kernel dct_kernel;
cl_kernel inv_dct_kernel;
cl_int error;

//buffers for dct
cl_mem block_src;
cl_mem block_dst;
cl_mem cl_luminace_table;
cl_mem cl_chrominace_table;

// kernel, rgb>YCbCr
cl_kernel kernel_ycbcr;

// buffers for color transormation
cl_mem src_rgb;
cl_mem dst_ycbcr;

//max number of local work items
size_t max_work_item_size[3];
size_t dct_max_local_work_item_size[3];

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


size_t set_dct_size(size_t i){
    if(i>=8){
        return 8;
    }
    else if(i<8 && i>=4){
        return 4;
    }
    else if(i<4 && i>=2){
        return 2;
    }
    else return 1;
}

void set_max_dct_device_worksize(){
    error = clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(max_work_item_size), max_work_item_size, NULL);
    checkClError(error, "clGetDeviceInfo");
    dct_max_local_work_item_size[0] = set_dct_size(max_work_item_size[0]);
    dct_max_local_work_item_size[1] = set_dct_size(max_work_item_size[1]);
    dct_max_local_work_item_size[2] = set_dct_size(max_work_item_size[2]);
//    cout << dct_max_local_work_item_size[0] << " " << dct_max_local_work_item_size [1] << " " <<dct_max_local_work_item_size[2] << "\n";
}


int initOpenCL(){
    error = getPlatformID();
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

    //sets max number of workgroups
    set_max_dct_device_worksize();

    // Command-queue
    queue = clCreateCommandQueue(context, devices[0], 0, &error);
    checkClError(error, "clCreateCommandQueue");

 /* // musela jsem zakomentovat, jinak to pres tohle neproslo.
    // Load kernels
    error = loadKernelFromFile("../src/jpeg.cl", &dct_kernel, "dct8x8");
    checkClError(error, "loadKernelFromFile");
    error = loadKernelFromFile("../src/jpeg.cl", &inv_dct_kernel, "inv_dct8x8");
    checkClError(error, "loadKernelFromFile");
    */

    error = loadKernelFromFile("../src/ycbcr.cl", &kernel_ycbcr, "ycbcr");
    checkClError(error, "loadKernelFromFile");

    // Alloc buffers with const size & copy data to quantization table buffers
    block_src = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_float)*64, NULL, &error);
    checkClError(error, "clCreateBuffer");
    block_dst = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float)*64, NULL, &error);
    checkClError(error, "clCreateBuffer");

    cl_luminace_table = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                sizeof(cl_int)*64, NULL, &error);
    checkClError(error, "clCreateBuffer");
    cl_chrominace_table = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                sizeof(cl_int)*64, NULL, &error);
    checkClError(error, "clCreateBuffer");
    error = clEnqueueWriteBuffer(queue,
            cl_luminace_table, //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_int)*64, //size in bytes of copied data
            luminace_table,       //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list
    checkClError(error,"clEnqueueWriteBuffer");
    error = clEnqueueWriteBuffer(queue,
            cl_chrominace_table, //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_int)*64, //size in bytes of copied data
            chrominace_table,       //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list
    checkClError(error,"clEnqueueWriteBuffer");
}

cl_int loadKernelFromFile(const char* fileName, cl_kernel* kernel, char* kernel_name){
    ifstream src_file(fileName);
    if(!src_file.is_open()) return EXIT_FAILURE;

    string src_prog(istreambuf_iterator<char>(src_file), (istreambuf_iterator<char>()));
    const char *src = src_prog.c_str();
    size_t length = src_prog.length();

    program = clCreateProgramWithSource(context, 1, &src, &length, &error);
    checkClError(error, "clCreatePogramWithSource");

    error = clBuildProgram(program, dev_count, devices, NULL, NULL, NULL);
    /*
    size_t len;
    char log[32048];
    memset(log, 0, sizeof(log));
    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(log), log, &len);
    cout<<"--- Build log ---\n "<<log<<endl;
    */
    checkClError(error, /*"clBuildProgram"*/kernel_name);

    (*kernel) = clCreateKernel(program, kernel_name, &error);
    checkClError(error, "clCreateKernel");

    return CL_SUCCESS;
}


void dct8x8_gpu(float* src, float* dst, cl_mem* table){
    // Copy data from memory to gpu
    error = clEnqueueWriteBuffer(queue,
            block_src, //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_float)*64, //size in bytes of copied data
            src,       //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list
    size_t len;
    char log[32048];
    memset(log, 0, sizeof(log));
    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(log), log, &len);
    cout<<"--- Build log ---\n "<<log<<endl;
    checkClError(error,"clEnqueueWriteBuffer");

    clSetKernelArg(dct_kernel, 0, sizeof(cl_mem), (void *)&block_src);
    clSetKernelArg(dct_kernel, 1, sizeof(cl_mem), (void *)&block_dst);
    clSetKernelArg(dct_kernel, 2, sizeof(cl_mem), (void *)table);

    size_t GWS[2], LWS[2];
    GWS[0] = 8;
    GWS[1] = 8;
    LWS[0] = dct_max_local_work_item_size[0];
    LWS[1] = dct_max_local_work_item_size[1];

    clEnqueueNDRangeKernel(queue, dct_kernel, 2, NULL, GWS, LWS, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, block_dst, CL_TRUE, 0, sizeof(cl_float)*64, dst, 0, NULL, NULL);

    clFinish(queue);
}

void inv_dct8x8_gpu(float* src, float* dst){
    // Copy data from memory to gpu
    error = clEnqueueWriteBuffer(queue,
            block_src, //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_float)*64, //size in bytes of copied data
            src,       //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list
    checkClError(error,"clEnqueueWriteBuffer");

    clSetKernelArg(inv_dct_kernel, 0, sizeof(cl_mem), (void *)&block_src);
    clSetKernelArg(inv_dct_kernel, 1, sizeof(cl_mem), (void *)&block_dst);

    size_t GWS[2], LWS[2];
    GWS[0] = 8;
    GWS[1] = 8;
    LWS[0] = dct_max_local_work_item_size[0];
    LWS[1] = dct_max_local_work_item_size[1];

    clEnqueueNDRangeKernel(queue, inv_dct_kernel, 2, NULL, GWS, LWS, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, block_dst, CL_TRUE, 0, sizeof(cl_float)*64, dst, 0, NULL, NULL);

    clFinish(queue);
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/* Alocates buffers and loads the image into the gpu memory */
void load_picture_data_to_gpu(pixmap* data, int size){
    src_rgb = clCreateBuffer(context, CL_MEM_READ_ONLY,
            sizeof(unsigned char) * size, NULL, &error);
    checkClError(error, "clCreateBuffer");
    dst_ycbcr = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
            sizeof(unsigned char) * size, NULL, &error);
    checkClError(error, "clCreateBuffer");

    error = clEnqueueWriteBuffer(queue,
            src_rgb, //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(unsigned char) * size, //size in bytes of copied data
            data->pixels,      //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list
    checkClError(error,"clEnqueueWriteBuffer");
}

/* Greatest common divider */
int gcd (int num1,int num2){
    int pom;

    if (num1 < num2) {
        pom = num1;
        num1 = num2;
        num2 = pom;
    }

    if ((num1%num2) == 0)
        return (num2);
    else
        return gcd(num2,(num1%num2));
}


void ycbcr_gpu(pixmap* data, unsigned char* dst){
    load_picture_data_to_gpu(data, data->width * data->height * 3);

    clSetKernelArg(kernel_ycbcr, 0, sizeof(cl_mem), (void *)&src_rgb);
    clSetKernelArg(kernel_ycbcr, 1, sizeof(cl_uint), &(data->width));
    clSetKernelArg(kernel_ycbcr, 2, sizeof(cl_uint), &(data->height));
    clSetKernelArg(kernel_ycbcr, 3, sizeof(cl_mem), (void *)&dst_ycbcr);

    size_t GWS[2], LWS[2];
    GWS[0] = data->height*3;
    GWS[1] = data->width*3;
    LWS[0] = gcd(data->height*3, max_work_item_size[0]);
    LWS[1] = gcd(data->width*3,  max_work_item_size[1]);

    cout << "gcd - LWS[0]=" << gcd(data->height, max_work_item_size[0]) << endl;
    cout << "gcd - LWS[1]=" <<gcd(data->width,  max_work_item_size[1])  << endl;

    clEnqueueNDRangeKernel(queue, kernel_ycbcr, 2, NULL, GWS, LWS, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, dst_ycbcr, CL_TRUE, 0, sizeof(unsigned char) * data->width * data->height * 3, dst, 0, NULL, NULL);

    clFinish(queue);
}

void color_transform_gpu (const char* image) {
    pixmap *data = loadTGAdata(image);
    if (data == NULL) return;
    int size = data->width * data->height;

    unsigned char *dst = new unsigned char[size*3];
    cout << "pred." << endl;
    initOpenCL();
    ycbcr_gpu(data, dst);

    pixmap *Y, *Cb, *Cr;

    Y  = createPixmap(data->width, data->height, 1); // 1 byte per pixel
    Cb = createPixmap(data->width, data->height, 1);
    Cr = createPixmap(data->width, data->height, 1);

    memcpy ( Y->pixels,            dst, size);
    memcpy (Cb->pixels,     dst + size, size);
    memcpy (Cr->pixels, dst + size * 2, size);

    //rgb_to_ycbcr(Y, Cb, Cr, data);
    saveGrayscalePixmap( Y,  "Y_gpu.tga");
    saveGrayscalePixmap(Cb, "Cb_gpu.tga");
    saveGrayscalePixmap(Cr, "Cr_gpu.tga");
    free(dst);
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
        cout << "ERROR: " << CLErrorString(err) << " " << debug << endl;
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


double get_time(void)
{
#if _WIN32
    static int initialized = 0;
    static LARGE_INTEGER frequency;
    LARGE_INTEGER value;

    if (!initialized) {
        initialized = 1;
        if (QueryPerformanceFrequency(&frequency) == 0)         {
            exit(-1);
        }
    }

    QueryPerformanceCounter(&value);
    return (double)value.QuadPart / (double)frequency.QuadPart;
#else
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        exit(-2);
    }
    return (double)tv.tv_sec + (double)tv.tv_usec/1000000.;
#endif
}
