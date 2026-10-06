/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "ligmarena.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

ligmarena_t *
arena_alloc(u64 res_size, u64 cmt_size)
{
	res_size               = PAGE_ALIGN(res_size);
	cmt_size               = PAGE_ALIGN(cmt_size);
	ligmarena_t *arena_ptr = (ligmarena_t *)mmap(NULL, res_size, PROT_NONE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if(arena_ptr == MAP_FAILED) {
		perror("Failed to reserve arena virtual address range");
		exit(1);
	}
	if(mprotect(arena_ptr, cmt_size, PROT_READ | PROT_WRITE)) {
		perror("Failed to commit page");
		exit(1);
	}

	arena_ptr->res_size = res_size;
	arena_ptr->cmt_size = cmt_size;
	arena_ptr->pos      = ARENA_BASE_POS;
	arena_ptr->cmt      = cmt_size;

	return arena_ptr;
}

void
arena_release(ligmarena_t *arena)
{
	if(munmap(arena, arena->res_size)) {
		perror("Failed to release arena virtual address range");
		exit(1);
	}
}

[[gnu::always_inline]]
static inline void *
__arena_push_no_zero(ligmarena_t *arena, u64 size)
{
	u64 pos_aligned = ALIGN(arena->pos, __alignof(void *));
	u64 new_pos     = pos_aligned + size;

	if(new_pos > arena->res_size) {
		errno = ENOMEM;
		perror("Attempted commit out of reserved address range");
		exit(1);
	}

	if(new_pos > arena->cmt) {
		u8 *ptr      = (u8 *)arena + arena->cmt;

		u64 new_cmt  = ALIGN(new_pos, arena->cmt_size);
		new_cmt      = MIN(new_cmt, arena->res_size);

		u64 cmt_size = new_cmt - arena->cmt;
		if(mprotect(ptr, cmt_size, PROT_READ | PROT_WRITE)) {
			perror("Failed to commit page");
			exit(1);
		}
		arena->cmt = new_cmt;
	}

	arena->pos = new_pos;
	return (u8 *)arena + pos_aligned;
}

void *
arena_push(ligmarena_t *arena, u64 size)
{
	void *ret = __arena_push_no_zero(arena, size);
	memset(ret, 0, size);
	return ret;
}

void *
arena_push_no_zero(ligmarena_t *arena, u64 size)
{
	return __arena_push_no_zero(arena, size);
}

void
arena_pop(ligmarena_t *arena, u64 size)
{
	size        = MIN(size, arena->pos - ARENA_BASE_POS);
	arena->pos -= size;
}

void
arena_pop_to(ligmarena_t *arena, u64 pos)
{
	u64 size = pos < arena->pos ? arena->pos - pos : 0;
	arena_pop(arena, size);
}

void
arena_clear(ligmarena_t *arena)
{
	arena_pop_to(arena, ARENA_BASE_POS);
}

void
sys_mem_decommit(void *ptr, u64 size)
{
	madvise(ptr, size, MADV_DONTNEED);
	mprotect(ptr, size, PROT_NONE);
}
