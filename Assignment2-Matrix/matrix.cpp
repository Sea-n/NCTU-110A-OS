#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>
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

unsigned paral(int N, int K);

unsigned A[800][800];

int main() {
	timeval beg, end;
	unsigned chk;
	float sec;
	int N;

	cout << "Input the matrix dimension: ";
	cin >> N;

	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			A[i][j] = i*N + j;

	for (int K=1; K<=16; K++) {
		gettimeofday(&beg, 0);
		printf("Multiplying matrices using %d process\n", K);

		chk = paral(N, K);

		gettimeofday(&end, 0);
		sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
		printf("Elapsed time: %.6lf sec, Checksum: %u\n", sec, chk);
	}
}

unsigned paral(int N, int K) {
	unsigned chk = 0;
	int shmid;
	int L, R;
	int *C;

	shmid = shmget(IPC_PRIVATE, 800*800*4, IPC_CREAT | 0600);
	C = (int *) shmat(shmid, NULL, 0);

	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			C[i*800 + j] = 0;

	for (int k=0; k<K; k++) {
		pid_t pid = fork();
		if (pid < 0) {
			exit(-1);
		} else if (pid == 0) {
			C = (int *) shmat(shmid, NULL, 0);
			L = (N+K-1) / K * k;
			R = (k == K-1) ? N : ((N+K-1) / K * (k+1));

			for (int i=L; i<R; i++)
				for (int j=0; j<N; j++)
					for (int k=0; k<N; k++)
						C[i*800 + j] += A[i][k] * A[k][j];

			shmdt(C);
			_exit(0);
		}
	}

	for (int k=0; k<K; k++)
		wait(NULL);

	shmctl(shmid, IPC_RMID, NULL);

	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++)
			chk += C[i*800 + j];

	return chk;
}
