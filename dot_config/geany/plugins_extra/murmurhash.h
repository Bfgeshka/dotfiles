/*
 * Minimalistic header for the MurmurHash2 function. This is in the public domain.
*/

#include <glib.h>

extern guint32	MurmurHash2(gconstpointer key, gsize len, guint32 seed);
