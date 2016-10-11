/*
 * drivers/char/random.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdint.h>
#include <string.h>

#include <kernel/fs/vnode.h>
#include "kernel.h"

typedef unsigned long long u64;

/* xorshift1024* generator, http://vigna.di.unimi.it/ftp/papers/xorshift.pdf */
static u64 next(void)
{
	static int p;
	static u64 s[16] = {
		0x84242f96eca9c41dull, 0xa3c65b8776f96855ull, 0x5b34a39f070b5837ull,
		0x4489affce4f31a1eull, 0x2ffeeb0a48316f40ull, 0xdc2d9891fe68c022ull,
		0x3659132bb12fea70ull, 0xaac17d8efa43cab8ull, 0xc4cb815590989b13ull,
		0x5ee975283d71c93bull, 0x691548c86c1bd540ull, 0x7910c41d10a1e6a5ull,
		0x0b5fc64563b3e2a8ull, 0x047f7684e9fc949dull, 0xb99181f2d8f685caull,
		0x284600e3f30e38c3ull
	};
	const u64 s0 = s[p];
	u64 s1 = s[p = (p + 1) & 15];

	s1 ^= s1 << 31; // a
	s[p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // b,c

	return s[p] * UINT64_C(1181783497276652981);
}

int open_random(struct vnode *vp, int flags)
{
	(void)vp;
	(void)flags;

	return 0;
}

int read_random(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;
	(void)off;

	static u64 m;
	static int remaining_bytes = 0;

	if ((int)count <= remaining_bytes) {
		memcpy(buf, (char *)&m + 8 - remaining_bytes, count);
		remaining_bytes -= count;
	} else {
		for (int i = count; i > 0; i -= 8, buf = (char *)buf + 8) {
			m = next();
			memcpy(buf, &m, min(i, 8));
			remaining_bytes = 8 - min(i, 8);
		}
	}
	*n = count;

	return 0;
}

const struct vnodeops random_vops = {
	.vop_open = open_random,
	.vop_read = read_random,
};
