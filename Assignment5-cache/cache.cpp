#include <sys/time.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#define F first
#define S second
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
	Node* prev;
	Node* next;
};

int main(int argc, char** argv) {
	int F, f, H, M, addr;
	timeval beg, end;
	double R, sec;

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " trace.txt\n";
		return -1;
	}

	FILE *trace;

	/* LFU */
	puts("LFU policy:");
	gettimeofday(&beg, 0);
	puts("Frame\tHit\t\tMiss\t\tPage fault ratio");
	for (F=64; F<=512; F<<=1) {
		trace = fopen(argv[1], "r");
		vector<pair<int, int>> LFU[SZ]; // LFU[hash][] = (addr, cache)
		int FREQ[512] = {0}; // cache -> freq
		int C2A[F]; // cache -> addr
		H = 0; M = 0; f = 0;

		while (fscanf(trace, "%d", &addr) == 1) {
			/* Check if addr in cache */
			bool flag = false;
			for (auto &p : LFU[hash(addr)]) {
				if (p.F == addr) {
					if (argc > 2)
						printf("IN-CACHE\taddr=%d,\tfreq=%d\n", addr, FREQ[p.S]);
					FREQ[p.S]++;
					H++;
					flag = true;
					break;
				}
			}
			if (flag)
				continue;

			M++;

			/* Add to cache */
			if (f < F) {
				LFU[hash(addr)].push_back({addr, f});
				FREQ[f] = 1;
				C2A[f++] = addr;

				if (argc > 2)
					printf("ADD-CACHE\taddr=%d\n", addr);
				continue;
			}

			/* Remove LFU */
			int mn = 0;
			for (int i=1; i<F; i++) {
				if (FREQ[mn] > FREQ[i])
					mn = i;
				else if (FREQ[mn] == FREQ[i])
					if (C2A[mn] > C2A[i])
						mn = i;
			}

			auto v = &LFU[hash(C2A[mn])];
			v->erase(remove(v->begin(), v->end(), make_pair(C2A[mn], mn)));

			if (argc > 2)
				printf("RM-CACHE\taddr=%d,\tfreq=%d->%d\n", addr, C2A[mn], FREQ[mn]);

			LFU[hash(addr)].push_back({addr, mn});
			FREQ[mn] = 1;
			C2A[mn] = addr;
		}

		fclose(trace);
		R = M * 1.0 / (H+M);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, R);
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
		Node *HEAD = nullptr;
		Node *TAIL = nullptr;
		map<int, Node*> LRU;
		trace = fopen(argv[1], "r");
		H = 0; M = 0; f = 0;

		while (fscanf(trace, "%d", &addr) == 1) {
			auto it = LRU.find(addr);
			if (it != LRU.end()) {
				H++;
				if (HEAD == it->S)
					continue;

				it->S->prev->next = it->S->next;

				if (TAIL == it->S) {
					TAIL = TAIL->prev;
					TAIL->next = nullptr;
				} else
					it->S->next->prev = it->S->prev;

				it->S->next = HEAD;
				it->S->prev = nullptr;
				HEAD->prev = it->S;
				HEAD = it->S;

				continue;
			}

			M++;

			while (f >= F) {
				LRU.erase(TAIL->addr);
				TAIL = TAIL->prev;
				TAIL->next = nullptr;
				f--;
			}

			Node *n = new Node;
			n->addr = addr;
			n->prev = nullptr;
			n->next = HEAD;
			if (HEAD)
				HEAD->prev = n;
			HEAD = n;
			LRU[addr] = n;
			f++;

			if (f == 1)
				TAIL = HEAD;
		}

		fclose(trace);
		R = M * 1.0 / (H+M);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, R);
	}
	gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	if (argc == 2)
	printf("Total elapsed tme %.4f sec\n", sec);

	return 0;
}
