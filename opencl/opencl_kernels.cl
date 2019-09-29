__kernel void add_vectors(__global float *a, 
                          __global float *b, 
                          __global float *c,
                          int num_elements) {
    int idx = get_global_id(0);
    if (idx < num_elements) {
        c[idx] = a[idx] + b[idx];
        //printf("id: %d) %f + %f = %f\n", idx, a[idx], b[idx], c[idx]);
    }

}



__kernel void mult_vectors(__global float* a, 
                           __global float *b, 
                           __global float *c,
                           int num_elements) {
    int idx = get_global_id(0);
    if (idx < num_elements)
        c[idx] = a[idx] * b[idx];
}
