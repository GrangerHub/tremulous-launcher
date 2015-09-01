/* sexp2rsa.c

   Copyright (C) 2002 Niels Möller

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>

#include "rsa.h"

#include "bignum.h"
#include "sexp.h"

#define GET(x, l, v)				\
do {						\
  if (!nettle_mpz_set_sexp((x), (l), (v))	\
      || mpz_sgn(x) <= 0)			\
    return 0;					\
} while(0)

/* Iterator should point past the algorithm tag, e.g.
 *
 *   (public-key (rsa (n |xxxx|) (e |xxxx|))
 *                    ^ here
 */

int
rsa_keypair_from_sexp_alist(struct rsa_public_key *pub,
			    struct rsa_private_key *priv,
			    unsigned limit,
			    struct sexp_iterator *i)
{
  static const uint8_t * const names[2]
    = { (uint8_t *)"n", (uint8_t *)"e" };
  struct sexp_iterator values[2];
  
  if (!sexp_iterator_assoc(i, 2, names, values))
    return 0;

  if (pub)
    {
      GET(pub->n, limit, &values[0]);
      GET(pub->e, limit, &values[1]);

      if (!rsa_public_key_prepare(pub))
	return 0;
    }
  
  return 1;
}

int
rsa_keypair_from_sexp(struct rsa_public_key *pub,
		      struct rsa_private_key *priv,
		      unsigned limit, 
		      size_t length, const uint8_t *expr)
{
  struct sexp_iterator i;
  static const uint8_t * const types[2]
    = { (uint8_t *)"private-key", (uint8_t *)"public-key" };
  static const uint8_t * const names[3]
    = { (uint8_t *)"rsa", (uint8_t *)"rsa-pkcs1", (uint8_t *)"rsa-pkcs1-sha1" };

  if (!sexp_iterator_first(&i, length, expr))
    return 0;

  if (!sexp_iterator_check_types(&i, 2, types))
    return 0;

  if (!sexp_iterator_check_types(&i, 3, names))
    return 0;

  return rsa_keypair_from_sexp_alist(pub, priv, limit, &i);
}
