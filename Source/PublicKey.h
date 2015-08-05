/*
  ==============================================================================

    PublicKey.h
    Created: 18 Jul 2015 6:51:23pm
    Author:  jkent

  ==============================================================================
*/

#ifndef PUBLICKEY_H_INCLUDED
#define PUBLICKEY_H_INCLUDED

#include "nettle/rsa.h"

enum revoked_status {
    REVOKED_UNKNOWN = 0,
    REVOKED_NO,
    REVOKED_YES
};

class PublicKey
{
public:
    PublicKey();
    PublicKey(const PublicKey &other);
    PublicKey(const char *buf, int length);
    PublicKey(const File &path);
    ~PublicKey();

    bool isLoaded();
    bool isValid();
    bool isRevoked();
    const String & getFingerprint();    
    
private:
    bool loaded;
    enum revoked_status revoked;
    File keyFile;
    struct rsa_public_key publicKey;
    String fingerprint;

    bool loadSexp(const char *buf, int length);
    bool loadSexp(const File &file);
    
    friend class Crypto;
};

#endif  // PUBLICKEY_H_INCLUDED