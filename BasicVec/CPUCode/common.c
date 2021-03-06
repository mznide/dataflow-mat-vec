#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"

int calc_align(int n, int align) {
	return n / align * align + (n % align > 0) * align;
}

void align_matrix(int n, int n_aligned, float *mat, float *mat_aligned)
{
	for (int i = 0; i < n_aligned; i++) {
		for (int j = 0; j < n_aligned; j++){
			if (i <n && j<n) {
				mat_aligned[i * n_aligned + j] = mat[i * n + j];
			}
			else {
				mat_aligned[i * n_aligned + j] = 0;
			}
		}
	}
}


float sum_vec(int n, float *vec) {
	float sum = 0;
	for (int i = 0; i < n; i++)
		sum += vec[i];
	return sum;
}

void generate_vector(int n, float *vector, int range) {
	for (int i = 0; i < n; i++) {
		vector[i] = vector[i] = rand_signdouble(range);
	}
}

void generate_matrix(int n, float *input, int range)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			input[i * n + j] = rand_signdouble(range);
}

void multiply_CPU(int n, float *input, float *vector, float *output){
	for (int i = 0; i < n; i++) {
		output[i] = 0.0;
		for (int j = 0; j < n; j++) {
			output[i] += input[i * n + j] * vector[j];
		}
	}
}

void transpose (int n, float *input, float *inputTransposed){
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			inputTransposed[j + n * i] = input[j * n + i];
}


int check(int size, float *output, float *expected)
{
	int status = 0;
	for (int i = 0; i < size; i++) {
		if (output[i] != expected[i]) {
			fprintf(stderr, "[%d] error, output: %f != expected: %f\n",
				i, output[i], expected[i]);
			status = 1;
		}
	}
	return status;
}


void error(const int status, const char * fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, argp);
    fprintf(stderr, "\n");
    va_end(argp);
    exit(status);
}


void rand_init(const int seed) {
    srand(seed >= 0 ? (unsigned int) seed : (unsigned int) time(NULL));
}


short rand_sign() {
    return rand() < (RAND_MAX >> 1) ? 1 : -1;
}


int rand_int(const int range) {
    return rand() % range;
}


double rand_double(const int range) {
    double r = range * (double)rand() / (double)RAND_MAX;
    if (r == 0) return rand_double(range);
    return r;
}


double rand_signdouble(const int range) {
    return rand_double(range) *
    rand_sign();
}


void timer_start(timing_t * t) {
#ifdef PLAINTIMER
    gettimeofday(&t->realbegin, NULL);
#else
    clock_gettime(CLOCK_MONOTONIC, &t->realbegin);
#endif
    t->cpubegin = clock();
}


void timer_stop(timing_t * t) {
#ifdef PLAINTIMER
    gettimeofday(&t->realend, NULL);
#else
    clock_gettime(CLOCK_MONOTONIC, &t->realend);
#endif
    t->cpuend = clock();
    // calculate the difference in ms
    t->realtime = (t->realend.tv_sec - t->realbegin.tv_sec) * 1000;
#ifdef PLAINTIMER
    t->realtime += (t->realend.tv_usec - t->realbegin.tv_usec) / 1000;
#else
    t->realtime += (t->realend.tv_nsec - t->realbegin.tv_nsec) / 1000;
#endif
    t->cputime = (t->cpuend - t->cpubegin) * 1000 / CLOCKS_PER_SEC;
}
