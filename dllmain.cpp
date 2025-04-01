#include <CL/cl.h>  // Include OpenCL (Assicurati che il percorso sia configurato correttamente)
#include <iostream>

// Dichiarazioni di variabili per OpenCL
cl_context opencl_context = nullptr;
cl_command_queue opencl_command_queue = nullptr;
cl_device_id opencl_device = nullptr;

// Funzione per inizializzare OpenCL
void initializeOpenCL() {
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, nullptr);  // Recupera la piattaforma OpenCL disponibile

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &opencl_device, nullptr);  // Usa la GPU
    opencl_context = clCreateContext(nullptr, 1, &opencl_device, nullptr, nullptr, nullptr);
    opencl_command_queue = clCreateCommandQueueWithProperties(opencl_context, opencl_device, nullptr, nullptr);
}

// Funzione per liberare le risorse di OpenCL
void cleanupOpenCL() {
    if (opencl_command_queue) {
        clReleaseCommandQueue(opencl_command_queue);
    }
    if (opencl_context) {
        clReleaseContext(opencl_context);
    }
}

// Funzioni CUDA emulate con OpenCL

extern "C" __declspec(dllexport) int cudaInit(int flags) {
    return 0; // Simula l'inizializzazione CUDA
}

extern "C" __declspec(dllexport) int cudaDeviceGet(int* device, int deviceOrdinal) {
    *device = 0;  // Simula il primo dispositivo OpenCL
    return 0; // Simula successo
}

extern "C" __declspec(dllexport) void* cudaMalloc(size_t size) {
    cl_int err;
    cl_mem buffer = clCreateBuffer(opencl_context, CL_MEM_READ_WRITE, size, NULL, &err);
    if (err != CL_SUCCESS) return nullptr;
    return (void*)buffer;  // Restituisce buffer OpenCL
}

extern "C" __declspec(dllexport) int cudaMemcpy(void* dst, const void* src, size_t size, int direction) {
    cl_int err = 0;
    if (direction == 1) {
        err = clEnqueueWriteBuffer(opencl_command_queue, (cl_mem)dst, CL_TRUE, 0, size, src, 0, nullptr, nullptr);
    }
    else if (direction == 2) {
        err = clEnqueueReadBuffer(opencl_command_queue, (cl_mem)src, CL_TRUE, 0, size, dst, 0, nullptr, nullptr);
    }
    return (err == CL_SUCCESS) ? 0 : -1;
}

extern "C" __declspec(dllexport) int cudaFree(void* ptr) {
    cl_int err = clReleaseMemObject((cl_mem)ptr);
    return (err == CL_SUCCESS) ? 0 : -1;
}

extern "C" __declspec(dllexport) void initialize() {
    initializeOpenCL();  // Inizializza OpenCL
}

extern "C" __declspec(dllexport) void cleanup() {
    cleanupOpenCL();  // Rilascia le risorse di OpenCL
}
