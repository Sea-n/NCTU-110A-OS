#include <iostream>

using namespace std;

/*
 * Student ID: 0816146
 * Student Name: Sean Wei
 * Email: nctu-os@sean.taipei
 * SE tag: xnxcxtxuxoxsx
 * Statement: I am fully aware that this program is not supposed to be posted
 * to a public server, such as a public GitHub repository or a public web page.
 */

int main() {
	int N, chk;
	clock_t begin, end;
	double cpu_time_used;
	unsigned A[800][800];
	unsigned B[800][800];
	unsigned C[800][800];

	cout << "Input the matrix dimension: ";
	cin >> N;

	/* Init */
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++) {
			A[i][j] = i*N + j;
			B[i][j] = i*N + j;
			C[i][j] = 0;
		}

	/* Calc */
	printf("Multiplying matrices using %d process\n", 1);
	begin = clock();
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			for (int k=0; k<N; k++)
				C[i][j] += A[i][k] * B[k][j];

	chk = 0;
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			chk += C[i][j];

	end = clock();
	cpu_time_used = ((double) (end - begin)) / CLOCKS_PER_SEC;
	printf("Elapsed time: %.6lf sec, Checksum: %u\n", cpu_time_used, chk);
}
