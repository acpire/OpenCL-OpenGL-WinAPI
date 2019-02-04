R"===(

__kernel void image_stabilization_float4_image_rgba(read_only image2d_t image_current, read_only image2d_t image_next, write_only image2d_t image_write, const int width,const int height){

    int idx = get_global_id(0);
    int idy = get_global_id(1);


}

#ifdef cl_khr_mipmap_image
#pragma OPENCL EXTENSION cl_khr_mipmap_image : enable
__kernel void image_stabilization_float4_image_rgba(read_only image2d_t image_current, read_only image2d_t image_next, write_only image2d_t image_write, const int width,const int height){

    int idx = get_global_id(0);
    int idy = get_global_id(1);
    int image_current_num_mip_levels = get_image_num_mip_levels(image_current);
    int image_next_num_mip_levels = get_image_num_mip_levels(image_next);


}
#endif

)==="