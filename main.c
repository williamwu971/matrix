#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <stdint.h>

void multiply(const float *mata, size_t mata_width, size_t mata_height,
              const float *matb, size_t matb_width, size_t matb_height,
              float **result_mat, size_t *res_width, size_t *res_height) {

    (void) matb_height;

    if (result_mat) {

        *result_mat = calloc(mata_height * matb_width, sizeof(float));

#pragma omp parallel
        {

            int thread_num = omp_get_thread_num();
            int num_threads = omp_get_num_threads();

            size_t start = mata_height / num_threads * thread_num;
            size_t end = mata_height / num_threads * (thread_num + 1);

            if (thread_num == num_threads - 1) {
                end = mata_height;
            }

            printf("thread %7d/%-7d from %7zu to %-7zu\n", thread_num, num_threads, start, end);

            for (size_t y = start; y < end; y++) {
                for (size_t x = 0; x < matb_width; x++) {
                    for (size_t k = 0; k < mata_width; k++) {
                        (*result_mat)[(y * matb_width) + x] +=
                                (mata[(y * mata_width) + k] *
                                 matb[(k * matb_width) + x]);
                    }
                }
            }

        }

        *res_width = matb_width;
        *res_height = mata_height;
    }
}

float *generate_matrix(long width, long height) {
    float *m = malloc(width * height * sizeof(float));
    srand(time(NULL));
    int base = rand();

    for (long y = 0; y < height; y++) {
        for (long x = 0; x < width; x++) {
            long idx = y * width + x;
            m[idx] = (float) idx + (float) base;
        }
    }

    return m;
}

int main(int argc, char **argv) {

    if (argc != 3) return 1;

    long width = strtol(argv[1], NULL, 10);
    long num_threads = strtol(argv[2], NULL, 10);

    omp_set_num_threads((int) num_threads);


    float *A = generate_matrix(width, width);
    float *B = generate_matrix(width, width);
    float *result_mat = NULL;
    size_t res_width = 0;
    size_t res_height = 0;

    uint64_t elapsed;
    struct timeval st, et;
    gettimeofday(&st, NULL);

    multiply(
            A, width, width,
            B, width, width,
            &result_mat, &res_width, &res_height
    );

    gettimeofday(&et, NULL);
    elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec) + 1;
    printf("(%s,%d) [%6lus] \n", __FUNCTION__, __LINE__, elapsed / 1000000);
    fprintf(stderr, "%12.3f ", (double) elapsed / 1000000.0f);

    assert(result_mat != NULL);
    assert(res_width != 0);
    assert(res_height != 0);

    printf("Hello, World!\n");
    return 0;
}
