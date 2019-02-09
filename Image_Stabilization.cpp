#include "Image_Stabilization.h"

Image_Stabilization::Image_Stabilization(clDevice* device, std::vector<dataImage> frames, std::vector<dataImage> result_frames)
{
	cl_uchar type_arguments[] = { sizeof(cl_uint) };
	cl_uint* norm_image_gpu = (cl_uint*)_malloca(frames.size() * sizeof(cl_uint));
	for (ptrdiff_t i = 0; i < frames.size(); i++) {
		size_t length_row_pitch_data = frames[i].width * sizeof(cl_uchar4);
		norm_image_gpu[i] = device->mallocImageMemory((const void*)frames[i].data, frames[i].height, frames[i].width, length_row_pitch_data, CL_RGBA, CL_UNORM_INT8);
	}
	kernel_image_stabilization = device->findKernel((const cl_char*)"image_stabilization_float4_image_rgba", sizeof("Image_Stabilization_float4_image_rgba"));

	cl_uint result = device->mallocBufferMemory(NULL, 4 * frames[0].height* frames[0].width * sizeof(cl_float));
	{
		cl_uint block_x = 15;
		cl_uint block_y = 15;
		cl_uint indices_images[] = { norm_image_gpu[0], norm_image_gpu[1] };
		cl_uint indices_buffers[] = { result };
		cl_uint indices_args[] = { 15, 15, frames[1].width, frames[1].height, block_x, block_y };
		cl_int length_args[] = { sizeof(cl_uint), sizeof(cl_uint), sizeof(cl_uint), sizeof(cl_uint), sizeof(cl_uint), sizeof(cl_uint) };
		device->(kernel_image_stabilization, indices_images, indices_buffers, (cl_char*)indices_args, length_args, sizeof(indices_images) / sizeof(cl_uint),
			sizeof(indices_buffers) / sizeof(cl_uint), sizeof(length_args) / sizeof(cl_int), 1, 1, 1);
	}
	device->readBuffer((void*) frames[0].data, result, frames[0].height * frames[0].width * sizeof(cl_float));
	device->popBufferMemory();
	for (ptrdiff_t i = frames.size() - 1; i != -1; i--) {
		device->freeImageMemory(norm_image_gpu[i]);
	}
	_freea(norm_image_gpu);
}

Image_Stabilization::~Image_Stabilization()
{
}