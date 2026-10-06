/* SPDX-License-Identifier: LGPL-2.1-or-later
 * Copyright (c) 2026 Ian Leng <ligmaleng@proton.me>
 */

#ifndef LIGMARENA_H
#define LIGMARENA_H

#ifndef __x86_64__
# error "Unsupported host architecture"
#endif

#ifndef __linux__
# error "Unsupported host platform"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __ASSEMBLY__
# define _AC(X, Y) X
# define _AT(T, X) X
#else
# define __AC(X, Y) (X##Y)
# define _AC(X, Y) __AC(X, Y)
# define _AT(T, X) ((T)(X))
#endif

typedef signed char      i8;
typedef signed short     i16;
typedef signed int       i32;
typedef signed long      i64;

typedef unsigned char    u8;
typedef unsigned short   u16;
typedef unsigned int     u32;
typedef unsigned long    u64;

typedef signed char      b8;
typedef signed int       b32;

typedef struct ligmarena ligmarena_t;
struct ligmarena
{
	u64          res_size;
	u64          cmt_size;
	u64          flags;
	u64          pos;
	u64          cmt;
	u64          res;
	ligmarena_t *prev;
	ligmarena_t *free_last;
};

#define KiB(n) (_AC(n, UL) << 10)
#define MiB(n) (_AC(n, UL) << 20)
#define GiB(n) (_AC(n, UL) << 30)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)

#define ARENA_BASE_POS (sizeof(ligmarena_t))
#define PUSH_STRUCT(A, T) (T *)arena_push((A, sizeof(T))
#define PUSH_ARRAY(A, T, C) (T *)arena_push((A, sizeof(T) * (C))
#define PUSH_STRUCT_NZ(A, T) (T *)arena_push_no_zero((A), sizeof(type))
#define PUSH_ARRAY_NZ(A, T, C) (T *)arena_push_no_zero((A), sizeof(type) * (C))

/*
 * calling noreturn functions, __builtin_unreachable() and __builtin_trap()
 * confuse the stack allocation in gcc, leading to overly large stack
 * frames, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82365
 *
 * Adding an empty inline assembly before it works around the problem
 */
#define barrier_before_unreachable() asm volatile("")

/* nofmt */
#define UNREACHABLE() ({asm volatile(""); __builtin_unreachable();})

ligmarena_t *arena_alloc(u64 res_size, u64 cmt_size);
void         arena_release(ligmarena_t *arena);
void        *arena_push_no_zero(ligmarena_t *arena, u64 size);
void        *arena_push(ligmarena_t *arena, u64 size);
void         arena_pop(ligmarena_t *arena, u64 size);
void         arena_pop_to(ligmarena_t *arena, u64 pos);
void         arena_clear(ligmarena_t *arena);
void         sys_mem_decommit(void *ptr, u64 size);

#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* LIGMARENA_H */
