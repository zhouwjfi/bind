/*
 * Portions Copyright (c) 1995-1998 by Trusted Information Systems, Inc.
 *
 * Permission to use, copy modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND TRUSTED INFORMATION SYSTEMS
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL
 * TRUSTED INFORMATION SYSTEMS BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 */

#ifndef DST_DST_INTERNAL_H
#define DST_DST_INTERNAL_H 1

#include <isc/lang.h>
#include <isc/buffer.h>
#include <isc/int.h>
#include <isc/region.h>

#include "../rename.h"

#include <dst/dst.h>

ISC_LANG_BEGINDECLS

/* 
 * define what crypto systems are supported.
 * BSAFE, DNSSAFE for RSA
 * OPENSSL for DSA
 * Only one package per algorithm can be defined.
 */
#if defined(BSAFE) && defined(DNSSAFE)
# error "Cannot have both BSAFE and DNSSAFE defined"
#endif

/***
 *** Types
 ***/

typedef struct dst_func dst_func;

struct dst_key {
	unsigned int	magic;
	dns_name_t *	key_name;	/* name of the key */
	unsigned int	key_size;	/* size of the key in bits */
	unsigned int	key_proto;	/* protocols this key is used for */
	unsigned int	key_alg;	/* algorithm of the key */
	isc_uint32_t	key_flags;	/* flags of the public key */
	isc_uint16_t	key_id;		/* identifier of the key */
	isc_mem_t	*mctx;		/* memory context */
	void *		opaque;		/* pointer to key in crypto pkg fmt */
	dst_func *	func;		/* crypto package specific functions */
};

struct dst_context {
	unsigned int magic;
	dst_key_t *key;
	isc_mem_t *mctx;
	void *opaque;
};

struct dst_func {
	/*
	 * Context functions
	 */
	isc_result_t (*createctx)(dst_key_t *key, dst_context_t *dctx);
	void (*destroyctx)(dst_context_t *dctx);
	isc_result_t (*adddata)(dst_context_t *dctx, const isc_region_t *data);

	/*
	 * Key operations
	 */
	isc_result_t (*sign)(dst_context_t *dctx, isc_buffer_t *sig);
	isc_result_t (*verify)(dst_context_t *dctx, const isc_region_t *sig);
	isc_result_t (*digest)(dst_context_t *dctx, isc_buffer_t *digest);
	isc_result_t (*computesecret)(const dst_key_t *pub,
				      const dst_key_t *priv,
				      isc_buffer_t *secret);
	isc_boolean_t (*compare)(const dst_key_t *key1, const dst_key_t *key2);
	isc_boolean_t (*paramcompare)(const dst_key_t *key1,
				      const dst_key_t *key2);
	isc_result_t (*generate)(dst_key_t *key, int parms);
	isc_boolean_t (*isprivate)(const dst_key_t *key);
	void (*destroy)(dst_key_t *key);

	/* conversion functions */
	isc_result_t (*todns)(const dst_key_t *key, isc_buffer_t *data);
	isc_result_t (*fromdns)(dst_key_t *key, isc_buffer_t *data);
	isc_result_t (*tofile)(const dst_key_t *key);
	isc_result_t (*fromfile)(dst_key_t *key, const isc_uint16_t id);
};

#ifndef DST_HASH_SIZE
#define DST_HASH_SIZE 20	/* RIPEMD160 & SHA-1 are 20 bytes, MD5 is 16 */
#endif

void
dst_s_hmacmd5_init(struct dst_func **funcp);
void
dst_s_dnssafersa_init(struct dst_func **funcp);
void
dst_s_openssldsa_init(struct dst_func **funcp);
void
dst_s_openssldh_init(struct dst_func **funcp);
void
dst_s_opensslmd5_init(struct dst_func **funcp);

/*
 * Support functions.
 */
int
dst_s_calculate_bits(const unsigned char *str, const int max_bits); 

isc_uint16_t
dst_s_id_calc(const unsigned char *key, const int keysize);

/*
 * Digest functions.
 */
isc_result_t
dst_s_md5(const unsigned int mode, void **context, isc_region_t *data,
	  isc_buffer_t *digest, isc_mem_t *mctx);

/*
 * Memory allocators using the DST memory pool.
 */
void *
dst_mem_alloc(size_t size);
void
dst_mem_free(void *ptr);
void *
dst_mem_realloc(void *ptr, size_t size);

ISC_LANG_ENDDECLS

#endif /* DST_DST_INTERNAL_H */
