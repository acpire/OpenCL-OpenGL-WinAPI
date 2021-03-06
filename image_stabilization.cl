R"===(
#define PI 3.14159265359f

float Gaussian_filter_x(float x, float sigma, float mean){
	const float _x = x - mean;
	const float _sigma = 2*sigma;
	const float div = _x / _sigma;
	return native_exp(-div * div)/(sigma * sqrt(2.0f * PI));
}

float Gaussian_filter_y(float y, float sigma, float mean){
	const float _y = y - mean;
	const float _sigma = 2*sigma;
	const float div = _y / _sigma;
	return native_exp(-div * div)/(sigma * sqrt(2.0f * PI));
}
float Gaussian_filter_xy(float y, float x, float sigma){
	const float _y = y * y;
	const float _x = x * x;
	const float _sigma = 2.0f*sigma*sigma;
	const float div = (_y + _x) / _sigma;
	return native_exp(-div)/(_sigma * PI);
}
__kernel void image_stabilization_float4_image_rgba(read_only image2d_t image_current, read_only image2d_t image_next,__global float4* result_buffer, int width_current, int height_current,  int width_next, int height_next, int block_x,const int block_y){

    int idx = get_global_id(0);
    int idy = get_global_id(1);
	float f_block_x = convert_float(block_x);
	float f_block_y = convert_float(block_y);
    const int stride_x = get_global_size(0);
    const int stride_y = get_global_size(1);
	int number_blocks_x = width_current / block_x;
	int number_blocks_y = height_current/ block_y;
	float mean_xy = 0.0f;
	float sigma = 0.0f;
	if (idx==0&&idy==0)
	printf("%f\n", f_block_x);
	for (float i =-f_block_y/ 2; i < f_block_y/ 2; i+=1.0f)
		for (float j = -f_block_x/ 2; j < f_block_x/ 2; j+=1.0f)
			mean_xy += i * f_block_x + j;
	mean_xy /= (f_block_y * f_block_x);
	float sum = 0.0f;
	for (float i =-f_block_y/ 2; i < f_block_y/ 2; i+=1.0f)
		for (float j = -f_block_x/ 2; j < f_block_x/ 2; j+=1.0f){
			sigma = (i * f_block_x + j) - mean_xy;
			sigma *= sigma;
			sum += sigma;
		}
	sigma = sum;
	sigma /= (f_block_y * f_block_x);
	sigma = sqrt(sigma);
	sigma = 1;
	for (int h = idy; h < number_blocks_y; h+=stride_y){
		for (int w = idx; w <  number_blocks_x; w+=stride_x){
			for (int i = 0; i < block_y; i++){
				float f_i = convert_float(i);
				for (int j = 0; j < block_x; j++){
					float f_j = convert_float(j);
					float result = Gaussian_filter_xy(f_i - f_block_y/ 2 , f_j - f_block_x/ 2, sigma);
					result_buffer[(h * block_y + i) * width_current + (w * block_x + j)] = (float4)result;
					printf("%f\n", result);
				}
			}
		}
	}

}

#ifdef cl_khr_mipmap_image
#pragma OPENCL EXTENSION cl_khr_mipmap_image : enable
__kernel void image_stabilization_float4_image_rgba(read_only image2d_t image_current, read_only image2d_t image_next, write_only image2d_t image_write, const int width_current,const int height_current, const int width_next,const int height_next){

    int idx = get_global_id(0);
    int idy = get_global_id(1);
    int image_current_num_mip_levels = get_image_num_mip_levels(image_current);
    int image_next_num_mip_levels = get_image_num_mip_levels(image_next);


}
#endif

)==="