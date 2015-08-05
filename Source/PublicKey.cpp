/*
  ==============================================================================

    PublicKey.cpp
    Created: 18 Jul 2015 6:51:23pm
    Author:  jkent

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "nettle/bignum.h"
#include "nettle/rsa.h"
#include "nettle/sexp.h"
#include "nettle/sha2.h"
#include "PublicKey.h"

PublicKey::PublicKey()
:loaded(false), revoked(REVOKED_UNKNOWN)
{
    rsa_public_key_init(&publicKey);
}

PublicKey::PublicKey(const PublicKey &other)
:loaded(other.loaded), revoked(other.revoked), keyFile(other.keyFile)
{
    mpz_init_set(publicKey.n, other.publicKey.n);
    mpz_init_set(publicKey.e, other.publicKey.e);
    publicKey.size = other.publicKey.size;
}

PublicKey::PublicKey(const char* buf, int length)
    :loaded(false), revoked(REVOKED_UNKNOWN)
{
    rsa_public_key_init(&publicKey);

    loadSexp(buf, length);
}

PublicKey::PublicKey(const File &file)
:loaded(false), revoked(REVOKED_UNKNOWN), keyFile(file)
{
    rsa_public_key_init(&publicKey);

    loadSexp(file);
}

PublicKey::~PublicKey()
{
    rsa_public_key_clear(&publicKey);
}

bool PublicKey::isLoaded()
{
    return loaded;
}

bool PublicKey::isValid()
{
    return revoked == REVOKED_NO;
}

bool PublicKey::isRevoked()
{
    return revoked == REVOKED_YES;
}

const String & PublicKey::getFingerprint()
{
    if (!loaded || fingerprint.isNotEmpty()) {
        return fingerprint;
    }
    
    MemoryBlock buffer;
    buffer.setSize(nettle_mpz_sizeinbase_256_u(publicKey.n));
    nettle_mpz_get_str_256(buffer.getSize(), (uint8_t *)buffer.getData(), publicKey.n);
    
    struct sha256_ctx hash;
    sha256_init(&hash);
    sha256_update(&hash, buffer.getSize(), (uint8_t *)buffer.getData());

    buffer.ensureSize(SHA256_DIGEST_SIZE);
    sha256_digest(&hash, SHA256_DIGEST_SIZE, (uint8_t *)buffer.getData());

    fingerprint = String::toHexString(buffer.getData(), SHA256_DIGEST_SIZE, 0);
    
    return fingerprint;
}

bool PublicKey::loadSexp(const char* buf, int length)
{
    if (!rsa_keypair_from_sexp(&publicKey, NULL, 0, length, (const uint8_t *)buf)) {
        return false;
    }

    loaded = true;
    return true;
}

bool PublicKey::loadSexp(const File &file)
{
    MemoryBlock buffer;
    
    if (!file.loadFileAsData(buffer)) {
        return false;
    }

    keyFile = file;

    if (!loadSexp((const char *)buffer.getData(), buffer.getSize())) {
        return false;
    }

    return true;
}