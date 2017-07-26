/*-
 * Copyright (c) 2017 The FreeBSD Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

uintptr_t lib1_data_addr(void);
uintptr_t lib2_data_addr(void);

static int main_data;

#ifdef __LP64__
#define HEX_FMT "0x%016llx"
#define HDR_FMT "%-18s"
#else
#define HEX_FMT "0x%08llx"
#define HDR_FMT "%-10s"
#endif

#ifndef MAP_32BIT
#define MAP_32BIT 0
#endif

// Return an address of something on the main stack.
uintptr_t
stack_addr(void)
{
	int s;
	uintptr_t addr = (uintptr_t)&s;

	return addr;
}

void
header(void)
{
	printf(HDR_FMT "  " HDR_FMT "  " HDR_FMT "  " HDR_FMT "  " HDR_FMT "  "
	    HDR_FMT "  " HDR_FMT "  " HDR_FMT "  " HDR_FMT "\n",
	    "exec,",
	    "malloc4k,",
	    "malloc4m,",
	    "mmap4k,",
	    "mmap4k32,",
	    "mmap4m,",
	    "so1,",
	    "so2,",
	    "stack");
}

void
print_addrs(void)
{
	void *malloc4k = malloc(4096);
	void *malloc4m = malloc(4 * 1024 * 1024);
	void *mmap4k = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	    MAP_PRIVATE | MAP_ANON, -1, 0);
	void *mmap4k32 = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	    MAP_PRIVATE | MAP_ANON | MAP_32BIT, -1, 0);
	void *mmap4m = mmap(NULL, 4 * 1024 * 1024, PROT_READ | PROT_WRITE,
	    MAP_ANON, -1, 0);

	printf(HEX_FMT ", " HEX_FMT ", " HEX_FMT ", " HEX_FMT ", " HEX_FMT ", "
	    HEX_FMT ", " HEX_FMT ", " HEX_FMT ", " HEX_FMT "\n",
	    (unsigned long long)&main_data,
	    (unsigned long long)malloc4k,
	    (unsigned long long)malloc4m,
	    (unsigned long long)mmap4k,
	    (unsigned long long)mmap4k32,
	    (unsigned long long)mmap4m,
	    (unsigned long long)lib1_data_addr(),
	    (unsigned long long)lib2_data_addr(),
	    (unsigned long long)stack_addr());
}

void
forkloop(size_t count, const char *argv0)
{
	const char *args[] = {NULL, NULL};
	const char *env[] = {NULL};
	int status;

	args[0] = argv0;
	for (size_t i = 0; i < count; i++) {
		switch (fork()) {
		case -1:
			err(1, "fork");
			break;
		case 0:
			if (execve(argv0, (char * const*)args, (char * const*)env) != 0)
				err(1, "execve");
			break;
		default:
			wait(&status);
			break;
		}
	}
}

int
main(int argc, char *argv[])
{
	size_t count;

	count = 1;
	if (argc == 1) {
		print_addrs();
		exit(0);
	}

	header();
	forkloop(atoi(argv[1]), argv[0]);
}
