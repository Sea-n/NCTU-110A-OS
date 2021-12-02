#include <sys/time.h>
#include <algorithm>
#include <iostream>
#include <vector>
#define SZ 1069
#define F first
#define S second
#define ALL(v) v.begin(), v.end()
#define RM(vec, val) vec.erase(remove(ALL(vec), val))
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

struct LFU_Node {
	int freq, seq, addr;
};

struct LRU_Node {
	int addr;
	LRU_Node *prev, *next;
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
		vector<pair<int, int>> LUT[SZ];  // LUT[hash][] = (addr, pos)
		LFU_Node LFU[512];  // LFU[pos] -> (freq, seq, addr)
		trace = fopen(argv[1], "r");
		H = 0; M = 0; f = 0;
		int seq = 0;

		while (fscanf(trace, "%d", &addr) == 1 && ++seq) {
			/* Check if page in memory */
			bool flag = false;
			for (auto &p : LUT[hash(addr)])
				if (p.F == addr) {
					LFU[p.S].freq++;
					LFU[p.S].seq = seq;
					H++;
					flag = true;
					break;
				}
			if (flag) continue;

			M++;

			/* Add to memory if there are free slot */
			if (f < F) {
				LUT[hash(addr)].push_back({addr, f});
				LFU[f++] = {1, seq, addr};
				continue;
			}

			/* Remove LFU and replace with addr */
			int mn = 0;
			for (int i=1; i<F; i++)
				if (LFU[mn].freq > LFU[i].freq || (LFU[mn].freq == LFU[i].freq && LFU[mn].seq > LFU[i].seq))
					mn = i;

			RM(LUT[hash(LFU[mn].addr)], make_pair(LFU[mn].addr, mn));

			LUT[hash(addr)].push_back({addr, mn});
			LFU[mn] = {1, seq, addr};
		}

		fclose(trace);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, M*1./(H+M));
	}
	gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	printf("Total elapsed tme %.4f sec\n\n", sec);


	/* LRU */
	puts("LRU policy:");
	gettimeofday(&beg, 0);
	puts("Frame\tHit\t\tMiss\t\tPage fault ratio");

	for (F=64; F<=512; F<<=1) {
		vector<pair<int, LRU_Node*>> LRU[SZ]; // LRU[hash][] = (addr, node)
		LRU_Node *HEAD = nullptr, *TAIL = nullptr, *cur;
		trace = fopen(argv[1], "r");
		H = 0; M = 0; f = 0;

		while (fscanf(trace, "%d", &addr) == 1) {
			/* Check if page in memory */
			cur = nullptr;
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

			/* Remove LRU if memory is full */
			while (f >= F) {
				RM(LRU[hash(TAIL->addr)], make_pair(TAIL->addr, TAIL));

				TAIL = TAIL->prev;
				TAIL->next = nullptr;
				f--;
			}

			/* Add page to memory */
			cur = new LRU_Node;
			cur->addr = addr;
			cur->prev = nullptr;
			cur->next = HEAD;
			if (HEAD) HEAD->prev = cur;
			HEAD = cur;
			if (f == 0) TAIL = HEAD;
			LRU[hash(addr)].push_back({addr, cur});
			f++;
		}

		fclose(trace);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, M*1./(H+M));
	}
	gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	printf("Total elapsed tme %.4f sec\n", sec);

	return 0;
}
