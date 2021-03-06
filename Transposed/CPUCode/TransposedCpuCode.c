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
int n = 16;

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


int main(int argc, char * argv[])
{
	if (argc > 1){
		parse_args(argc, argv);
	}

	const int size = n * n;

	float *mat = malloc(size * sizeof(float));
	float *vec = malloc(n * sizeof(float));
	float *output = malloc(n * sizeof(float));
	float *expected = malloc(n * sizeof(float));
	float *mat_transposed = malloc(n * n * sizeof(float));

	generate_matrix(n, mat, range);
	generate_vector(n, vec, range);

	timing_t timer1;
	timer_start(&timer1);

	transpose(n, mat, mat_transposed);
	MatVecMultiply(size, n, mat_transposed, vec, output);

	timer_stop(&timer1);
	float sum = sum_vec(n, output);


	printf("%d %f %ld %ld\n", n, sum, timer1.realtime, timer1.cputime);


	if (trace == 1) {

		printf("\nInput matrix\n");

		for (int i=0; i<n; i++){
			for (int j=0; j<n; j++){
				printf("%f " , mat[i*n+j]);
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
	free(expected);
	free(mat_transposed);

	return 0;
}
