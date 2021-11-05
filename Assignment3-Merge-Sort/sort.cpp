#include <semaphore.h>
#include <sys/time.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

/*
 * Student ID: 0816146
 * Student Name: Sean Wei
 * Email: nctu-os@sean.taipei
 * SE tag: xnxcxtxuxoxsx
 * Statement: I am fully aware that this program is not supposed to be posted
 * to a public server, such as a public GitHub repository or a public web page.
 */

void bubble_sort(int L, int R);
void merge_sort(int L, int R);
void *worker(void *);

sem_t MUTEX, DONE;
int A[1000200];
int progress;  // Treat as bit array: (1-7) merge sort, (8-15) bubble sort
queue<int> Q;
int N;

int main() {
	char out_name[]{"output_?.txt"};
	timeval beg, end;
	pthread_t t[8];
	ifstream inp;
	ofstream out;
	float ms;

	for (int T=1; T<=8; T++) {
		/* Reset array everytime, not count as exec time */
		inp.open("input.txt");
		inp >> N;
		for (int i=0; i<N; i++)
			inp >> A[i];
		inp.close();

		gettimeofday(&beg, 0);

		sem_init(&MUTEX, 0, 1);
		sem_init(&DONE, 0, 0);
		progress = 0;

		/* Initial jobs */
		sem_wait(&MUTEX);
		for (int i=8; i<16; i++)
			Q.push(i);
		sem_post(&MUTEX);

		/* Start T threads */
		for (int i=0; i<T; i++)
			pthread_create(&t[i], NULL, worker, NULL);

		/* Wait for T threads done */
		for (int i=0; i<T; i++)
			sem_wait(&DONE);

		gettimeofday(&end, 0);
		ms = (end.tv_sec - beg.tv_sec) * 1e3 + (end.tv_usec - beg.tv_usec) / 1e3;
		printf("worker thread #%d, elapsed %.3lf ms\n", T, ms);

		/* Output result array, not count as exec time */
		out_name[7] = '0' | T;
		out.open(out_name);
		for (int i=0; i<N; i++)
			out << A[i] << ' ';
		out.close();

		sem_destroy(&MUTEX);
		sem_destroy(&DONE);
	}
}

void bubble_sort(const int L, const int R) {
	for (int i=L; i<R-1; i++)
		for (int j=i; j<R; j++)
			if (A[i] > A[j])
				swap(A[i], A[j]);
}

void merge_sort(const int L, const int R) {
	int M = (L+R) >> 1;
	int k = L, p = L, q = M;
	int B[N];

	while (p < M && q < R)
		B[k++] = (A[p] <= A[q]) ? A[p++] : A[q++];
	while (p < M) B[k++] = A[p++];
	while (q < R) B[k++] = A[q++];

	for (int i=L; i<R; i++)
		A[i] = B[i];
}

int LT[] = {0, 0, 0, 4, 0, 2, 4, 6};
int RT[] = {0, 8, 4, 8, 2, 4, 6, 8};
void *worker(void *) {
	int job, L, R;
	for (;;) {
		/* Critical section: Get job from queue */
		sem_wait(&MUTEX);

		if (Q.empty()) {
			sem_post(&MUTEX);
			sem_post(&DONE);
			return nullptr;
		}

		job = Q.front(); Q.pop();
		sem_post(&MUTEX);

		if (job > 7) {
			L = N * (job-8) / 8;
			R = N * (job-7) / 8;
			bubble_sort(L, R);
		} else {
			L = N * LT[job] / 8;
			R = N * RT[job] / 8;
			merge_sort(L, R);
		}

		/* Critical section: Dispatch new job if applicable */
		sem_wait(&MUTEX);
		progress |= 1<<job;
		if (progress & 1<<(job^1))
			Q.push(job>>1);
		sem_post(&MUTEX);
	}
}
