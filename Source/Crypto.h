/*
  ==============================================================================

    Crypto.h
    Created: 18 Jul 2015 10:44:39am
    Author:  jkent

  ==============================================================================
*/

#ifndef CRYPTO_H_INCLUDED
#define CRYPTO_H_INCLUDED

#include "PublicKey.h"
#include "nettle/sha2.h"

class Crypto
{
public:
    static void init();

    static bool hashFile(const File &file, uint8_t *digest);
    static bool verify(const File &file, const uint8_t *digest = nullptr);

private:
    static const char KEY_REVOCATION_CHECK_URL[];

    static OwnedArray<PublicKey> publicKeys;

    static void loadKeys();
    static void checkKeys();
};

#endif  // CRYPTO_H_INCLUDED
