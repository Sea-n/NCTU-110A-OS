#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <map>

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
	Node* nxt;
};

int main(int argc, char** argv) {
	timeval beg, end;
	ifstream trace;
	float R, sec;
	int F, f, H, M, addr;

	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " trace.txt\n";
		return -1;
	}

	/* LFU */
	puts("LFU policy:");
	gettimeofday(&beg, 0);
	puts("Frame\tHit\t\tMiss\t\tPage fault ratio");
	for (F=64; F<=512; F<<=1) {
		map<int, int> FREQ;  // page addr => freq
		map<pair<int, int>, bool> LFU;  // (freq, page addr) => true
		trace.open(argv[1]);
		H = 0; M = 0; f = 0;

		while (trace >> addr) {
			/* Check if addr in cache */
			auto it = FREQ.find(addr);
			if (it != FREQ.end()) {
//				printf("IN-CACHE\taddr=%d,\tfreq=%d->%d\n", addr, it->first, it->second);
				LFU.erase({FREQ[addr], addr});
				FREQ[addr]++;
				LFU[{FREQ[addr], addr}] = 1;
				H++;
				continue;
			}

			M++;

			/* Remove LFU */
			if (f == F) {
				auto it = LFU.upper_bound({0, 0});
//				printf("ERASE\t\tfreq=%d,\taddr=%d\n", it->first.first, it->first.second);
				FREQ.erase(it->first.second);
				LFU.erase(it);
				f--;
			}

			/* Add to cache */
//			printf("ADD-CACHE\taddr=%d\n", addr);
			f++;
			FREQ[addr] = 1;
			LFU[{1, addr}] = 1;
		}

		trace.close();
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
		trace.open(argv[1]);
		H = 0; M = 0;

		while (trace >> addr) {
			H++;
		}

		trace.close();
		R = M * 1.0 / (H+M);
		printf("%d\t%d\t\t%d\t\t%.10f\n", F, H, M, R);
	}
	gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	printf("Total elapsed tme %.4f sec\n\n", sec);

	return 0;
}
