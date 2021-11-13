#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#define POOL_SIZE 20000

/*
 * Student ID: 0816146
 * Student Name: Sean Wei
 * Email: nctu-os@sean.taipei
 * SE tag: xnxcxtxuxoxsx
 * Statement: I am fully aware that this program is not supposed to be posted
 * to a public server, such as a public GitHub repository or a public web page.
 */

struct block {
	size_t size;
	int free;
	struct block *prev;
	struct block *next;
};

void *mem = 0;
struct block *head;

void *malloc(size_t size) {
	struct block *cur, *rem, *cand;
	size_t sz;

	/* Special case: output result */
	if (size == 0) {
		sz = 0;
		for (cur = head; cur; cur = cur->next)
			if (sz < cur->size && cur->free)
				sz = cur->size;

		char out[] = "Max Free Chunk Size = 000000\n";
		out[22] |= (sz / 100000) % 10;
		out[23] |= (sz / 10000) % 10;
		out[24] |= (sz / 1000) % 10;
		out[25] |= (sz / 100) % 10;
		out[26] |= (sz / 10) % 10;
		out[27] |= (sz / 1) % 10;
		write(1, out, 29);

		munmap(mem, POOL_SIZE);
		return 0;
	}

	size = (size+31) >> 5 << 5;

	/* Initial */
	if (mem == 0) {
		mem = mmap(0, POOL_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		head = mem;
		head->size = POOL_SIZE - 32;
		head->free = 1;
		head->prev = 0;
		head->next = 0;
	}

	/* Find available chunk */
	for (cur = head, cand = 0, sz = 1e9; cur; cur = cur->next) {
		if (cur->size < size || !cur->free)
			continue;

		if (cur->size == size) {
			cur->free = 0;
			return cur + 1;
		}

		// if (cur->size > size)
		if (sz > cur->size) {
			sz = cur->size;
			cand = cur;
		}
	}

	if ((cur = cand)) {
		rem = cur + 1 + (size>>5);
		rem->free = 1;
		rem->size = cur->size - 32 - size;
		rem->free = 1;
		rem->prev = cur;
		rem->next = cur->next;

		if (cur->next)
			cur->next->prev = rem;
		cur->next = rem;

		cur->size = size;
		cur->free = 0;
		return cur + 1;
	}

	// failed
	return 0;
}

void free(void *ptr) {
	struct block *cur;
	cur = ptr;
	--cur;
	cur->free = 1;

	if (cur->next && cur->next->free) {
		cur->size += 32 + cur->next->size;
		if (cur->next->next) {
			cur->next->next->prev = cur;
			cur->next = cur->next->next;
		} else
			cur->next = 0;
	}

	if (cur->prev && cur->prev->free) {
		cur->prev->size += 32 + cur->size;
		cur->prev->next = cur->next;
		if (cur->next)
			cur->next->prev = cur->prev;
	}
}
