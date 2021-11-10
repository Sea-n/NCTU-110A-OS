#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

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
	struct block *cur, *rem;
	cur = head;

	if (size == 0) {
		size_t sz = 0;
		while (cur) {
			if (sz < cur->size)
				sz = cur->size;
			cur = cur->next;
		}

		char out[] = "Max Free Chunk Size = 00000\n";
		out[22] |= (sz / 10000) % 10;
		out[23] |= (sz / 1000) % 10;
		out[24] |= (sz / 100) % 10;
		out[25] |= (sz / 10) % 10;
		out[26] |= (sz / 1) % 10;
		write(1, out, 28);

		munmap(mem, 20000);
		return 0;
	}

	size = (size+31) >> 5 << 5;

	if (mem == 0) {
		mem = mmap(0, 20000, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		head = mem;
		head->size = 20000 - 32;
		head->free = 1;
		head->prev = 0;
		head->next = 0;
	}

	for (;;) {
		if (cur->size < size)
			continue;

		if (cur->size == size) {
			cur->free = 0;
			return cur + 32;
		}

		// if (cur->size > size)
		rem = cur + 32 + size;
		rem->size = cur->size - 32 - size;
		rem->free = 1;
		rem->prev = cur;
		rem->next = cur->next;

		cur->next->prev = rem;
		cur->next = rem;


		cur->free = 0;
		return cur + 32;
	}

	// failed
	return 0;
}

void free(void *ptr) {
	struct block *cur;
	cur = ptr - 32;
	cur->free = 1;

	if (cur->next && cur->next->free) {
		cur->size += 32 + cur->next->size;
		cur->next->next->prev = cur;
		cur->next = cur->next->next;
	}

	if (cur->prev && cur->prev->free) {
		cur->prev->size += 32 + cur->size;
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;
	}
}
