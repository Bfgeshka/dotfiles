/*
 * MurmurHash2, by Austin Appleby
 * 
 * Note - This code makes a few assumptions about how your machine behaves -
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 * machines.
 *
 * Adapted to glib data types by Emil Brink. Mr Appleby's code is in the public
 * domain, so it stands to reason that this adaption is, too. Note that the
 * rest of Geanyuniq (which this file is part of) is licensed according to the
 * GNU General Public License, version 3 or later.
*/

#include <glib.h>

guint32 MurmurHash2(gconstpointer key, gsize len, guint32 seed)
{
	/* 'm' and 'r' are mixing constants generated offline.
	 * They're not really 'magic', they just happen to work well.
	*/
	const guint32 m = 0x5bd1e995;
	const gint r = 24;

	/* Initialize the hash to a 'random' value. */
	guint32 h = seed ^ len;

	/* Mix 4 bytes at a time into the hash. */
	const guchar * data = key;
	while(len >= 4)
	{
		guint32 k = *(guint32 *)data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	/* Handle the last few bytes of the input array. */
	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	};

	/* Do a few final mixes of the hash to ensure the last few
	 * bytes are well-incorporated.
	*/
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
