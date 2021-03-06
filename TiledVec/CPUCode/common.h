#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <sys/time.h>


int calc_align(int n, int align);
void align_matrix(int n, int n_aligned, float *mat, float *mat_aligned);


void generate_vector(int n, float *vector, int range);
void generate_matrix(int n, float *input, int range);
void multiply_CPU(int n, float *input, float *vector, float *output);
void transpose (int n, float *input, float *input_transposed);

int check(int size, float *output, float *expected);
float sum_vec(int n, float *vec);

void error(const int status, const char * fmt, ...);

void rand_init(const int seed);
short rand_sign();
int rand_int(const int range);
double rand_double(const int range);
double rand_signdouble(const int range);


#define PLAINTIMER

#ifdef __MACH__
#define PLAINTIMER
#endif

// timer_t is already used in time_h (Linux)
typedef struct timing_t {
#ifdef PLAINTIMER
    struct timeval realbegin, realend;
#else
    struct timespec realbegin, realend;
#endif
    clock_t cpubegin, cpuend;
    //
    clock_t realtime, cputime;  // in ms
} timing_t;

void timer_start(timing_t * t);
void timer_stop(timing_t * t);


#endif
