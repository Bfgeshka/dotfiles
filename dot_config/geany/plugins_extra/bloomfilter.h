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

#if !defined BLOOMFILTER_H
#define	BLOOMFILTER_H

#include <glib.h>

typedef struct BloomFilter BloomFilter;

/* A callback function for exactly determining if the string is contained.
 * For a "pure" probabilistic Bloom filter, use NULL. For our application,
 * we must be able to determine exact containment.
*/
typedef gboolean	(*BloomContains)(gpointer user, const gchar *string, gsize string_length);

extern BloomFilter *	bloom_filter_new(gsize filter_size, gsize num_hashes, BloomContains contains, gpointer user);
extern BloomFilter *	bloom_filter_new_with_probability(gfloat prob, gsize num_elements, BloomContains contains, gpointer user);
extern gsize		bloom_filter_num_bits(const BloomFilter *bf);
extern gsize		bloom_filter_num_hashes(const BloomFilter *bf);
extern gsize		bloom_filter_size(const BloomFilter *bf);
extern void		bloom_filter_insert(BloomFilter *bf, const gchar *string, gssize string_length);
extern gboolean		bloom_filter_contains(const BloomFilter *bf, const gchar *string, gssize string_length);
extern void		bloom_filter_destroy(BloomFilter *bf);

#endif	/* BLOOMFILTER_H */
