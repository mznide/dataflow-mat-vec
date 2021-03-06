/**
 * 	Summary:
 *     CPU code for Matrix*vector multiplication.
 *     Ensures inputs are transposed to match the order they are consumed in by the kernel.
 * 	   # Matrix is read as a stream (transposed matrix)
 *	   # x is read as a stream (every n ticks)
 *	   # big loop (size=n)
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <MaxSLiCInterface.h>
#include "Maxfiles.h"
#include "common.h"

#define VECTOR_SIZE MatVecMultiply_vectorSize
#define C MatVecMultiply_C

void help(const char * cmd) {
    printf("Usage: %s [filename]\n", cmd);
    printf("\nOptions:\n");
    printf("  -h, --help\n\tPrint short help\n");
    printf("  -n, --size\n\tSize n of matrix\n");
    printf("  -r, --range\n\tRange of elements\n");
    printf("  -t, --trace\n\tTrace level: 0,1,2\n");

};

// show help
int help_flag = 0;

// number of rows / columns. Matrix size is n*n
int n = 36;

/* tracing
	0 - prints only n, sum of result, realtime, cputime
	1 - prints input, output, final result
	2 - tests correctness of result and prints if test passed
*/
int trace = 0;

// elements of matrix are in -range  to +range interval
float range = 100.0;

struct option options[] = {
	{"help",	required_argument, 0, 'h'},
	{"size",	required_argument, 0, 'n'},
	{"trace",	required_argument, 0, 't'},
	{"range",	required_argument, 0, 'r'},
	{0,0,0,0}
};

#define SHORTOPT "hn:t:r:"

void parse_args(int argc, char * argv[]) {
	while (1) {
		int option_index = 0;
		int opt = getopt_long(argc, argv, SHORTOPT, options, &option_index);

		if (opt == -1) break;

		switch (opt) {
			case 'h':
				help_flag = 1;
				break;
			case 'n':
				n = atoi(optarg);
				break;
			case 't':
				trace = atoi(optarg);
				break;
			case 'r':
				range = atoi(optarg);
				break;
			case '?':
				error(1, "Invalid1 option '%c'", optopt);
			default:
				abort();
		}
	}
	if (help_flag) {
		help(argv[0]);
		exit(0);
	}
}

//transforms matrix for vectorized multiplication
void transform_vector (int n, float *input, float *matrixTransformed, int vectorSize){
	for (int v = 0; v < n; v=v+vectorSize) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < vectorSize; j++) {
				matrixTransformed[i*vectorSize +j + v*n] = input[j+i*n+v];

			}
		}
	}
}

void transformC(int n, float *mat, float *matrixTransformed, int C1){
	int count = 0;
	for (int yy = 0; yy < n; yy += C1) {
		for ( int x = 0; x < n; ++x) {
			for ( int y = yy; y < yy + C1; ++y) {
				matrixTransformed[count] = mat[y * n + x];
				count++;
			}
		}
	}
}

void transform_vector_C(int n, float *mat, float *matrixTransformed, int vectorSize, int C1){
	int count = 0;
	for (int yy = 0; yy < n; yy += C1) {
		for ( int x = 0; x < n/vectorSize; ++x) {
			for ( int y = yy; y < yy + C1; ++y) {
				for (int v = 0; v < vectorSize; v++){
					matrixTransformed[count] = mat[y * n + x*vectorSize+v];
					count++;
				}
			}
		}
	}
}

void print_matrix(int n, float *mat){
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			printf("%f " , mat[i*n+j]);
		}
		printf("\n");
	}
}


int main(int argc, char * argv[])
{
	if (argc > 1){
		parse_args(argc, argv);
	}

	const int size = n * n;

	float *mat = malloc(size * sizeof(float));

	float *vec = malloc(n * sizeof(float));

	float *output = malloc(n * sizeof(float));
	float *output1 = malloc(n * sizeof(float)*C);
	float *expected = malloc(n * sizeof(float));


	generate_matrix(n, mat, range);
	generate_vector(n, vec, range);

	timing_t timer1;
	timer_start(&timer1);
	timing_t timer2;
	timer_start(&timer2);
	float *mat_transformed = malloc(size * sizeof(float));
	transform_vector_C(n, mat, mat_transformed, VECTOR_SIZE, C);


	timer_stop(&timer2);
	MatVecMultiply(size, n, mat_transformed, vec, output);

	for (int i=0; i<n; i++){
		for (int j=0; j<C; j++)
			output[i] += output1[i+n*j];
	}

	timer_stop(&timer1);
	float sum = sum_vec(n, output);

	printf("%d %f %ld %ld %ld %ld\n", n, sum, timer1.realtime, timer1.cputime, timer2.realtime, timer2.cputime);

	if (trace == 1) {

		printf("\nInput matrix\n");
		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%.2f " , mat_transformed[i*n+j]);
			}
			printf("\n");
		}

		printf("\nInput vector \n");
		for (int i=0; i<n; i++){
			printf("%f " , vec[i]);
		}

		printf("\n\nResult\n");
		for (int i=0; i<n; i++){
			printf("%f " , output[i]);
		}
		printf("\n");

		printf("\n\nResult\n");
		for (int i=0; i<n*2; i++){
			printf("%f " , output1[i]);
		}
		printf("\n");
	}
	else if (trace == 2) {
		multiply_CPU(n, mat, vec, expected);
		int status = check(n, output, expected);
		if (status)
			printf("Test failed.\n");
		else
			printf("Test passed OK!\n");
		return status;
	}

	free(mat);
	free(vec);
	free(output);
	free(output1);
	free(expected);
	free(mat_transformed);

	return 0;
}
