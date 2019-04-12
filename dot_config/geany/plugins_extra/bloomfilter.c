/*
 * A Bloom Filter implementation, on top of glib. The assumed data is 0-terminated
 * strings of characters.
 *
 * Copyright (C) 2012 by Emil Brink <emil@obsession.se>.
 *
 * This file is part of Geanyuniq.
 *
 * Geanyuniq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Geanyuniq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Geanyuniq.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "murmurhash.h"
#include "bloomfilter.h"

/* -------------------------------------------------------------------------------------------------------------- */

struct BloomFilter {
	gsize		m;
	gsize		k;
	gsize		size;
	gulong		*bits;
	gsize		bits_length;
	BloomContains	contains;
	gpointer	user;
};

/* -------------------------------------------------------------------------------------------------------------- */

static gboolean cb_contains_always(gpointer user, const gchar *string, gsize length)
{
	return TRUE;
}

BloomFilter * bloom_filter_new(gsize filter_size, gsize num_hashes, BloomContains contains, gpointer user)
{
	BloomFilter	*bf;
	const gsize	bits_length = (filter_size + (CHAR_BIT * sizeof *bf->bits) - 1) / (CHAR_BIT * sizeof *bf->bits);
	const gsize	bits_size = bits_length * sizeof *bf->bits;

	if((bf = g_malloc(sizeof *bf + bits_size)) != NULL)
	{
		bf->m = filter_size;
		bf->k = num_hashes;
		bf->size = 0;
		bf->bits = (gulong *) (bf + 1);
		bf->bits_length = bits_length;
		bf->contains = contains != NULL ? contains : cb_contains_always;
		bf->user = user;
		memset(bf->bits, 0, bits_size);
	}
	return bf;
}

BloomFilter * bloom_filter_new_with_probability(gfloat prob, gsize num_elements, BloomContains contains, gpointer user)
{
	const gfloat	m = -(num_elements * logf(prob)) / powf(log(2.f), 2.f);
	const gfloat	k = logf(2.f) * m / num_elements;

#if defined BLOOM_FILTER_STANDALONE
	printf("computed bloom filter size %f -> %u bytes\n", m, (guint) (m + .5f) / 8);
	printf(" so m/n = %.1f\n", m / num_elements);
	printf(" which gives k=%f\n", k);
#endif
	return bloom_filter_new((gsize) (m + .5f), (guint) (k + 0.5f), contains, user);
}

void bloom_filter_destroy(BloomFilter *bf)
{
	g_free(bf);
}

gsize bloom_filter_num_bits(const BloomFilter *bf)
{
	return bf->m;
}

gsize bloom_filter_num_hashes(const BloomFilter *bf)
{
	return bf->k;
}

gsize bloom_filter_size(const BloomFilter *bf)
{
	return bf->size;
}

void bloom_filter_insert(BloomFilter *bf, const gchar *string, gssize string_length)
{
	const gsize	len = string_length > 0 ? string_length : strlen(string);
	gsize		i;

	/* Repeatedly hash the string, and set bits in the Bloom filter's bit array. */
	for(i = 0; i < bf->k; i++)
	{
		const guint32	hash = MurmurHash2(string, len, i);
		const gsize	pos = hash % bf->m;
		const gsize	slot = pos / (CHAR_BIT * sizeof *bf->bits);
		const gsize	bit = pos % (CHAR_BIT * sizeof *bf->bits);
#if defined BLOOM_FILTER_STANDALONE
		printf("hash(%s,%zu)=%u -> pos=%zu -> slot=%zu, bit=%zu\n", string, i, hash, pos, slot, bit);
#endif
		bf->bits[slot] |= 1UL << bit;
	}
	bf->size++;
}

gboolean bloom_filter_contains(const BloomFilter *bf, const gchar *string, gssize string_length)
{
	const gsize	len = string_length > 0 ? string_length : strlen(string);
	gsize		i;

	/* Check the Bloom filter, by hashing and checking bits. */
	for(i = 0; i < bf->k; i++)
	{
		const guint32	hash = MurmurHash2(string, len, i);
		const gsize	pos = hash % bf->m;
		const gsize	slot = pos / (CHAR_BIT * sizeof *bf->bits);
		const gsize	bit = pos % (CHAR_BIT * sizeof *bf->bits);

		/* If a bit is not set, the element is not contained, for sure. */
		if((bf->bits[slot] & (1UL << bit)) == 0)
			return FALSE;
	}
	/* Bit-checking says yes, call user's contains() function to make sure. */
	return bf->contains(bf->user, string, len);
}

/* -------------------------------------------------------------------------------------------------------------- */

#if defined BLOOM_FILTER_STANDALONE

int main(void)
{
	BloomFilter	*bf;

	bf = bloom_filter_new_with_probability(1e-6, 98570, NULL, NULL);
	bloom_filter_insert(bf, "foo", -1);
	printf("contains(foo)=%d\n", bloom_filter_contains(bf, "foo", -1));
	printf("contains(foz)=%d\n", bloom_filter_contains(bf, "foz", -1));
	bloom_filter_destroy(bf);
}

#endif	/* BLOOM_FILTER_STANDALONE */
