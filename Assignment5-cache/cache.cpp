#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <map>
#define F first
#define S second

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

char fbuf[1002003];
int main(int argc, char** argv) {
	int F, f, H, M, addr;
	timeval beg, end;
	double R, sec;

	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " trace.txt\n";
		return -1;
	}

	FILE *trace;
	trace = fopen(argv[1], "r");

	/* LFU */
	puts("LFU policy:");
	gettimeofday(&beg, 0);
	puts("Frame\tHit\t\tMiss\t\tPage fault ratio");
	for (F=64; F<=512; F<<=1) {
		fseek(trace, 0, SEEK_SET);
		map<int, int> LFU;  // page addr => freq
		H = 0; M = 0; f = 0;

		while (fscanf(trace, "%d", &addr) == 1) {
			/* Check if addr in cache */
			auto it = LFU.find(addr);
			if (it != LFU.end()) {
				if (argc > 2)
					printf("IN-CACHE\taddr=%d,\tfreq=%d\n", addr, LFU[addr]);
				LFU[addr]++;
				H++;
				continue;
			}

			M++;

			/* Remove LFU */
			while (f >= F) {
				auto it = LFU.begin();
				auto rm = it;
				while (++it != LFU.end()) {
					if (it->S > 0 && it->S < rm->S)
						rm = it;
					else if (it->S == rm->S && it->F < rm->F)
						rm = it;
				}

				if (argc > 2)
					printf("RM-CACHE\taddr=%d,\tfreq=%d->%d\n", addr, rm->S, rm->F);
				LFU.erase(rm);
				f--;
			}

			/* Add to cache */
			if (argc > 2)
				printf("ADD-CACHE\taddr=%d\n", addr);
			f++;
			LFU[addr] = 1;
		}

		R = M * 1.0 / (H+M);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, R);
	}
	gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	printf("Total elapsed tme %.4f sec\n\n", sec);


	/* LRU */
	puts("LRU policy:");
	gettimeofday(&beg, 0);
	puts("Frame\tHit\t\tMiss\t\tPage fault ratio");

	for (F=64; F<=512; F<<=1) {
		Node *HEAD = nullptr;
		Node *TAIL = nullptr;
		map<int, Node*> LRU;
		fseek(trace, 0, SEEK_SET);
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

		R = M * 1.0 / (H+M);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, R);
	}
	gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	printf("Total elapsed tme %.4f sec\n\n", sec);

	return 0;
}
