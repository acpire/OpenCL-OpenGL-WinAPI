#include "Image_Stabilization.h"



Image_Stabilization::Image_Stabilization(clDevice* device, std::vector<dataImage> frames, std::vector<dataImage> result_frames)
{
	cl_uchar type_arguments[] = { sizeof(cl_uint) };
	size_t _type_image[] = { CL_RGBA };
	size_t _type_data[] = { CL_FLOAT, CL_UNORM_INT8, CL_UNSIGNED_INT8 };
	cl_uint* norm_image_gpu = (cl_uint*)alloca(frames.size() * sizeof(cl_uint));
	for (size_t i = 0; i < frames.size(); i++) {
		size_t length_row_pitch_data = frames[i].width * sizeof(cl_uchar4);
		norm_image_gpu[i] = device->mallocImageMemory((const void**)&frames[i].data, &frames[i].height, &frames[i].width, &length_row_pitch_data, 1, _type_image, _type_data + 1);
	}
	kernel_image_stabilization = device->findKernel((const cl_char*)"image_stabilization_float4_image_rgba", sizeof("Image_Stabilization_float4_image_rgba"));

	for (size_t i = 0; i < frames.size(); i++) {
		device->freeImageMemory(norm_image_gpu[i]);
	}
}


Image_Stabilization::~Image_Stabilization()
{
}
