#include <sys/time.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#define F first
#define S second
#define ALL(v) v.begin(), v.end()
#define RM(vec, val) vec.erase(remove(ALL(vec), val))
#define SZ 1069
#define hash(x) (x % SZ)

using namespace std;

/*
 * Student ID: 0816146
 * Student Name: Sean Wei
 * Email: nctu-os@sean.taipei
 * SE tag: xnxcxtxuxoxsx
 * Statement: I am fully aware that this program is not supposed to be posted
 * to a public server, such as a public GitHub repository or a public web page.
 */

struct Node {
	int addr;
	Node *prev, *next;
};

int main(int argc, char** argv) {
	int F, f, H, M, addr;
	timeval beg, end;
	FILE *trace;
	double sec;

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " trace.txt\n";
		return -1;
	}


	/* LFU */
	puts("LFU policy:");
	gettimeofday(&beg, 0);
	puts("Frame\tHit\t\tMiss\t\tPage fault ratio");

	for (F=64; F<=512; F<<=1) {
		vector<pair<int, int>> LFU[SZ]; // LFU[hash][] = (addr, cache)
		int FREQ[512] = {0}; // cache -> freq
		int C2A[F]; // cache -> addr
		trace = fopen(argv[1], "r");
		H = 0; M = 0; f = 0;

		while (fscanf(trace, "%d", &addr) == 1) {
			/* Check if addr in cache */
			bool flag = false;
			for (auto &p : LFU[hash(addr)])
				if (p.F == addr) {
					if (argc > 2)
						printf("IN-CACHE\taddr=%d,\tfreq=%d\n", addr, FREQ[p.S]);
					FREQ[p.S]++;
					H++;
					flag = true;
					break;
				}
			if (flag) continue;

			M++;

			/* Add to cache if there are free slot */
			if (f < F) {
				LFU[hash(addr)].push_back({addr, f});
				FREQ[f] = 1;
				C2A[f++] = addr;

				if (argc > 2)
					printf("ADD-CACHE\taddr=%d\n", addr);
				continue;
			}

			/* Remove LFU and replace with addr */
			int mn = 0;
			for (int i=1; i<F; i++)
				if (FREQ[mn] > FREQ[i] || (FREQ[mn] == FREQ[i] && C2A[mn] > C2A[i]))
					mn = i;

			RM(LFU[hash(C2A[mn])], make_pair(C2A[mn], mn));

			if (argc > 2) printf("RM-CACHE\taddr=%d,\tfreq=%d->%d\n", addr, C2A[mn], FREQ[mn]);

			LFU[hash(addr)].push_back({addr, mn});
			FREQ[mn] = 1;
			C2A[mn] = addr;
		}

		fclose(trace);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, M*1./(H+M));
	}
	gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	if (argc == 2)
	printf("Total elapsed tme %.4f sec\n\n", sec);


	/* LRU */
	puts("LRU policy:");
	gettimeofday(&beg, 0);
	puts("Frame\tHit\t\tMiss\t\tPage fault ratio");

	for (F=64; F<=512; F<<=1) {
		vector<pair<int, Node*>> LRU[SZ]; // LRU[hash][] = (addr, node)
		Node *HEAD = nullptr, *TAIL = nullptr;
		trace = fopen(argv[1], "r");
		H = 0; M = 0; f = 0;

		while (fscanf(trace, "%d", &addr) == 1) {
			/* Check if addr in cache */
			Node *cur = nullptr;
			for (auto &p : LRU[hash(addr)])
				if (p.F == addr)
					cur = p.S;
			if (cur != nullptr) {
				H++;
				if (HEAD == cur)
					continue;

				cur->prev->next = cur->next;

				if (TAIL == cur) {
					TAIL = TAIL->prev;
					TAIL->next = nullptr;
				} else
					cur->next->prev = cur->prev;

				cur->next = HEAD;
				cur->prev = nullptr;
				HEAD->prev = cur;
				HEAD = cur;

				continue;
			}

			M++;

			/* Remove LRU if cache is full */
			while (f >= F) {
				RM(LRU[hash(TAIL->addr)], make_pair(TAIL->addr, TAIL));

				TAIL = TAIL->prev;
				TAIL->next = nullptr;
				f--;
			}

			/* Add to cache */
			Node *n = new Node;
			n->addr = addr;
			n->prev = nullptr;
			n->next = HEAD;
			if (HEAD) HEAD->prev = n;
			HEAD = n;
			if (f == 0) TAIL = HEAD;
			LRU[hash(addr)].push_back({addr, n});
			f++;
		}

		fclose(trace);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, M*1./(H+M));
	}
	gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	if (argc == 2)
	printf("Total elapsed tme %.4f sec\n", sec);

	return 0;
}
