// GPGPU test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "OpenCV.h"
#include "glDevice.h"
#include "clDevice.h"
#include "Load_Save_Images.h"
#include "WinAPI.h"
#include "InverseFilter.h"
#include "Wiener_filter.h"
#include "MakeNoise.h"
#include "fractalImageCompression.h"
#include "Image_Stabilization.h"

const char convolution[] =
#include "convolution.cl"
;
const char fourier[] =
#include "fourier_transform.cl"
;
const char filters[] =
#include "filters.cl"
;
const char fractals[] =
#include "fractal.cl"
;

const char image_stabilization[] =
#include "image_stabilization.cl"
;

int main()
{
	WinAPI window_api;
	window_api.InitWindow();
	window_api.InitOpenGLContext();
	glDevice gl_device;
	clPlatform platform;

	std::vector<std::wstring> dir_images = WinAPI::GetNameFolderFiles(L"Images");
	std::vector<dataImage> images(dir_images.size());
	size_t index = 0;
	for (size_t i = 0; i < dir_images.size(); i++)
		images[i] = WIN_load_image(dir_images[i].c_str());

	cl_char* place = (cl_char*)_aligned_malloc(platform.getNumberDevices() * sizeof(clDevice), alignof(clDevice));
	clDevice** cl_devices = (clDevice**)_aligned_malloc(platform.getNumberDevices() * sizeof(clDevice*), alignof(clDevice*));
	cl_char* aligned_place = place;
	for (size_t i = 0; i < platform.getNumberDevices(); i++) {
		cl_devices[i] = new(aligned_place) clDevice(&platform, i);
		aligned_place += sizeof(clDevice);
		cl_devices[i]->clPushProgram((cl_char*)convolution, sizeof(convolution), (cl_char*)"-cl-unsafe-math-optimizations -cl-fast-relaxed-math");
		cl_devices[i]->clPushProgram((cl_char*)fourier, sizeof(fourier), (cl_char*)"-cl-unsafe-math-optimizations -cl-fast-relaxed-math");
		cl_devices[i]->clPushProgram((cl_char*)filters, sizeof(filters), NULL);

		cl_devices[i]->clPushProgram((cl_char*)image_stabilization, sizeof(image_stabilization), NULL);
	}

	size_t i = 0;
	if (i < platform.getNumberDevices()) {
		Image_Stabilization(cl_devices[i], images, images);
	}
	index = 0;
	for (auto i : dir_images) {
		size_t start = i.size() - 1;
		std::wstring name;
		while (i[start - 1] != L'//'&&start!=0)
			start--;
		while (start != i.size() && i[start] != '.')
			name.push_back(i[start++]);
		name.push_back(0);
		WIN_save_image(images[index++], L"Result//", name.c_str(), PNG);
	}
	window_api.DispatchMessageWindow();
	_aligned_free(cl_devices);
	_aligned_free(place);
}